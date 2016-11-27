#ifndef CM_STACK_H
#define CM_STACK_H

/*
 * cm_Stack is just a frontend to the cm_List class, implemented entirely in
 * macros as follows.
 */

#include "cm_list.h"

#define cm_Stack 			cm_List
#define cm_stack_new()  		cm_list_new ()
#define cm_stack_free(obj)		cm_list_free (obj)
#define cm_stack_push(obj, ptr)		cm_list_put (obj, 0, ptr)
#define cm_stack_top(obj)		cm_list_get (obj, 0)
#define cm_stack_pop(obj)		cm_list_del (obj, 0)
#define cm_stack_put(obj, i, ptr)	cm_list_put (obj, i, ptr)
#define cm_stack_get(obj, i)		cm_list_get (obj, i)
#define cm_stack_del(obj, i)		cm_list_del (obj, i)
#define cm_stack_push_num(obj, num)	cm_list_put_num (obj, 0, num)
#define cm_stack_top_num(obj)		cm_stack_get_num (obj, 0)
#define cm_stack_pop_num(obj)		cm_stack_del_num (obj, 0)
#define cm_stack_put_num(obj, i, num)	cm_list_put_num (obj, i, num)
#define cm_stack_get_num(obj, i)	cm_list_get_num (obj, i)
#define cm_stack_del_num(obj, i)	cm_list_del_num (obj, i)
#define cm_stack_size(obj)		cm_list_size (obj)

#endif
