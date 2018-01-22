#ifndef FOCUS_GRAPH_H
#define FOCUS_GRAPH_H 1

#include <Eina.h>

typedef struct {
   Evas_Object *obj;
} Graph_Node;

typedef enum {
   TOP = 0,
   LEFT = 1,
   RIGHT = 2,
   BOTTOM = 3,
} Direction;

typedef struct {
   Eina_List *rel;
   int distance;
} Side_Definition;

typedef struct {
   Side_Definition close[4], relative[4];
} Calc_Result;

void focus_graph_nearest_neighbours_calc(Eina_Iterator *iter,  Evas_Object *origin, Calc_Result *result);

#endif
