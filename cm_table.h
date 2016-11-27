#ifndef CM_TABLE_H
#define CM_TABLE_H

/*
 * cm_table_Node - the tree is made out of nodes of this sort.  it's worth
 * making nodes as small as possible.  note that a char is enough to hold the 
 * height because of the logarithmic relationship between size and height. 
 */

typedef struct cm_table_node {
  void			*index, *contents;
  unsigned char		left_height, right_height;
  unsigned int		left_size, right_size;
  struct cm_table_node 	*left, *right;
} cm_table_Node;

typedef struct {
  cm_table_Node		*root;
  int 			(*cmp_func) (void*, void*);
  cm_table_Node* 	(*new_node_func) (void);
  cm_table_Node*	(*free_node_func) (cm_table_Node*);
} cm_Table;


cm_Table* cm_table_init (cm_Table* table, int (*cmp_func) (void*, void*));
cm_Table* cm_table_new (int (*cmp_func) (void*, void*));
cm_Table* cm_table_empty (cm_Table* table);
cm_Table* cm_table_clean (cm_Table* table);
cm_Table* cm_table_free (cm_Table* table);

void*     cm_table_put (cm_Table* table, void* index, void* ptr);
void*     cm_table_get (cm_Table* table, void* index);
void*     cm_table_del (cm_Table* table, void* index);

void*     cm_table_get_min (cm_Table* table, void** index_ref);
void*     cm_table_get_max (cm_Table* table, void** index_ref);
void*     cm_table_del_min (cm_Table* table, void** index_ref);
void*     cm_table_del_max (cm_Table* table, void** index_ref);

void*     cm_table_get_prev (cm_Table* table, void** index_ref);
void*     cm_table_get_next (cm_Table* table, void** index_ref);
void*     cm_table_del_prev (cm_Table* table, void** index_ref);
void*     cm_table_del_next (cm_Table* table, void** index_ref);


/* evaluate the number of elements in the table */
#define cm_table_size(table) \
  (!table || !table->root ? 0 \
   : table->root->left_size + table->root->right_size + 1)


/*
 * macros for storing non-negative ints.  upon storage, they add 1 to numbers
 * then cast to void*.  upon retrieval, they cast back to int and subract 1.
 * this enables us to store the value 0 and implies that these macros return -1
 * on error where the normal functions return NULL. 
 */
 
#define NUM_IN(i)	(void *) ((i) + 1)
#define NUM_OUT(p)	(((int) p) - 1)

#define cm_table_put_num(table, index, num) \
  NUM_OUT (cm_table_put (table, index, NUM_IN (num)))

#define cm_table_get_num(table, index) \
  NUM_OUT (cm_table_get (table, index))

#define cm_table_del_num(table, index) \
  NUM_OUT (cm_table_del (table, index))

#define cm_table_get_min_num(table, index_ref) \
  NUM_OUT (cm_table_get_min (table, index_ref))

#define cm_table_del_min_num(table, index_ref) \
  NUM_OUT (cm_table_del_min (table, index_ref))

#define cm_table_get_max_num(table, index_ref) \
  NUM_OUT (cm_table_get_max (table, index_ref))

#define cm_table_del_max_num(table, index_ref) \
  NUM_OUT (cm_table_del_max (table, index_ref))

#define cm_table_get_prev_num(table, index_ref) \
  NUM_OUT (cm_table_get_prev (table, index_ref))

#define cm_table_del_prev_num(table, index_ref) \
  NUM_OUT (cm_table_del_prev (table, index_ref)) 

#define cm_table_get_next_num(table, index_ref) \
  NUM_OUT (cm_table_get_next (table, index_ref))

#define cm_table_del_next_num(table, index_ref) \
  NUM_OUT (cm_table_del_next (table, index_ref))

#endif

