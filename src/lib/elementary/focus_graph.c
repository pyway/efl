#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#include <Elementary.h>
#include <Elementary_Cursor.h>

#include "elm_priv.h"
#include "focus_graph.h"

static inline int
_distance(Eina_Rect r1, Eina_Rect r2, Eina_Bool is_x)
{
   int min, max, point;
   int v1, v2;

   if (is_x)
     {
        min = r2.x;
        max = eina_rectangle_max_x(&r2.rect);
        point = r1.x + r1.w/2;
     }
   else
     {
        min = r2.y;
        max = eina_rectangle_max_y(&r2.rect);
        point = r1.y + r1.h/2;
     }

   v1 = abs(min - point);
   v2 = abs(max - point);

   if (v1 < v2)
     return v1;
   else
     return v2;
}

static inline void
_side_free(Side_Definition *def)
{
   def->rel = eina_list_free(def->rel);
   def->distance = INT_MAX;
}

static inline void
_side_add(Side_Definition *def, int distance, Graph_Node *node)
{
   if (!eina_list_data_find(def->rel , node))
     def->rel = eina_list_append(def->rel, node);
   def->distance = distance;
}

static inline void
_side_handle(Side_Definition *def, int distance, Graph_Node *node)
{
   if (distance < def->distance)
     {
        _side_free(def);
        _side_add(def, distance, node);
     }
   if (distance == def->distance)
     {
        _side_add(def, distance, node);
     }
}
#ifdef DEBUG
static void
_debug_list(Eina_List *l)
{
  Eina_List *lst;
  Graph_Node *n;

  EINA_LIST_FOREACH(l, lst, n)
    {
       printf("%p - %s\n", n->obj, efl_class_name_get(n->obj));
    }
}

static void
_debug_result(Calc_Result *result)
{
   for (int i = 0; i < 4; ++i)
     {
        if (result->close[i].rel)
          {
             printf("CLOSE CALL %d\n", i);
             _debug_list(result->close[i].rel);
          }
        else
          {
             printf("RELATIVE CALL %d\n", i);
             _debug_list(result->relative[i].rel);
          }
     }
}
#endif

static inline void
_min_max(Eina_Rect rect, int *min, int *max, Eina_Bool is_x)
{
   if (is_x)
     {
        *min = rect.x;
        *max = eina_rectangle_max_x(&rect.rect);
     }
   else
     {
        *min = rect.y;
        *max = eina_rectangle_max_y(&rect.rect);
     }
}

static inline Eina_Bool
_is_in_sight(Eina_Rect origin, Eina_Rect node, Eina_Bool is_x)
{
   int omin, omax, nmin, nmax;
   _min_max(origin, &omin, &omax, is_x);
   _min_max(node, &nmin, &nmax, is_x);


   /*
    * Following pixelart means:
    * |xxx| is the origin
    * |---| is the node
    */

   return
      /*
       *           |---------|
       *     |xxxxxxxxxx|
       */
      (nmin > omin && nmin < omax) ||
      /*
       * |---------|
       *     |xxxxxxxxxx|
       */
      (nmax > omin && nmax < omax) ||
      /*
       * |------------------------------|
       *     |xxxxxxxxxx|
       */
      (nmin <= omin && nmax >= omax);
}

void
focus_graph_nearest_neighbours_calc(Eina_Iterator *iter, Evas_Object *origin, Calc_Result *result)
{
   Graph_Node *node;
   Eina_Rect origin_size;
   Eina_Rectangle_Outside helper[4] = {
    EINA_RECTANGLE_OUTSIDE_TOP,
    EINA_RECTANGLE_OUTSIDE_LEFT,
    EINA_RECTANGLE_OUTSIDE_RIGHT,
    EINA_RECTANGLE_OUTSIDE_BOTTOM
   };

   origin_size = efl_gfx_geometry_get(origin);
   memset(result, 0, sizeof(Calc_Result));
   for (int i = 0; i < 4; ++i)
     {
        result->relative[i].distance = INT_MAX;
        result->close[i].distance = INT_MAX;
     }

   EINA_ITERATOR_FOREACH(iter, node)
     {
        Eina_Rectangle_Outside outside;
        Eina_Rect node_size;

        if (node->obj == origin) continue;

        node_size = efl_ui_focus_object_focus_geometry_get(node->obj);
        outside = eina_rectangle_outside_position(&origin_size.rect, &node_size.rect);

        for (int i = 0; i < 4; ++i)
          {
             Eina_Bool is_x = (helper[i] == EINA_RECTANGLE_OUTSIDE_RIGHT || helper[i] == EINA_RECTANGLE_OUTSIDE_LEFT);
             int distance = _distance(origin_size, node_size, is_x);
             Side_Definition *close, *relative;
             close = &result->close[i];
             relative = &result->relative[i];

             if (outside & helper[i])
               {
                  if (_is_in_sight(origin_size, node_size, !is_x))
                    {
                       //close call
                       if (relative->rel)
                         _side_free(relative);

                       _side_handle(close, distance, node);
                    }
                  else
                    {
                       //relative position
                       if (!close->rel)
                         _side_handle(relative, distance, node);
                    }
               }
          }
     }
   #ifdef DEBUG
     _debug_result(result);
   #endif
}
