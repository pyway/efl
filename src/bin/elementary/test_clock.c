#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#include "test.h"
#endif
#include <Elementary.h>

struct _api_data
{
   unsigned int state;  /* What state we are testing       */
   Evas_Object *box;    /* Used in set_api_state           */
};
typedef struct _api_data api_data;

enum _api_state
{
   CLOCK_HIDE_SEC,
   CLOCK_SHOW_AM_PM,
   CLOCK_SHOW_SEC,
   CLOCK_EDIT_MIN,
   CLOCK_EDIT_HOUR,
   CLOCK_EDIT_ALL,
   CLOCK_HIDE_AM_PM,
   API_STATE_LAST
};
typedef enum _api_state api_state;

static const char* api_state_description[] = {
   "Hide Sec",
   "Show AM/PM",
   "Show Sec",
   "Edit Min",
   "Edit Hour",
   "Edit All",
   "Hide AM/PM",
   NULL
};

static void
set_api_state(api_data *api)
{
   const Eina_List *items = elm_box_children_get(api->box);
   Evas_Object *ck = eina_list_nth(items, 0);
   if (!eina_list_count(items))
     return;

   /* use elm_box_children_get() to get list of children */
   switch(api->state)
     { /* Put all api-changes under switch */
      case CLOCK_HIDE_SEC:
        elm_clock_show_seconds_set(ck, EINA_FALSE);
        break;

      case CLOCK_SHOW_AM_PM:
        elm_clock_show_am_pm_set(ck,  EINA_TRUE);
        break;

      case CLOCK_SHOW_SEC:
        elm_clock_show_seconds_set(ck, EINA_TRUE);
        break;

      case CLOCK_EDIT_MIN:
        elm_clock_edit_set(ck, EINA_TRUE);
        elm_clock_edit_mode_set(ck, ELM_CLOCK_EDIT_MIN_DECIMAL | ELM_CLOCK_EDIT_MIN_UNIT);
        break;

      case CLOCK_EDIT_HOUR:
        elm_clock_edit_set(ck, EINA_TRUE);
        elm_clock_edit_mode_set(ck, ELM_CLOCK_EDIT_HOUR_DECIMAL | ELM_CLOCK_EDIT_HOUR_UNIT);
        break;

      case CLOCK_EDIT_ALL:
        elm_clock_edit_set(ck, EINA_TRUE);
        elm_clock_edit_mode_set(ck, ELM_CLOCK_EDIT_ALL);
        break;

      case CLOCK_HIDE_AM_PM:
        elm_clock_show_am_pm_set(ck,  EINA_FALSE);
        break;

      case API_STATE_LAST:

        break;
      default:
        return;
     }
}

static void
_api_bt_clicked(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{  /* Will add here a SWITCH command containing code to modify test-object */
   /* in accordance a->state value. */
   api_data *a = data;
   char str[128];

   printf("clicked event on API Button: api_state=<%s>\n", api_state_description[a->state]);
   set_api_state(a);
   a->state++;
   sprintf(str, "Next API function (%u)", a->state);
   elm_object_text_set(obj, str);
   elm_object_disabled_set(obj, a->state == API_STATE_LAST);
}

static void
_cleanup_cb(void *data, Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   free(data);
}

void
test_clock(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *win, *bx, *ck, *bt, *bxx;
   unsigned int digedit;
   api_data *api = calloc(1, sizeof(api_data));

   win = elm_win_util_standard_add("clock", "Clock");
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_event_callback_add(win, EVAS_CALLBACK_FREE, _cleanup_cb, api);

   bxx = elm_box_add(win);
   evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bxx);
   evas_object_show(bxx);

   bx = elm_box_add(bxx);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   api->box = bx;
   evas_object_show(bx);

   bt = elm_button_add(bxx);
   elm_object_text_set(bt, "Next API function");
   evas_object_smart_callback_add(bt, "clicked", _api_bt_clicked, (void *) api);
   elm_box_pack_end(bxx, bt);
   elm_object_disabled_set(bt, EINA_TRUE);
   evas_object_show(bt);

   elm_box_pack_end(bxx, bx);

   ck = elm_clock_add(bx);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   ck = elm_clock_add(bx);
   elm_clock_show_am_pm_set(ck, EINA_TRUE);
   elm_box_pack_end(bx, ck);
   elm_clock_time_set(ck, 17, 25, 0);
   evas_object_show(ck);

   ck = elm_clock_add(bx);
   elm_clock_show_seconds_set(ck, EINA_TRUE);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   ck = elm_clock_add(bx);
   elm_clock_show_seconds_set(ck, EINA_TRUE);
   elm_clock_show_am_pm_set(ck, EINA_TRUE);
   elm_box_pack_end(bx, ck);
   elm_clock_time_set(ck, 11, 59, 57);
   evas_object_show(ck);

   ck = elm_clock_add(bx);
   elm_clock_show_seconds_set(ck, EINA_TRUE);
   elm_clock_show_am_pm_set(ck, EINA_FALSE);
   elm_clock_time_set(ck, 23, 59, 57);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   ck = elm_clock_add(bx);
   elm_clock_edit_set(ck, EINA_TRUE);
   elm_clock_show_seconds_set(ck, EINA_TRUE);
   elm_clock_show_am_pm_set(ck, EINA_TRUE);
   elm_clock_time_set(ck, 10, 11, 12);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   ck = elm_clock_add(bx);
   elm_clock_show_seconds_set(ck, EINA_TRUE);
   elm_clock_edit_set(ck, EINA_TRUE);
   digedit = ELM_CLOCK_EDIT_HOUR_UNIT | ELM_CLOCK_EDIT_MIN_UNIT | ELM_CLOCK_EDIT_SEC_UNIT;
   elm_clock_edit_mode_set(ck, digedit);
   elm_box_pack_end(bx, ck);
   elm_clock_time_set(ck, 0, 0, 0);
   evas_object_show(ck);

   evas_object_show(win);
}

