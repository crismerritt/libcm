#include "cm_list.h"

#define NULL ((void*) 0)

#define NEW(type) ((type *) malloc (sizeof (type)))
#define FREE(ptr) (free (ptr), NULL)

#define BALANCE(t) (!t ? 0 : (int) (t->left_height - t->right_height))
#define MAX(x, y) (x > y ? x : y)
#define HEIGHT(t) (!t ? 0 : MAX (t->left_height, t->right_height) + 1)
#define SIZE(t) (!t ? 0 : t->left_size + t->right_size + 1)


static cm_list_Node*
rebalance (cm_list_Node* root)
{ 
  cm_list_Node* temp;

  root->left_height = HEIGHT (root->left);
  root->right_height = HEIGHT (root->right);
  root->left_size = SIZE (root->left);
  root->right_size = SIZE (root->right);

  switch (BALANCE (root))
  { 
    case -1:  case 0:  case 1:  
    return root;

    case -2:  /* right heavy - rotate left */
    switch (BALANCE (root->right))
    {
      case -1:  case 0:
      temp = root->right;
      root->right = temp->left;
      temp->left = root;
      temp->left->right_height = temp->left_height;
      temp->left->right_size = temp->left_size;
      temp->left_height = HEIGHT (temp->left);
      temp->left_size = SIZE (temp->left);
      return temp; 

      case 1:
      temp = root->right->left;
      root->right->left = temp->right;
      temp->right = root->right;
      root->right = temp->left;
      temp->left = root;
      temp->left->right_height = temp->left_height;
      temp->right->left_height = temp->right_height;
      temp->left->right_size = temp->left_size;
      temp->right->left_size = temp->right_size;
      temp->left_height = HEIGHT (temp->left);
      temp->right_height = HEIGHT (temp->right);
      temp->left_size = SIZE (temp->left);
      temp->right_size = SIZE (temp->right);
      return temp;
    }

    case 2:  /* left heavy - rotate right */
    switch (BALANCE (root->left))
    {
      case 1:  case 0:
      temp = root->left;
      root->left = temp->right;
      temp->right = root;
      temp->right->left_height = temp->right_height;
      temp->right->left_size = temp->right_size;
      temp->right_height = HEIGHT (temp->right);
      temp->right_size = SIZE (temp->right);
      return temp; 

      case -1:
      temp = root->left->right;
      root->left->right = temp->left;
      temp->left = root->left;
      root->left = temp->right;
      temp->right = root;
      temp->right->left_height = temp->right_height;
      temp->left->right_height = temp->left_height;
      temp->right->left_size = temp->right_size;
      temp->left->right_size = temp->left_size;
      temp->right_height = HEIGHT (temp->right);
      temp->left_height = HEIGHT (temp->left);
      temp->right_size = SIZE (temp->right);
      temp->left_size = SIZE (temp->left);
      return temp;
    }
  }
}


static cm_list_Node*
tree_put (cm_list_Node* root, unsigned index, cm_list_Node* node)
{ 
  if (!root)
  { 
    /* put node here */
    node->left = node->right = NULL;
    return rebalance (node);
  }

  else if (root->left_size == index)
  { 
    /* root goes to far left of right subtree.  new node goes here */
    node->left = root->left;
    node->right = tree_put (root->right, 0, root);
    return rebalance (node);
  }

  else if (root->left_size > index)
  { 
    root->left = tree_put (root->left, index, node);
    return rebalance (root); 
  }

  else /* root->left_size < index */
  { 
    root->right = tree_put (root->right, index - root->left_size - 1, node);
    return rebalance (root); 
  }
}


static cm_list_Node*
tree_del (cm_list_Node* root, unsigned index, cm_list_Node** node_ref)
{ 
  if (!root)
  { 
    return *node_ref = NULL;
  }

  else if (root->left_size == index)
  { 
    /* remove root */

    *node_ref = root;
    
    if (!root->left && !root->right)
    { 
      return NULL; 
    }

    else if (!root->left)
    { 
      return root->right;
    }
      
    else if (!root->right)
    { 
      return root->left;
    }

    else /* root->left && root->right */
    { 
      /* delete the node from the far-left of the right subtree
       * and replace root with it. */
      cm_list_Node* new_root;
      root->right = tree_del (root->right, 0, &new_root);
      new_root->left = root->left;
      new_root->right = root->right;
      return rebalance (new_root);
    }
  }

  else if (root->left_size > index)
  { 
    root->left = tree_del (root->left, index, node_ref);
    return rebalance (root);
  }

  else /* root->left_size < index */
  { 
    root->right = tree_del (root->right, index - root->left_size - 1, node_ref);
    return rebalance (root);
  }
}


