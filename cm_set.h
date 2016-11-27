#ifndef CM_SET_H
#define CM_SET_H

/* 
 * cm_Set is just an frontend to cm_Table which duplicates the index in the
 * contents. 
 */

#include "cm_table.h"

#define cm_Set				cm_Table
#define cm_set_new(cmp_func)		cm_table_new (cmp_func)
#define cm_set_free(obj)		cm_table_free (obj)
#define cm_set_put(obj, index)		cm_table_put (obj, index, index)
#define cm_set_get(obj, index)		cm_table_get (obj, index)
#define cm_set_del(obj, index)		cm_table_del (obj, index)
#define cm_set_get_min(obj)		cm_table_get_min (obj, NULL)
#define cm_set_get_max(obj)		cm_table_get_max (obj, NULL)
#define cm_set_del_min(obj)		cm_table_del_min (obj, NULL)
#define cm_set_del_max(obj)		cm_table_del_max (obj, NULL)
#define cm_set_get_prev(obj, index_ref)	cm_table_get_prev (obj, index_ref)
#define cm_set_get_next(obj, index_ref)	cm_table_get_next (obj, index_ref)
#define cm_set_del_prev(obj, index_ref)	cm_table_del_prev (obj, index_ref)
#define cm_set_del_next(obj, index_ref)	cm_table_del_next (obj, index_ref)
#define cm_set_size(obj)		cm_table_size (obj)

#define cm_set_put_num(obj, i)		NUM_OUT (cm_set_put (obj, NUM_IN (i)))
#define cm_set_get_num(obj, i)		NUM_OUT (cm_set_get (obj, NUM_IN (i)))
#define cm_set_del_num(obj, i)		NUM_OUT (cm_set_del (obj, NUM_IN (i)))
#define cm_set_get_min_num(obj)		NUM_OUT (cm_set_get_min (obj))
#define cm_set_get_max_num(obj)		NUM_OUT (cm_set_get_max (obj))
#define cm_set_del_min_num(obj)		NUM_OUT (cm_set_del_min (obj))
#define cm_set_del_max_num(obj)		NUM_OUT (cm_set_del_max (obj))

/* see cm_set.c */
extern int cm_set_get_prev_num (cm_Set* set, int* index_ref);
extern int cm_set_get_next_num (cm_Set* set, int* index_ref);
extern int cm_set_del_prev_num (cm_Set* set, int* index_ref);
extern int cm_set_del_next_num (cm_Set* set, int* index_ref);

#endif

