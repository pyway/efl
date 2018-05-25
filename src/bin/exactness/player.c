#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#define _POSIX_
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

#include <sys/types.h>
#ifdef HAVE_SYS_SYSINFO_H
# include <sys/sysinfo.h>
#endif

#ifndef EFL_EO_API_SUPPORT
#define EFL_EO_API_SUPPORT
#endif
#include <Eina.h>
#include <Eo.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Getopt.h>
#include <Ecore_File.h>
#include <Ecore_Con.h>
#include <Elementary.h>
#include <Exactness.h>

#include "exactness_private.h"

#define PATH_ 1024
#define IMAGE_FILENAME_EXT ".png"

typedef struct
{
   Eina_Debug_Session *session;
   int srcid;
   void *buffer;
   unsigned int size;
} _Main_Loop_Info;

#define WRAPPER_TO_XFER_MAIN_LOOP(foo) \
static void \
_intern_main_loop ## foo(void *data) \
{ \
   _Main_Loop_Info *info = data; \
   _main_loop ## foo(info->session, info->srcid, info->buffer, info->size); \
   free(info->buffer); \
   free(info); \
} \
static Eina_Bool \
foo(Eina_Debug_Session *session, int srcid, void *buffer, int size) \
{ \
   _Main_Loop_Info *info = calloc(1, sizeof(*info)); \
   info->session = session; \
   info->srcid = srcid; \
   info->size = size; \
   if (info->size) \
     { \
        info->buffer = malloc(info->size); \
        memcpy(info->buffer, buffer, info->size); \
     } \
   ecore_main_loop_thread_safe_call_async(_intern_main_loop ## foo, info); \
   return EINA_TRUE; \
}

#ifndef WORDS_BIGENDIAN
#define SWAP_64(x) x
#define SWAP_32(x) x
#define SWAP_16(x) x
#define SWAP_DBL(x) x
#else
#define SWAP_64(x) eina_swap64(x)
#define SWAP_32(x) eina_swap32(x)
#define SWAP_16(x) eina_swap16(x)
#define SWAP_DBL(x) SWAP_64(x)
#endif

#define EXTRACT_INT(_buf) \
({ \
   int __i; \
   memcpy(&__i, _buf, sizeof(int)); \
   _buf += sizeof(int); \
   SWAP_32(__i); \
})

#define EXTRACT_DOUBLE(_buf) \
({ \
   double __d; \
   memcpy(&__d, _buf, sizeof(double)); \
   _buf += sizeof(double); \
   SWAP_DBL(__d); \
})

#define EXTRACT_STRING(_buf) \
({ \
   char *__s = _buf ? strdup(_buf) : NULL; \
   int __len = (__s ? strlen(__s) : 0) + 1; \
   _buf += __len; \
   __s; \
})

typedef enum
{
   FTYPE_UNKNOWN,
   FTYPE_DIR,
   FTYPE_REC = FTYPE_DIR,
   FTYPE_EXU,
   FTYPE_REMOTE
} File_Type;

static File_Type _dest_type = FTYPE_UNKNOWN;
static Eina_Stringshare *_dest = NULL;
static Exactness_Unit *_dest_unit = NULL;

static File_Type _src_type = FTYPE_UNKNOWN;
static Eina_Stringshare *_src_filename = NULL;
static Exactness_Unit *_src_unit = NULL;

static const char *_test_name = NULL;
static int _verbose = 0;

static Evas *(*_evas_new)(void) = NULL;
static Eina_List *_evas_list = NULL;

static Eina_List *_cur_event_list = NULL;

static int _cur_shot_id = 0;
static Eina_Bool _shot_needed = EINA_FALSE;
static Eina_Bool _scan_objects = EINA_FALSE, _disable_shots = EINA_FALSE;

static Eina_Bool _stabilization_timer_cb(void *);

static Eina_Bool _exit_required = EINA_FALSE;

static void
_printf(int verbose, const char *fmt, ...)
{
   va_list ap;
   if (!_verbose || verbose > _verbose) return;

   va_start(ap, fmt);
   vprintf(fmt, ap);
   va_end(ap);
}

static void
_shot_do(Evas *e)
{
   Ecore_Evas *ee_orig;
   int w, h;

   if (!e) return;

   ee_orig = ecore_evas_ecore_evas_get(e);

   ecore_evas_geometry_get(ee_orig, NULL, NULL, &w, &h);
   if ((w < 1) || (h < 1)) return;

   if (!_disable_shots)
     {
        void *e_data;
        Evas_Object *snapshot = evas_object_image_filled_add(e);
        if (!snapshot) return;
        evas_object_image_snapshot_set(snapshot, EINA_TRUE);

        evas_object_geometry_set(snapshot, 0, 0, w, h);
        efl_gfx_entity_visible_set(snapshot, EINA_TRUE);

        if (_dest_type == FTYPE_DIR)
          {
             int dir_name_len;
             char *filename;

             dir_name_len = strlen(_dest) + 1; /* includes space of a '/' */
             filename = malloc(strlen(_test_name) + strlen(IMAGE_FILENAME_EXT) +
                   dir_name_len + 8); /* also space for serial */

             sprintf(filename, "%s/%s%c%03d%s", _dest, _test_name,
                   SHOT_DELIMITER, _cur_shot_id, IMAGE_FILENAME_EXT);
             e_data = filename;
          }
        else if (_dest_type == FTYPE_EXU)
          {
             Exactness_Image *ex_img = malloc(sizeof(*ex_img));
             ex_img->w = w;
             ex_img->h = h;
             _dest_unit->imgs = eina_list_append(_dest_unit->imgs, ex_img);
             _dest_unit->nb_shots++;
             e_data = ex_img;
          }
        efl_key_data_set(e, "_shot", e_data);
        efl_key_data_set(e, "_snapshot", snapshot);
        _shot_needed = EINA_TRUE;
     }

   if (_scan_objects && _dest_type == FTYPE_EXU)
     {
        Eina_Iterator *iter;
        Eo *obj;
        Exactness_Objects *e_objs = calloc(1, sizeof(*e_objs));
        iter = eo_objects_iterator_new();
        EINA_ITERATOR_FOREACH(iter, obj)
          {
             if (!efl_isa(obj, EFL_CANVAS_OBJECT_CLASS) &&
                   !efl_isa(obj, EFL_CANVAS_SCENE_INTERFACE)) continue;
             Exactness_Object *e_obj = calloc(1, sizeof(*e_obj));
             Eo *parent = efl_parent_get(obj);
             e_obj->id = (long long) obj;
             if (efl_isa(parent, EFL_CANVAS_OBJECT_CLASS) ||
                   efl_isa(parent, EFL_CANVAS_SCENE_INTERFACE))
               {
                  e_obj->parent_id = (long long) efl_parent_get(obj);
               }
             else
               {
                  e_obj->parent_id = 0;
               }
             e_obj->kl_name = eina_stringshare_add(efl_class_name_get(obj));
             if (efl_isa(obj, EFL_CANVAS_OBJECT_CLASS))
               {
                  Eina_Size2D sz = efl_gfx_entity_size_get(obj);
                  e_obj->w = sz.w;
                  e_obj->h = sz.h;
                  Eina_Position2D pos = efl_gfx_entity_position_get(obj);
                  e_obj->x = pos.x;
                  e_obj->y = pos.y;
               }
             e_objs->objs = eina_list_append(e_objs->objs, e_obj);
          }
        eina_iterator_free(iter);
        _dest_unit->objs = eina_list_append(_dest_unit->objs, e_objs);
     }
}

static Exactness_Image *
_sync_shot_get(Evas *e)
{
   Exactness_Image *ex_img;
   Ecore_Evas *ee_orig;
   unsigned int *pixels;
   int w, h, nb_bytes;

   if (!e) return NULL;

   ee_orig = ecore_evas_ecore_evas_get(e);

   ecore_evas_manual_render(ee_orig);
   pixels = (void *)ecore_evas_buffer_pixels_get(ee_orig);
   if (!pixels) return NULL;

   ecore_evas_geometry_get(ee_orig, NULL, NULL, &w, &h);
   if ((w < 1) || (h < 1)) return NULL;

   ex_img = malloc(sizeof(*ex_img));
   nb_bytes = w * h * 4;
   ex_img->w = w;
   ex_img->h = h;
   ex_img->pixels = malloc(nb_bytes);
   memcpy(ex_img->pixels, pixels, nb_bytes);
   return ex_img;
}

static void
_feed_event(Exactness_Action_Type type, unsigned int n_evas, void *data)
{
   static Evas_Object *rect = NULL;
   static unsigned int rect_evas;

   Eo *e = eina_list_nth(_evas_list, n_evas);

   if (rect && rect_evas != n_evas)
     {
        efl_del(rect);
        rect = NULL;
     }
   if (_verbose && !rect)
     {
        rect = evas_object_rectangle_add(e);
        evas_object_repeat_events_set(rect, EINA_TRUE);
        evas_object_color_set(rect, 255, 0, 0, 255);
        evas_object_resize(rect, 15, 15);
        evas_object_layer_set(rect, 100);
        evas_object_show(rect);
        rect_evas = n_evas;
     }

   switch (type)
     {
      case EXACTNESS_ACTION_MOUSE_IN:
           {
              _printf(1, "Mouse in\n");
              _printf(2, "%s evas_event_feed_mouse_in n_evas=<%d>\n", __func__, n_evas);
              if (e) evas_event_feed_mouse_in(e, time(NULL), NULL);
              break;
           }
      case EXACTNESS_ACTION_MOUSE_OUT:
           {
              _printf(1, "Mouse out\n");
              _printf(2, "%s evas_event_feed_mouse_out n_evas=<%d>\n", __func__, n_evas);
              if (e) evas_event_feed_mouse_out(e, time(NULL), NULL);
              break;
           }
      case EXACTNESS_ACTION_MOUSE_WHEEL:
           {
              Exactness_Action_Mouse_Wheel *t = data;
              _printf(1, "Mouse wheel\n");
              _printf(2, "%s evas_event_feed_mouse_wheel n_evas=<%d>\n", __func__, n_evas);
              if (e) evas_event_feed_mouse_wheel(e, t->direction, t->z, time(NULL), NULL);

              break;
           }
      case EXACTNESS_ACTION_MULTI_DOWN:
           {
              Exactness_Action_Multi_Event *t = data;
              _printf(2, "%s evas_event_feed_multi_down n_evas=<%d>\n", __func__, n_evas);
              if (!t->d)
                {
                   if (e) evas_event_feed_mouse_down(e, t->b, t->flags, time(NULL), NULL);
                   if (rect) evas_object_color_set(rect, 255, 255, 0, 255);
                }
              else
                {
                   if (e) evas_event_feed_multi_down(e,
                         t->d, t->x, t->y, t->rad,
                         t->radx, t->rady, t->pres, t->ang, t->fx, t->fy,
                         t->flags, time(NULL), NULL);
                }

              break;
           }
      case EXACTNESS_ACTION_MULTI_UP:
           {
              Exactness_Action_Multi_Event *t = data;
              _printf(2, "%s evas_event_feed_multi_up n_evas=<%d>\n", __func__, n_evas);
              if (!t->d)
                {
                   if (e) evas_event_feed_mouse_up(e, t->b, t->flags, time(NULL), NULL);
                   if (rect) evas_object_color_set(rect, 255, 0, 0, 255);
                }
              else
                {
                   if (e) evas_event_feed_multi_up(e,
                         t->d, t->x, t->y, t->rad,
                         t->radx, t->rady, t->pres, t->ang, t->fx, t->fy,
                         t->flags, time(NULL), NULL);
                }

              break;
           }
      case EXACTNESS_ACTION_MULTI_MOVE:
           {
              Exactness_Action_Multi_Move *t = data;
              _printf(2, "%s evas_event_feed_multi_move n_evas=<%d>\n", __func__, n_evas);
              if (!t->d)
                {
                   if (e) evas_event_feed_mouse_move(e, t->x, t->y, time(NULL), NULL);
                   if (rect)
                     {
                        evas_object_move(rect, t->x, t->y);
                        evas_object_color_set(rect, 255, 0, 0, 255);
                     }
                }
              else
                {
                   if (e) evas_event_feed_multi_move(e,
                         t->d, t->x, t->y, t->rad,
                         t->radx, t->rady, t->pres, t->ang, t->fx, t->fy,
                         time(NULL), NULL);
                }

              break;
           }
      case EXACTNESS_ACTION_KEY_DOWN:
           {
              Exactness_Action_Key_Down_Up *t = data;
              _printf(2, "%s evas_event_feed_key_down n_evas=<%d>\n", __func__, n_evas);
              if (e)
                 evas_event_feed_key_down_with_keycode(e,
                       t->keyname, t->key, t->string,
                       t->compose, time(NULL), NULL, t->keycode);
              break;
           }
      case EXACTNESS_ACTION_KEY_UP:
           {
              Exactness_Action_Key_Down_Up *t = data;
              _printf(2, "%s evas_event_feed_key_up n_evas=<%d>\n", __func__, n_evas);
              if (e) evas_event_feed_key_up_with_keycode(e,
                    t->keyname, t->key, t->string,
                    t->compose, time(NULL), NULL, t->keycode);

              break;
           }
      case EXACTNESS_ACTION_TAKE_SHOT:
           {
              _printf(2, "%s take shot n_evas=<%d>\n", __func__, n_evas);
              if (rect) evas_object_color_set(rect, 0, 0, 255, 255);
              _cur_shot_id++;
              if (_dest_type != FTYPE_UNKNOWN && e) _shot_do(e);
              break;
           }
      case EXACTNESS_ACTION_EFL_EVENT:
           {
              Exactness_Action_Efl_Event *t = data;
              Eina_Bool found = EINA_FALSE;
              Eina_List *itr;
              EINA_LIST_FOREACH(_evas_list, itr, e)
                {
                   Eo *o = efl_name_find(e, t->wdg_name);
                   if (o)
                     {
                        _printf(2, "%s EFL event invoke %s on %s\n",
                              __func__, t->event_name, t->wdg_name);
                        Efl_Event_Description d;
                        found = EINA_TRUE;
                        memset(&d, 0, sizeof(d));
                        d.name = t->event_name;
                        d.legacy_is = EINA_TRUE;
                        efl_event_callback_legacy_call(o, &d, NULL);
#if 0
                        /* Remove when events stuff work well */
                        Eina_Size2D sz = efl_gfx_size_get(o);
                        Eina_Position2D pos = efl_gfx_position_get(o);
                        if (!strcmp(t->event_name, "clicked") ||
                              !strcmp(t->event_name, "clicked,double"))
                          {
                             int x = pos.x + (sz.w / 2);
                             int y = pos.y + (sz.h / 2);
                             evas_event_feed_mouse_move(e, x, y, time(NULL), NULL);
                             evas_event_feed_mouse_down(e, 0, EVAS_BUTTON_NONE, time(NULL), NULL);
                             evas_event_feed_mouse_up(e, 0, EVAS_BUTTON_NONE, time(NULL), NULL);
                             if (rect)
                               {
                                  evas_object_move(rect, x, y);
                                  evas_object_color_set(rect, 255, 0, 0, 255);
                               }
                              if (!strcmp(t->event_name, "clicked,double"))
                                {
                                   evas_event_feed_mouse_down(e, 0, EVAS_BUTTON_DOUBLE_CLICK,
                                         time(NULL), NULL);
                                   evas_event_feed_mouse_up(e, 0, EVAS_BUTTON_DOUBLE_CLICK,
                                         time(NULL), NULL);
                                }
                          }
#endif
                     }
                }
              if (!found) fprintf(stderr, "Failed finding %s.\n", t->wdg_name);
              break;
           }
      case EXACTNESS_ACTION_STABILIZE:
           {
              _printf(2, "%s stabilize\n", __func__);
              if (rect) evas_object_color_set(rect, 255, 165, 0, 255);
              ecore_timer_add(0.1, _stabilization_timer_cb, NULL);
              break;
           }
      default: /* All non-input events are not handeled */
         break;
     }
}

static Eina_Bool
_feed_event_timer_cb(void *data EINA_UNUSED)
{
   Exactness_Action *act = eina_list_data_get(_cur_event_list);
   _feed_event(act->type, act->n_evas, act->data);

   _cur_event_list = eina_list_next(_cur_event_list);
   if (!_cur_event_list)
     {  /* Finished reading all events */
        _exit_required = EINA_TRUE;
        if (!_shot_needed) ecore_main_loop_quit();
     }
   else
     {
        if (act->type != EXACTNESS_ACTION_STABILIZE)
          {
             act = eina_list_data_get(_cur_event_list);
             _printf(2, "  %s timer_time=<%f>\n", __func__, act->delay_ms / 1000.0);
             ecore_timer_add(act->delay_ms / 1000.0, _feed_event_timer_cb, NULL);
          }
     }
   return ECORE_CALLBACK_CANCEL;
}

static Eina_Bool
_stabilization_timer_cb(void *data EINA_UNUSED)
{
   Eina_List *itr;
   Evas *e;
   Eina_Bool need_more = EINA_FALSE;
   EINA_LIST_FOREACH(_evas_list, itr, e)
     {
        Exactness_Image *last_img = efl_key_data_get(e, "_last_shot");
        Exactness_Image *cur_img = _sync_shot_get(e);
        if (!last_img || exactness_image_compare(last_img, cur_img, NULL)) need_more = EINA_TRUE;
        exactness_image_free(last_img);
        efl_key_data_set(e, "_last_shot", cur_img);
     }
   EINA_LIST_FOREACH(_evas_list, itr, e)
     {
        if (!need_more)
          {
             Exactness_Image *last_img = efl_key_data_get(e, "_last_shot");
             exactness_image_free(last_img);
             efl_key_data_set(e, "_last_shot", NULL);
          }
     }
   if (!need_more)
     {
        if (_src_type != FTYPE_REMOTE)
          {
             Exactness_Action *act = eina_list_data_get(_cur_event_list);
             _printf(2, "  %s timer_time=<%f>\n", __func__, act->delay_ms / 1000.0);
             ecore_timer_add(act->delay_ms / 1000.0, _feed_event_timer_cb, NULL);
          }
        return ECORE_CALLBACK_CANCEL;
     }
   else return ECORE_CALLBACK_RENEW;
}

static void
_evas_render_post_cb(void *data EINA_UNUSED, const Efl_Event *event)
{
   if (_shot_needed)
     {
        Evas_Event_Render_Post *post = event->info;
        void *e_data = efl_key_data_get(event->object, "_shot");
        Evas_Object *snapshot = efl_key_data_get(event->object, "_snapshot");
        void *pixels;
        int w, h;

        // Nothing was updated, so let's not bother sending nothingness
        if (!post->updated_area) return;
        pixels = evas_object_image_data_get(snapshot, EINA_FALSE);
        if (!pixels) return;
        evas_object_geometry_get(snapshot, NULL, NULL, &w, &h);

        if (_dest_type == FTYPE_DIR)
          {
             char *filename = e_data;
             Eo *o = evas_object_image_add(event->object);
             evas_object_image_size_set(o, w, h);
             evas_object_image_data_set(o, pixels);
             _printf(1, "Shot taken (%s).\n", filename);
             if (!evas_object_image_save(o, filename, NULL, NULL))
               {
                  printf("Cannot save widget to <%s>\n", filename);
               }
             free(filename);
          }
        else if (_dest_type == FTYPE_EXU)
          {
             int nb_bytes = w * h * 4;
             Exactness_Image *ex_img = e_data;
             ex_img->pixels = malloc(nb_bytes);
             memcpy(ex_img->pixels, pixels, nb_bytes);
             _printf(1, "Shot taken (in %s).\n", _dest);
          }
        efl_key_data_set(event->object, "_shot", NULL);
        evas_object_del(snapshot);
        /* This part is needed when the shot is needed at the end of the scenario.
         * As it is async, we need to wait for the shot termination. */
        _shot_needed = EINA_FALSE;
        if (_exit_required) ecore_main_loop_quit();
     }
}

static void
_main_loop_mouse_in_cb(Eina_Debug_Session *session EINA_UNUSED, int srcid EINA_UNUSED, void *buffer, int size EINA_UNUSED)
{
   char *buf = buffer;
   int n_evas = EXTRACT_INT(buf);
   _feed_event(EXACTNESS_ACTION_MOUSE_IN, n_evas, NULL);
}

static void
_main_loop_mouse_out_cb(Eina_Debug_Session *session EINA_UNUSED, int srcid EINA_UNUSED, void *buffer, int size EINA_UNUSED)
{
   char *buf = buffer;
   int n_evas = EXTRACT_INT(buf);
   _feed_event(EXACTNESS_ACTION_MOUSE_OUT, n_evas, NULL);
}

static void
_main_loop_mouse_wheel_cb(Eina_Debug_Session *session EINA_UNUSED, int srcid EINA_UNUSED, void *buffer, int size EINA_UNUSED)
{
   char *buf = buffer;
   Exactness_Action_Mouse_Wheel t;
   int n_evas = EXTRACT_INT(buf);
   t.direction = EXTRACT_INT(buf);
   t.z = EXTRACT_INT(buf);
   _feed_event(EXACTNESS_ACTION_MOUSE_WHEEL, n_evas, &t);
}

static void
_main_loop_multi_down_cb(Eina_Debug_Session *session EINA_UNUSED, int srcid EINA_UNUSED, void *buffer, int size EINA_UNUSED)
{
   char *buf = buffer;
   Exactness_Action_Multi_Event t;
   int n_evas = EXTRACT_INT(buf);
   t.d = EXTRACT_INT(buf);
   t.b = EXTRACT_INT(buf);
   t.x = EXTRACT_INT(buf);
   t.y = EXTRACT_INT(buf);
   t.rad = EXTRACT_DOUBLE(buf);
   t.radx = EXTRACT_DOUBLE(buf);
   t.rady = EXTRACT_DOUBLE(buf);
   t.pres = EXTRACT_DOUBLE(buf);
   t.ang = EXTRACT_DOUBLE(buf);
   t.fx = EXTRACT_DOUBLE(buf);
   t.fy = EXTRACT_DOUBLE(buf);
   t.flags = EXTRACT_INT(buf);
   _feed_event(EXACTNESS_ACTION_MULTI_DOWN, n_evas, &t);
}

static void
_main_loop_multi_up_cb(Eina_Debug_Session *session EINA_UNUSED, int srcid EINA_UNUSED, void *buffer, int size EINA_UNUSED)
{
   char *buf = buffer;
   Exactness_Action_Multi_Event t;
   int n_evas = EXTRACT_INT(buf);
   t.d = EXTRACT_INT(buf);
   t.b = EXTRACT_INT(buf);
   t.x = EXTRACT_INT(buf);
   t.y = EXTRACT_INT(buf);
   t.rad = EXTRACT_DOUBLE(buf);
   t.radx = EXTRACT_DOUBLE(buf);
   t.rady = EXTRACT_DOUBLE(buf);
   t.pres = EXTRACT_DOUBLE(buf);
   t.ang = EXTRACT_DOUBLE(buf);
   t.fx = EXTRACT_DOUBLE(buf);
   t.fy = EXTRACT_DOUBLE(buf);
   t.flags = EXTRACT_INT(buf);
   _feed_event(EXACTNESS_ACTION_MULTI_UP, n_evas, &t);
}

static void
_main_loop_multi_move_cb(Eina_Debug_Session *session EINA_UNUSED, int srcid EINA_UNUSED, void *buffer, int size EINA_UNUSED)
{
   char *buf = buffer;
   Exactness_Action_Multi_Move t;
   int n_evas = EXTRACT_INT(buf);
   t.d = EXTRACT_INT(buf);
   t.x = EXTRACT_INT(buf);
   t.y = EXTRACT_INT(buf);
   t.rad = EXTRACT_DOUBLE(buf);
   t.radx = EXTRACT_DOUBLE(buf);
   t.rady = EXTRACT_DOUBLE(buf);
   t.pres = EXTRACT_DOUBLE(buf);
   t.ang = EXTRACT_DOUBLE(buf);
   t.fx = EXTRACT_DOUBLE(buf);
   t.fy = EXTRACT_DOUBLE(buf);
   _feed_event(EXACTNESS_ACTION_MULTI_MOVE, n_evas, &t);
}

static void
_main_loop_key_down_cb(Eina_Debug_Session *session EINA_UNUSED, int srcid EINA_UNUSED, void *buffer, int size EINA_UNUSED)
{
   char *buf = buffer;
   Exactness_Action_Key_Down_Up t;
   int n_evas = EXTRACT_INT(buf);
   t.keyname = EXTRACT_STRING(buf);
   t.key = EXTRACT_STRING(buf);
   t.string = EXTRACT_STRING(buf);
   t.compose = EXTRACT_STRING(buf);
   t.keycode = EXTRACT_INT(buf);
   _feed_event(EXACTNESS_ACTION_KEY_DOWN, n_evas, &t);
}

static void
_main_loop_key_up_cb(Eina_Debug_Session *session EINA_UNUSED, int srcid EINA_UNUSED, void *buffer, int size EINA_UNUSED)
{
   char *buf = buffer;
   Exactness_Action_Key_Down_Up t;
   int n_evas = EXTRACT_INT(buf);
   t.keyname = EXTRACT_STRING(buf);
   t.key = EXTRACT_STRING(buf);
   t.string = EXTRACT_STRING(buf);
   t.compose = EXTRACT_STRING(buf);
   t.keycode = EXTRACT_INT(buf);
   _feed_event(EXACTNESS_ACTION_KEY_UP, n_evas, &t);
}

static void
_main_loop_take_shot_cb(Eina_Debug_Session *session EINA_UNUSED, int srcid EINA_UNUSED, void *buffer, int size EINA_UNUSED)
{
   char *buf = buffer;
   int n_evas = EXTRACT_INT(buf);
   _feed_event(EXACTNESS_ACTION_TAKE_SHOT, n_evas, NULL);
}

static void
_main_loop_efl_event_cb(Eina_Debug_Session *session EINA_UNUSED, int srcid EINA_UNUSED, void *buffer, int size EINA_UNUSED)
{
   char *buf = buffer;
   Exactness_Action_Efl_Event t;
   t.wdg_name = EXTRACT_STRING(buf);
   t.event_name = EXTRACT_STRING(buf);
   _feed_event(EXACTNESS_ACTION_EFL_EVENT, 0, &t);
}

static void
_main_loop_stabilize_cb(Eina_Debug_Session *session EINA_UNUSED, int srcid EINA_UNUSED, void *buffer EINA_UNUSED, int size EINA_UNUSED)
{
   _feed_event(EXACTNESS_ACTION_STABILIZE, 0, NULL);
}

static void
_main_loop_finish_cb(Eina_Debug_Session *session EINA_UNUSED, int srcid EINA_UNUSED, void *buffer EINA_UNUSED, int size EINA_UNUSED)
{
   ecore_main_loop_quit();
}

WRAPPER_TO_XFER_MAIN_LOOP(_mouse_in_cb)
WRAPPER_TO_XFER_MAIN_LOOP(_mouse_out_cb)
WRAPPER_TO_XFER_MAIN_LOOP(_mouse_wheel_cb)
WRAPPER_TO_XFER_MAIN_LOOP(_multi_down_cb)
WRAPPER_TO_XFER_MAIN_LOOP(_multi_up_cb)
WRAPPER_TO_XFER_MAIN_LOOP(_multi_move_cb)
WRAPPER_TO_XFER_MAIN_LOOP(_key_down_cb)
WRAPPER_TO_XFER_MAIN_LOOP(_key_up_cb)
WRAPPER_TO_XFER_MAIN_LOOP(_take_shot_cb)
WRAPPER_TO_XFER_MAIN_LOOP(_efl_event_cb)
WRAPPER_TO_XFER_MAIN_LOOP(_stabilize_cb)
WRAPPER_TO_XFER_MAIN_LOOP(_finish_cb)

EINA_DEBUG_OPCODES_ARRAY_DEFINE(_debug_ops,
     {"Exactness/Actions/Mouse In", NULL, &_mouse_in_cb},
     {"Exactness/Actions/Mouse Out", NULL, &_mouse_out_cb},
     {"Exactness/Actions/Mouse Wheel", NULL, &_mouse_wheel_cb},
     {"Exactness/Actions/Multi Down", NULL, &_multi_down_cb},
     {"Exactness/Actions/Multi Up", NULL, &_multi_up_cb},
     {"Exactness/Actions/Multi Move", NULL, &_multi_move_cb},
     {"Exactness/Actions/Key Down", NULL, &_key_down_cb},
     {"Exactness/Actions/Key Up", NULL, &_key_up_cb},
     {"Exactness/Actions/Take Shot", NULL, &_take_shot_cb},
     {"Exactness/Actions/EFL Event", NULL, &_efl_event_cb},
     {"Exactness/Actions/Stabilize", NULL, &_stabilize_cb},
     {"Exactness/Actions/Finish", NULL, &_finish_cb},
     {NULL, NULL, NULL}
);

static Eina_Bool
_src_feed(void *data EINA_UNUSED)
{
   if (!_evas_list) return EINA_TRUE;
   _cur_event_list = _src_unit->actions;
   Exactness_Action *act = eina_list_data_get(_cur_event_list);

   if (act->delay_ms)
     {
        _printf(2, "  Waiting <%f>\n", act->delay_ms / 1000.0);
        ecore_timer_add(act->delay_ms / 1000.0, _feed_event_timer_cb, NULL);
     }
   else
     {
        _feed_event_timer_cb(NULL);
     }
   return EINA_FALSE;
}

static Eina_Bool
_src_open()
{
   if (_src_type != FTYPE_REMOTE)
     {
        Eina_List *itr, *itr2;
        Exactness_Action *act;
        _printf(2, "<%s> Source file is <%s>\n", __func__, _src_filename);
        if (_src_type == FTYPE_EXU)
          {
             _src_unit = exactness_unit_file_read(_src_filename);
          }
        else if (_src_type == FTYPE_REC)
          {
             _src_unit = legacy_rec_file_read(_src_filename);
          }
        if (!_src_unit) return EINA_FALSE;
        EINA_LIST_REVERSE_FOREACH_SAFE(_src_unit->actions, itr, itr2, act)
          {
             if (act->type == EXACTNESS_ACTION_TAKE_SHOT) break;
             _src_unit->actions = eina_list_remove(_src_unit->actions, act);
          }
     }
   else
     {
        eina_debug_opcodes_register(NULL, _debug_ops(), NULL, NULL);
     }
   return EINA_TRUE;
}

static int
_prg_invoke(const char *full_path, int argc, char **argv)
{
   Eina_Value *ret__;
   int real__;

   void (*efl_main)(void *data, const Efl_Event *ev);
   int (*elm_main)(int argc, char **argv);
   int (*c_main)(int argc, char **argv);
   Eina_Module *h = eina_module_new(full_path);
   if (!h || !eina_module_load(h))
     {
        fprintf(stderr, "Failed loading %s.\n", full_path);
        if (h) eina_module_free(h);
        return 1;
     }
   efl_main = eina_module_symbol_get(h, "efl_main");
   elm_main = eina_module_symbol_get(h, "elm_main");
   c_main = eina_module_symbol_get(h, "main");
   _evas_new = eina_module_symbol_get(h, "evas_new");
   if (!_evas_new)
     {
        fprintf(stderr, "Failed loading symbol 'evas_new' from %s.\n", full_path);
        eina_module_free(h);
        return 1;
     }

   if (_src_open())
     {
        if (efl_main)
          {
             elm_init(argc, argv);
             elm_theme_overlay_add(NULL, DATA_DIR"/exactness_play.edj");
             efl_event_callback_add(efl_main_loop_get(), EFL_LOOP_EVENT_ARGUMENTS, efl_main, NULL);
             ret__ = efl_loop_begin(efl_main_loop_get());
             real__ = efl_loop_exit_code_process(ret__);
             elm_shutdown();
          }
        else if (elm_main)
          {
             elm_init(argc, argv);
             elm_theme_overlay_add(NULL, DATA_DIR"/exactness_play.edj");
             real__ = elm_main(argc, argv);
             elm_shutdown();
          }
        else if (c_main)
          {
             real__ = c_main(argc, argv);
          }
        else
          {
             fprintf(stderr, "Failed loading symbol 'efl_main', 'elm_main' or 'main' from %s.\n", full_path);
             eina_module_free(h);
             real__ = 1;
          }
     }
   else real__ = 1;

   return real__;
}

static Eina_Stringshare *
_prg_full_path_guess(const char *prg)
{
   char full_path[PATH_];
   if (strchr(prg, '/')) return eina_stringshare_add(prg);
   char *paths = strdup(getenv("PATH"));
   Eina_Stringshare *ret = NULL;
   while (paths && *paths && !ret)
     {
        char *real_path;
        char *colon = strchr(paths, ':');
        if (colon) *colon = '\0';

        sprintf(full_path, "%s/%s", paths, prg);
        real_path = ecore_file_realpath(full_path);
        if (*real_path)
          {
             ret = eina_stringshare_add(real_path);
             // check if executable
          }
        free(real_path);

        paths += strlen(paths);
        if (colon) paths++;
     }
   return ret;
}

static Eina_Bool
_mkdir(const char *path, Eina_Bool skip_last)
{
   if (!ecore_file_exists(path))
     {
        const char *cur = path + 1;
        do
          {
             char *slash = strchr(cur, '/');
             if (slash) *slash = '\0';
             else if (skip_last) return EINA_TRUE;
             if (!ecore_file_exists(path) && !ecore_file_mkdir(path)) return EINA_FALSE;
             if (slash) *slash = '/';
             if (slash) cur = slash + 1;
             else cur = NULL;
          }
        while (cur);
     }
   return EINA_TRUE;
}

static Evas *
_my_evas_new(int w EINA_UNUSED, int h EINA_UNUSED)
{
   Evas *e;
   if (!_evas_new) return NULL;
   e = _evas_new();
   if (e)
     {
        _printf(1, "New Evas\n");
        _evas_list = eina_list_append(_evas_list, e);
        efl_event_callback_add(e, EFL_CANVAS_SCENE_EVENT_RENDER_POST, _evas_render_post_cb, NULL);
     }
   return e;
}

static const Ecore_Getopt optdesc = {
  "exactness_play",
  "%prog [options] <-s|-o|-v|-t|-h> command",
  PACKAGE_VERSION,
  "(C) 2017 Enlightenment",
  "BSD",
  "A scenario player for EFL based applications.",
  1,
  {
    ECORE_GETOPT_STORE_STR('o', "output",
          " Set the destination for the shots.\n"
          " If a .exu is given, the shots are stored in the file.\n"
          " Otherwise the given path is considered as a directory\n"
          " where shots will be stored.\n"
          " If omitted, the output type (exu or dir) is determined\n"
          " by the given test extension (resp. exu or rec)."),
    ECORE_GETOPT_STORE_STR('t', "test", "Test to run on the given application"),
    ECORE_GETOPT_STORE_TRUE('s', "show-on-screen", "Show on screen."),
    ECORE_GETOPT_STORE_TRUE(0, "scan-objects", "Extract information of all the objects at every shot."),
    ECORE_GETOPT_STORE_TRUE(0, "external-injection", "Expect events injection via Eina debug channel."),
    ECORE_GETOPT_STORE_TRUE(0, "disable-screenshots", "Disable screenshots."),
    ECORE_GETOPT_COUNT('v', "verbose", "Turn verbose messages on."),

    ECORE_GETOPT_LICENSE('L', "license"),
    ECORE_GETOPT_COPYRIGHT('C', "copyright"),
    ECORE_GETOPT_VERSION('V', "version"),
    ECORE_GETOPT_HELP('h', "help"),
    ECORE_GETOPT_SENTINEL
  }
};

int main(int argc, char **argv)
{
   int pret = 1, opt_args = 0;
   char *src = NULL, *dest = NULL, *eq;
   Eina_Bool show_on_screen = EINA_FALSE;
   Eina_Bool want_quit = EINA_FALSE, external_injection = EINA_FALSE;

   Ecore_Getopt_Value values[] = {
     ECORE_GETOPT_VALUE_STR(dest),
     ECORE_GETOPT_VALUE_STR(src),
     ECORE_GETOPT_VALUE_BOOL(show_on_screen),
     ECORE_GETOPT_VALUE_BOOL(_scan_objects),
     ECORE_GETOPT_VALUE_BOOL(external_injection),
     ECORE_GETOPT_VALUE_BOOL(_disable_shots),
     ECORE_GETOPT_VALUE_INT(_verbose),

     ECORE_GETOPT_VALUE_BOOL(want_quit),
     ECORE_GETOPT_VALUE_BOOL(want_quit),
     ECORE_GETOPT_VALUE_BOOL(want_quit),
     ECORE_GETOPT_VALUE_BOOL(want_quit),
     ECORE_GETOPT_VALUE_NONE
   };

   eina_init();
   eet_init();
   ecore_init();

   opt_args = ecore_getopt_parse(&optdesc, values, argc, argv);
   if (opt_args < 0)
     {
        fprintf(stderr, "Failed parsing arguments.\n");
        goto end;
     }
   if (want_quit) goto end;

   /* Check for a sentinel */
   if (argv[opt_args] && !strcmp(argv[opt_args], "--")) opt_args++;

   /* Check for env variables */
   do
     {
        eq = argv[opt_args] ? strchr(argv[opt_args], '=') : NULL;
        if (eq)
          {
             char *var = malloc(eq - argv[opt_args] + 1);
             memcpy(var, argv[opt_args], eq - argv[opt_args]);
             var[eq - argv[opt_args]] = '\0';
             setenv(var, eq + 1, 1);
             opt_args++;
          }
     } while (eq);

   if (dest)
     {
        _dest = eina_stringshare_add(dest);
        if (!strcmp(_dest + strlen(_dest) - 4,".exu"))
          {
             _dest_type = FTYPE_EXU;
             if (!_mkdir(_dest, EINA_TRUE))
               {
                  fprintf(stderr, "Path for %s cannot be created\n", _dest);
                  goto end;
               }
          }
        else
          {
             _dest_type = FTYPE_DIR;
             if (!_mkdir(_dest, EINA_FALSE))
               {
                  fprintf(stderr, "Directory %s cannot be created\n", _dest);
                  goto end;
               }
          }
     }
   if (!src && !external_injection)
     {
        fprintf(stderr, "no test file specified\n");
        goto end;
     }
   if (src && external_injection)
     {
        fprintf(stderr, "Cannot inject events from a source file and from outside simultaneously\n");
        goto end;
     }
   if (external_injection) _src_type = FTYPE_REMOTE;
   if (src)
     {
        _src_filename = eina_stringshare_add(src);
        if (!strcmp(_src_filename + strlen(_src_filename) - 4,".exu"))
          {
             _src_type = FTYPE_EXU;
             if (_dest_type == FTYPE_UNKNOWN)
               {
                  _dest_type = FTYPE_EXU;
                  _dest = "./output.exu";
               }
          }
        else if (!strcmp(_src_filename + strlen(_src_filename) - 4,".rec"))
          {
             _src_type = FTYPE_REC;
             if (_dest_type == FTYPE_UNKNOWN)
               {
                  _dest_type = FTYPE_DIR;
                  _dest = ".";
               }
          }
        char *slash = strrchr(_src_filename, '/');
        if (slash) _test_name = strdup(slash + 1);
        else _test_name = strdup(_src_filename);
        char *dot = strrchr(_test_name, '.');
        if (dot) *dot = '\0';
     }

   if (_scan_objects && _dest_type != FTYPE_EXU)
     {
        fprintf(stderr, "Scan objects options is available only if the destination is a EXU file\n");
        goto end;
     }

   if (!show_on_screen) setenv("ELM_ENGINE", "buffer", 1);
   if (!argv[opt_args])
     {
        fprintf(stderr, "no program specified\nUse -h for more information\n");
        goto end;
     }

   if (_dest_type == FTYPE_EXU) _dest_unit = calloc(1, sizeof(*_dest_unit));

   efl_object_init();
   evas_init();

   /* Replace the current command line to hide the Exactness part */
   int len = argv[argc - 1] + strlen(argv[argc - 1]) - argv[opt_args];
   memcpy(argv[0], argv[opt_args], len);
   memset(argv[0] + len, 0, _POSIX_PATH_MAX - len);

   int i;
   for (i = opt_args; i < argc; i++)
     {
        if (i != opt_args)
          {
             argv[i - opt_args] = argv[0] + (argv[i] - argv[opt_args]);
          }
        _printf(1, "%s ", argv[i - opt_args]);
     }
   _printf(1, "\n");

   ecore_evas_callback_new_set(_my_evas_new);
   ecore_idler_add(_src_feed, NULL);
   pret = _prg_invoke(_prg_full_path_guess(argv[0]), argc - opt_args, argv);

   if (_dest && _dest_unit)
     {
        if (_src_unit) _dest_unit->actions = _src_unit->actions;
        exactness_unit_file_write(_dest_unit, _dest);
     }

end:
   eet_shutdown();
   eina_shutdown();
   return pret;
}