static cm_list_Node*
tree_free (cm_list_Node* root, cm_List* list)
{ 
  if (!root) return NULL;
  tree_free (root->right, list);
  tree_free (root->left, list); 
  list->free_node_func ? (*list->free_node_func) (root) : FREE (root);
  return NULL;
}


/*
 * public frontends to the recursive functions above.
 */


cm_List*
cm_list_init (cm_List* list)
{ 
  if (!list) return NULL;
  list->root = NULL;
  list->new_node_func = NULL;
  list->free_node_func = NULL;
  return list; 
}


cm_List*
cm_list_new (void)
{ 
  return cm_list_init (NEW (cm_List)); 
}


cm_List*
cm_list_empty (cm_List* list)
{ 
  if (!list) return NULL;
  list->root = tree_free (list->root, list);
  return list;
}


cm_List*
cm_list_clean (cm_List* list)
{ 
  return cm_list_empty (list);
}


cm_List*
cm_list_free (cm_List* list)
{ 
  return FREE (cm_list_clean (list));
}


void*
cm_list_put (cm_List* list, unsigned index, void* contents)
{ 
  cm_list_Node* node = list->new_node_func ?
    (*list->new_node_func)() : NEW (cm_list_Node);
  if (!node) return NULL;
  node->contents = contents;
  list->root = tree_put (list->root, index, node);
  return contents; 
}


void*
cm_list_del (cm_List* list, unsigned index)
{ 
  cm_list_Node* node;
  void* contents;
  if (!list) return NULL;
  list->root = tree_del (list->root, index, &node);
  if (!node) return NULL;
  contents = node->contents;
  list->free_node_func ? (*list->free_node_func) (node) : FREE (node);
  return contents; 
}


/*
 * get is implemented non-recursively
 */

void*
cm_list_get (cm_List* list, unsigned index)
{ 
  cm_list_Node* node = list ? list->root : NULL;

  while (node && node->left_size != index)
  {
    if (node->left_size > index)
    {
      node = node->left;
    }

    else
    {
      index -= node->left_size + 1;
      node = node->right;
    }
  }

  return node ? node->contents : NULL; 
}


/*
 * cm_list_check - check tree balance and consistency.  just for testing.
 */

static int /* bool - true means well formed tree */
tree_check (cm_list_Node* root, int* height_ref, int* size_ref)
{
  if (!root) 
  {
    *height_ref = *size_ref = 0;
    return 1;
  }

  else
  {
    int left_height, right_height, left_size, right_size, balance;

    if (!tree_check (root->left, &left_height, &left_size) ||
	!tree_check (root->right, &right_height, &right_size))
      return 0;

    balance = left_height - right_height;
    
    if (left_height != HEIGHT (root->left) ||
	left_size != SIZE (root->left) ||
	right_height != HEIGHT (root->right) ||
	right_size != SIZE (root->right) ||
	balance < -1 || balance > 1)
      return 0;
    
    *height_ref = MAX (left_height, right_height) + 1;
    *size_ref = left_size + right_size + 1;
    return 1;
  }
}


int /* bool - true means well formed tree */
cm_list_check (cm_List* list)
{
  int height, size;
  return !list ? 1 : tree_check (list->root, &height, &size);
}


/*
 * cm_list_print - a nifty debugging tool.
 */

#include <stdio.h>

/* we don't need to pass these parameters around the recursion */
static FILE* file;
static char format[16];

static void
tree_print (cm_list_Node* node, int level)
{ 
  int i;
  if (!node) return;
  tree_print (node->left, level - 1);
  for (i = 0;  i < level;  i++) fputc ('\t', file);
  fprintf (file, format, node->contents);
  tree_print (node->right, level - 1);
}


void
cm_list_print (cm_List* list, FILE* file, char char_format)
{ 
  file = file;
  sprintf (format, "%%%c\n", char_format);
  tree_print (list->root, HEIGHT (list->root));
}
