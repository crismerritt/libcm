#ifndef CM_LIST_H
#define CM_LIST_H

/*
 * cm_list_Node - the tree is made out of nodes of this sort.  it's worth
 * making nodes as small as possible.  note that a char is enough to hold the 
 * height because of the logarithmic relationship between size and height.
 */

typedef struct cm_list_node {
  void *		contents;
  unsigned char		left_height, right_height;
  unsigned int		left_size, right_size;
  struct cm_list_node 	*left, *right;
} cm_list_Node;
	
typedef struct {
  cm_list_Node	*root;
  cm_list_Node* (*new_node_func) (void);
  cm_list_Node* (*free_node_func) (cm_list_Node*);
} cm_List;


cm_List*	cm_list_init (cm_List* list);
cm_List*	cm_list_new (void);
cm_List*	cm_list_empty (cm_List* list);
cm_List*	cm_list_clean (cm_List* list);
cm_List*	cm_list_free (cm_List* list);
void*		cm_list_put (cm_List* list, unsigned index, void* ptr);
void*		cm_list_get (cm_List* list, unsigned index);
void*		cm_list_del (cm_List* list, unsigned index);


/* evaluate the number of elements in the list */
#define cm_list_size(list) \
  (!list || !list->root ? \
   0 : list->root->left_size + list->root->right_size + 1)


/*
 * macros for storing non-negative ints.  they add 1 to numbers 
 * for storage, subtract 1 upon retrieval, and return -1 on error.   
 * assumes that sizeof (int) == sizeof (void*).
 */
 
#define cm_list_put_num(list, index, num) \
  (((int) cm_list_put (list, index, (void*) ((num) + 1))) - 1)

#define cm_list_get_num(list, index) \
  (((int) cm_list_get (list, index)) - 1)
  
#define cm_list_del_num(list, index) \
  (((int) cm_list_del (list, index)) - 1)

  
/*
 * macros for operations on the head and tail of the list
 */
 
#define cm_list_put_head(list, ptr) \
  cm_list_put (list, 0, ptr)

#define cm_list_put_tail(list, ptr) \
  cm_list_put (list, cm_list_size (list), ptr)

#define cm_list_get_head(list) \
  cm_list_get (list, 0)

#define cm_list_get_tail(list) \
  cm_list_get (list, cm_list_size (list) - 1)

#define cm_list_del_head(list) \
  cm_list_del (list, 0)

#define cm_list_del_tail(list) \
  cm_list_del (list, cm_list_size (list) - 1)

#define cm_list_put_head_num(list, num) \
  cm_list_put_num (list, 0, num)

#define cm_list_put_tail_num(list, num) \
  cm_list_put_num (list, cm_list_size (list), num)

#define cm_list_get_head_num(list) \
  cm_list_get_num (list, 0)

#define cm_list_get_tail_num(list) \
  cm_list_get_num (list, cm_list_size (list) - 1)

#define cm_list_del_head_num(list) \
  cm_list_del_num (list, 0)

#define cm_list_del_tail_num(list) \
  cm_list_del_num (list, cm_list_size (list) - 1)

#endif
