#include "checks.h"

static Eina_Bool
_is_implemented(Eolian_Class *class, Eolian_Function *function_id, Eolian_Function_Type *already_provided)
{
   Eolian_Implement *implements;
   Eina_Iterator *iter;

   iter = eolian_class_implements_get(class);

   EINA_ITERATOR_FOREACH(iter, implements)
     {
        Eolian_Function_Type type;
        /* check if the function is the correct one */
        if (eolian_implement_function_get(implements, &type) != function_id) continue;

        /* check if this is the correct type */
        if (type == eolian_function_type_get(function_id)) return EINA_TRUE;

        /*now the crap beginns */
        if (eolian_function_type_get(function_id) == EOLIAN_PROPERTY)
          {
            if ((type == EOLIAN_PROP_SET && *already_provided == EOLIAN_PROP_GET) ||
                (type == EOLIAN_PROP_GET && *already_provided == EOLIAN_PROP_SET))
              {
                 *already_provided = EOLIAN_UNRESOLVED;
                 return EINA_TRUE;
              }
            *already_provided = type;
          }
     }
   return EINA_FALSE;
}

static void
extend_with_inherits(const Eolian_Unit *unit, Eina_List **logic_inherits, Eina_List **api_inherits, const Eolian_Class *c)
{
   Eina_Iterator *iterator;
   Eina_Bool first = EINA_TRUE;
   const char *inherit_name;

   iterator = eolian_class_inherits_get(c);

   EINA_ITERATOR_FOREACH(iterator, inherit_name)
     {
        const Eolian_Class *inherit = eolian_class_get_by_name(unit, inherit_name);

        /* if its the first class we are also inheriting logic from that */
        if (first)
           {
               if (!eina_list_data_find(*logic_inherits, inherit))
                 *logic_inherits = eina_list_append(*logic_inherits, inherit);
           }

        first = EINA_FALSE;

        if (!eina_list_data_find(*api_inherits, inherit))
          {
             *api_inherits = eina_list_append(*api_inherits, inherit);
          }

        /* recusivly expand the inherit set with every extension or inherit */
        extend_with_inherits(unit, logic_inherits, api_inherits, inherit);
     }
}

static Eina_List*
check_unimplemented(const Eolian_Unit *src, const char *classname)
{
   Eina_List *inherit_list = NULL;
   Eina_List *impl_inherit_list = NULL;
   Eina_List *function_list = NULL;
   Eina_List *not_found = NULL;
   Eina_List *node;
   const Eolian_Class *head = eolian_class_get_by_file(src, classname);

   if (eolian_class_type_get(head) != EOLIAN_CLASS_REGULAR) return NULL;

   /* build a list of all inherited classes */
   {
      inherit_list = eina_list_append(inherit_list, head);
      impl_inherit_list = eina_list_append(impl_inherit_list, head);

      extend_with_inherits(src, &impl_inherit_list, &inherit_list, head);
   }

   /* now find every single function available on the object */
   {
      Eolian_Class *c;
      EINA_LIST_FOREACH(inherit_list, node, c)
        {
           Eolian_Function *id;
           Eina_Iterator *itr;

           itr = eolian_class_functions_get(c, EOLIAN_PROP_GET);
           EINA_ITERATOR_FOREACH(itr, id)
             {
                function_list = eina_list_append(function_list, id);
             }

           itr = eolian_class_functions_get(c, EOLIAN_PROP_SET);
           EINA_ITERATOR_FOREACH(itr, id)
             {
                function_list = eina_list_append(function_list, id);
             }

           itr = eolian_class_functions_get(c, EOLIAN_PROPERTY);
           EINA_ITERATOR_FOREACH(itr, id)
             {
                function_list = eina_list_append(function_list, id);
             }

           itr = eolian_class_functions_get(c, EOLIAN_METHOD);
           EINA_ITERATOR_FOREACH(itr, id)
             {
                function_list = eina_list_append(function_list, id);
             }
        }
   }
   /* now check every single function */
   {
      Eolian_Function *function_id;

      EINA_LIST_FOREACH(function_list, node, function_id)
        {
           Eina_Bool found = EINA_FALSE;
           Eina_List *inherit_node;
           Eolian_Class *inherit;
           Eolian_Function_Type partly_type = EOLIAN_UNRESOLVED;

           //printf("%s\n", eolian_function_name_get(function_id));

           /* first by checking the inerhits */
           EINA_LIST_FOREACH(impl_inherit_list, inherit_node, inherit)
             {
                if (_is_implemented(inherit, function_id, &partly_type))
                  {
                     //printf("Found in %s as regular inherit\n", eolian_class_full_name_get(inherit));
                     found = EINA_TRUE;
                     break;
                  }
             }
           if (!found)
             {
                /* then by checking the mixins of the inherit list */
                EINA_LIST_FOREACH(inherit_list, inherit_node, inherit)
                  {
                     if (eolian_class_type_get(inherit) != EOLIAN_CLASS_MIXIN) continue;

                     if (_is_implemented(inherit, function_id, &partly_type))
                       {
                          //printf("Found in %s as mixin inherit\n", eolian_class_full_name_get(inherit));
                          found = EINA_TRUE;
                          break;
                       }
                  }
             }
           if (!found)
             not_found = eina_list_append(not_found, function_id);
        }
   }

   return not_found;
}

void
apply_checks(const Eolian_Unit *src, const char *filename)
{
  Eina_List *functions = check_unimplemented(src, filename);
  Eina_List *node;
  Eolian_Function *function;

  EINA_LIST_FOREACH(functions, node, function)
    {
       const Eolian_Class *c = eolian_function_class_get(function);
       printf("%s.%s not implemented\n", eolian_class_full_name_get(c), eolian_function_name_get(function));
    }
}

#if 0

static const char *
_get_filename(const char *path)
{
   if (!path)
     return NULL;
   const char *ret1 = strrchr(path, '/');
   const char *ret2 = strrchr(path, '\\');
   if (!ret1 && !ret2)
     return path;
   if (ret1 && ret2)
     {
        if (ret1 > ret2)
          return ret1 + 1;
        else
          return ret2 + 1;
     }
   if (ret1)
     return ret1 + 1;
   return ret2 + 1;
}

int main(int argc, char const *argv[])
{
  const char *filename = argv[1];

  eolian_init();

  eolian_system_directory_scan();

  const Eolian_Unit *src = eolian_file_parse(filename);
  if (!src)
    {
       fprintf(stderr, "eolian: could not parse file '%s'\n", filename);
       return -1;
    }

  if (!eolian_database_validate())
    {
       fprintf(stderr, "eolian: failed validating database\n");
       return -1;
    }

  apply_checks(src, _get_filename(filename));

  return 0;
}
#endif
