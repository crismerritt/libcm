#include "cm_set.h"

/*
 * the get_prev_num, get_next_num, del_prev_num and del_next_num operations are
 * hard to implement as macros because they require some temporary variables,
 * as we see below.  i prefer to implement them as functions, rather than play
 * games with temporary variables in macros.
 */


int
cm_set_get_prev_num (cm_Set* set, int* index_ref)
{
  void* index = (void*) (*index_ref + 1);
  int x = (int) (cm_set_get_prev (set, &index)) - 1;
  return (x < 0) ? -1 : (*index_ref = x);
}

int
cm_set_get_next_num (cm_Set* set, int* index_ref)
{
  void* index = (void*) (*index_ref + 1);
  int x = (int) (cm_set_get_next (set, &index)) - 1;
  return (x < 0) ? -1 : (*index_ref = x);
}

int
cm_set_del_prev_num (cm_Set* set, int* index_ref)
{
  void* index = (void*) (*index_ref + 1);
  int x = (int) (cm_set_del_prev (set, &index)) - 1;
  return (x < 0) ? -1 : (*index_ref = x);
}

int
cm_set_del_next_num (cm_Set* set, int* index_ref)
{
  void* index = (void*) (*index_ref + 1);
  int x = (int) (cm_set_del_next (set, &index)) - 1;
  return (x < 0) ? -1 : (*index_ref = x);
}
