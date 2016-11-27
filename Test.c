#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cm_list.h"
#include "cm_table.h"
#include "cm_heap.h"


#define TEST_SIZE 1000


static int
R (int max)			/* R - return random x | 0 <= x < max */
{
  extern double drand48();
  return (int) (drand48() * max);
}


static int
CMP (char* s1, char* s2)
{
  int i1 = atoi (s1);
  int i2 = atoi (s2);
  return i1 - i2;
}

static void 
fail (void)
{
  printf ("test failed\n");
  exit (1);
}


static void
memory_error (void)
{
  printf ("memory error\n");
  exit (1);
}


char heap_mem [1024 * 1024];
cm_Heap* heap;

static inline cm_list_Node*
list_node_new (void)
{
  return (cm_list_Node*) cm_heap_alloc (heap, sizeof (cm_list_Node));
}

static inline cm_list_Node*
list_node_free (cm_list_Node* node)
{
  cm_heap_free (heap, node);
  return NULL;
}

static inline cm_table_Node*
table_node_new (void)
{
  return (cm_table_Node*) cm_heap_alloc (heap, sizeof (cm_table_Node));
}

static inline cm_table_Node*
table_node_free (cm_table_Node* node)
{
  cm_heap_free (heap, node);
  return NULL;
}


void
main (int argc, char* argv[])
{
  cm_Table	*table1 = cm_table_new (CMP);
  cm_Table	*table2 = cm_table_new (CMP);
  cm_List	*list = cm_list_new ();

  char	*s, *b, buf[16];
  int 	i;

  if (!table1 || !table2 || !list)
    memory_error();

  heap = cm_heap_init (heap_mem, sizeof heap_mem);
  if (!heap) fail();

  table1->new_node_func = table_node_new;
  table1->free_node_func = table_node_free;
  table2->new_node_func = table_node_new;
  table2->free_node_func = table_node_free;
  list->new_node_func = list_node_new;
  list->free_node_func = list_node_free;

  /* init table1 */
  for(i = 0;  i < TEST_SIZE;  i++)
  {
    sprintf(buf, "%d", i);
    s = strdup (buf);
    if (!cm_table_put (table1, s, s))
      memory_error();
  }
  
  while (cm_table_size (table2) < TEST_SIZE)
  {
    switch (R (12))  /* choose a case randomly */
    { 
      case 0:  /* move from table1 to list with 'del_prev || 'del_next */
      if (!cm_table_size (table1)) break;
      sprintf (b = buf, "%d", R (TEST_SIZE));
      (s = cm_table_del_prev (table1, &b)) 
	|| (s = cm_table_del_next (table1, &b));
      if (!s) fail();
      if (!cm_list_put (list, R (cm_list_size (list)), s))
	memory_error();
      break;
      
      case 1:  /* move from table1 to list with 'del_next || 'del_prev */
      if (!cm_table_size (table1)) break;
      sprintf (b = buf, "%d", R (TEST_SIZE));
      (s = cm_table_del_next (table1, &b)) 
	|| (s = cm_table_del_prev (table1, &b));
      if (!s) fail();
      if (!cm_list_put (list, R (cm_list_size (list)), s))
	memory_error();
      break;
      
      case 2:  /* move from table1 to list with 'del_min */
      if (!cm_table_size (table1)) break;
      sprintf (b = buf, "%d", R (TEST_SIZE));
      s = cm_table_del_min (table1, &b);
      if (!s) fail();
      if (!cm_list_put (list, R (cm_list_size (list)), s))
	memory_error();
      break;
      
      case 3:  /* move from table1 to list with 'del_max */
      if (!cm_table_size (table1)) break;
      sprintf (b = buf, "%d", R (TEST_SIZE));
      s = cm_table_del_max (table1, &b);
      if (!s) fail();
      if (!cm_list_put (list, R (cm_list_size (list)), s))
	memory_error();
      break;
      
      case 4:  /* excercise 'get_prev || 'get_next */
      if (!cm_table_size (table1)) break;
      sprintf (b = buf, "%d", R (TEST_SIZE));
      (s = cm_table_get_prev (table1, &b)) 
	|| (s = cm_table_get_next (table1, &b));
      s = cm_table_del (table1, s);  
      if (!s) fail();
      if (!cm_list_put (list, R (cm_list_size (list)), s))
	memory_error();
      break;
      
      case 5:  /* excercise 'get_next || 'get_prev */
      if (!cm_table_size (table1)) break;
      sprintf (b = buf, "%d", R(TEST_SIZE));
      (s = cm_table_get_next(table1, &b)) 
	|| (s = cm_table_get_prev (table1, &b));
      s = cm_table_del (table1, s);  
      if (!s) fail();
      if (!cm_list_put (list, R (cm_list_size (list)), s))
	memory_error();
      break;
      
      /* move from list to table2 */
      case 6:  case 7:  case 8:  case 9:  case 10:  case 11:
      if (!cm_list_size (list)) break;
      s = cm_list_del (list, R (cm_list_size (list)));
      if (!s) fail();
      if (!cm_table_put (table2, s, s))
	memory_error();
      break;
    }

    /* check all structures after each operation */
    if (!cm_table_check (table1) 
	|| !cm_table_check (table2)
	|| !cm_list_check (list))
      fail();
  }

  if (cm_list_size (list) || cm_table_size (table1))
    fail();

  fprintf (stderr, "%s: test passed\n", argv[0]);
  exit (0);
}