static void
_edit_bt_clicked(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   Evas_Object *ck = data;

   if (!elm_clock_edit_get(ck))
     {
        elm_object_text_set(obj, "Done");
        elm_clock_edit_set(ck, EINA_TRUE);
        return;
     }
   elm_object_text_set(obj, "Edit");
   elm_clock_edit_set(ck, EINA_FALSE);
}

static void
_hmode_bt_clicked(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   Evas_Object *ck = data;

   if (!elm_clock_show_am_pm_get(ck))
     {
        elm_object_text_set(obj, "24h");
        elm_clock_show_am_pm_set(ck, EINA_TRUE);
        return;
     }
   elm_object_text_set(obj, "12h");
   elm_clock_show_am_pm_set(ck, EINA_FALSE);
}

static Eina_Bool
_pause_cb(void *data)
{
   Evas_Object *ck = data;
   Evas_Object *bt = evas_object_data_get(ck, "_button");
   elm_clock_pause_set(ck, EINA_TRUE);
   elm_object_text_set(bt, "Resume");
   return EINA_FALSE;
}

static void
_pause_resume_bt_clicked(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   Evas_Object *ck = data;

   if (!elm_clock_pause_get(ck))
     {
        elm_object_text_set(obj, "Resume");
        elm_clock_pause_set(ck, EINA_TRUE);
        return;
     }
   elm_object_text_set(obj, "Pause");
   elm_clock_pause_set(ck, EINA_FALSE);
   ecore_timer_add(2, _pause_cb, ck);
}

void
test_clock_edit(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *win, *bx, *hbx, *ck, *bt, *bxx;
   api_data *api = calloc(1, sizeof(api_data));

   win = elm_win_util_standard_add("clock-edit", "Clock Edit");
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_event_callback_add(win, EVAS_CALLBACK_FREE, _cleanup_cb, api);

   bxx = elm_box_add(win);
   evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bxx);
   evas_object_show(bxx);

   bx = elm_box_add(bxx);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   api->box = bx;
   evas_object_show(bx);

   bt = elm_button_add(bxx);
   elm_object_text_set(bt, "Next API function");
   evas_object_smart_callback_add(bt, "clicked", _api_bt_clicked, (void *) api);
   elm_box_pack_end(bxx, bt);
   elm_object_disabled_set(bt, api->state == API_STATE_LAST);
   evas_object_show(bt);

   elm_box_pack_end(bxx, bx);

   ck = elm_clock_add(bx);
   elm_clock_time_set(ck, 0, 15, 3);
   elm_clock_edit_mode_set(ck, ELM_CLOCK_EDIT_DEFAULT);
   elm_clock_show_seconds_set(ck, EINA_TRUE);
   elm_clock_show_am_pm_set(ck, EINA_TRUE);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   hbx = elm_box_add(bx);
   evas_object_size_hint_weight_set(hbx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_horizontal_set(hbx, EINA_TRUE);
   elm_box_pack_end(bx, hbx);
   evas_object_show(hbx);

   bt = elm_button_add(hbx);
   elm_object_text_set(bt, "Edit");
   evas_object_smart_callback_add(bt, "clicked", _edit_bt_clicked, ck);
   elm_box_pack_end(hbx, bt);
   evas_object_show(bt);
   _edit_bt_clicked(ck, bt, NULL);

   bt = elm_button_add(hbx);
   elm_object_text_set(bt, "24h");
   evas_object_smart_callback_add(bt, "clicked", _hmode_bt_clicked, ck);
   elm_box_pack_end(hbx, bt);
   evas_object_show(bt);

   evas_object_show(win);
}

void
test_clock_edit2(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *win, *bx, *ck, *lb;

   win = elm_win_util_standard_add("clock-edit2", "Clock Edit 2");
   elm_win_autodel_set(win, EINA_TRUE);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bx);
   evas_object_show(bx);

   lb = elm_label_add(win);
   elm_object_text_set(lb,
                       "Check time updates for a user mouse button hold."
                       );
   evas_object_size_hint_weight_set(lb, 0.0, 0.0);
   evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);

   ck = elm_clock_add(win);
   elm_clock_show_seconds_set(ck, 1);
   elm_clock_edit_set(ck, EINA_TRUE);
   elm_clock_first_interval_set(ck, 0.4);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   ck = elm_clock_add(win);
   elm_clock_show_seconds_set(ck, 1);
   elm_clock_edit_set(ck, EINA_TRUE);
   elm_clock_first_interval_set(ck, 1.2);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   ck = elm_clock_add(win);
   elm_clock_show_seconds_set(ck, 1);
   elm_clock_edit_set(ck, EINA_TRUE);
   elm_clock_first_interval_set(ck, 2.0);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   evas_object_show(win);
}

void
test_clock_pause(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *win, *bx, *ck, *bt;

   win = elm_win_util_standard_add("clock4", "Clock4");
   elm_win_autodel_set(win, EINA_TRUE);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bx);
   evas_object_show(bx);

   ck = elm_clock_add(win);
   elm_clock_show_seconds_set(ck, EINA_TRUE);
   elm_clock_time_set(ck, 0, 0, 0);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Pause");
   evas_object_smart_callback_add(bt, "clicked", _pause_resume_bt_clicked, ck);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   evas_object_data_set(ck, "_button", bt);
   ecore_timer_add(2, _pause_cb, ck);

   evas_object_show(win);
}
