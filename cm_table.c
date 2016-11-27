#include "cm_table.h"

#define NULL ((void*) 0)

#define NEW(type) ((type *) malloc (sizeof (type)))
#define FREE(ptr) (free (ptr), NULL)

#define BALANCE(n) (!n ? 0 : (int) (n->left_height - n->right_height))
#define MAX(x, y) (x > y ? x : y)
#define HEIGHT(n) (!n ? 0 : MAX (n->left_height, n->right_height) + 1)
#define SIZE(n) (!n ? 0 : n->left_size + n->right_size + 1)

#define CMP(i1, i2) \
  (table->cmp_func ? ((*table->cmp_func) (i1, i2)) \
   : ((char *) i1 - (char *) i2))



static cm_table_Node*
rebalance (cm_table_Node* root)
{ 
  cm_table_Node* temp;

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


static cm_table_Node*
tree_put (cm_table_Node* root, void* index, cm_table_Node* node, 
	  cm_Table* table)
{ 
  int cmp;

  if (!root) 
  { 
    /* put node here */
    node->left = node->right = NULL;
    return rebalance (node);
  }

  cmp = CMP (index, root->index);

  if (cmp == 0)  /* index == root->index */
  { 
    /* node replaces root here */
    node->right = root->right;
    node->left = root->left;
    table->free_node_func ? (*table->free_node_func) (node) : FREE (node);
    return rebalance (node);
  }

  else if (cmp < 0)  /* index < root->index */
  { 
    root->left = tree_put (root->left, index, node, table);
    return rebalance (root);
  }

  else /* index > root->index */
  { 
    root->right = tree_put (root->right, index, node, table);
    return rebalance (root);
  }
}


static cm_table_Node*
tree_get_prev (cm_table_Node* root, void* index, cm_Table* table)
{ 
  if (!root) 
  {
    return NULL;
  }

  else if (CMP (index, root->index) <= 0)  /* index <= root->index */
  {
    return tree_get_prev (root->left, index, table);
  }

  else /* index > root->index */
  { 
    cm_table_Node* prev = tree_get_prev (root->right, index, table);
    return prev ? prev : root; 
  }
}

  
static cm_table_Node*
tree_get_next (cm_table_Node* root, void* index, cm_Table* table)
{ 
  if (!root)
  {
    return NULL;
  }

  else if (CMP (index, root->index) >= 0)  /* index >= root->index */
  {
    return tree_get_next (root->right, index, table);
  }

  else /* index < root->index */
  { 
    cm_table_Node* next = tree_get_next (root->left, index, table);
    return next ? next : root; 
  }
}


static cm_table_Node*
tree_del_min (cm_table_Node* root, cm_table_Node** node_ref)
{ 
  if (!root)
  { 
    return *node_ref = NULL;
  }

  else if (!root->left)
  { 
    *node_ref = root;
    return root->right;
  }

  else 
  { 
    root->left = tree_del_min (root->left, node_ref);
    return rebalance (root);
  }
}


static cm_table_Node*
tree_del_max (cm_table_Node* root, cm_table_Node** node_ref)
{ 
  if (!root)
  { 
    return *node_ref = NULL;
  }

  else if (!root->right)
  {
    *node_ref = root;
    return root->left;
  }

  else
  { 
    root->right = tree_del_max (root->right, node_ref);
    return rebalance (root);
  }
}


static cm_table_Node*
tree_del (cm_table_Node* root, void* index, cm_table_Node** node_ref, 
	  cm_Table* table)
{ 
  int cmp;

  if (!root)
  { 
    return *node_ref = NULL;
  }

  cmp = CMP (index, root->index);

  if (cmp == 0)  /* index == root->index */
  { 
    /* remove this node */

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
      /* delete the node from the far-left of the right-subtree and
       * replace root with it. */
      cm_table_Node* new_root;
      root->right = tree_del_min (root->right, &new_root);
      new_root->left = root->left;
      new_root->right = root->right;
      return rebalance (new_root);
    }
  }

  else if (cmp < 0)  /* index < root->index */
  { 
    root->left = tree_del (root->left, index, node_ref, table);
    return rebalance (root);
  }
    
  else /* index > root->index */
  { 
    root->right = tree_del (root->right, index, node_ref, table);
    return rebalance (root);
  }
}


static cm_table_Node*
tree_del_prev (cm_table_Node* root, void* index, cm_table_Node** node_ref, 
	       cm_Table* table)
{ 
  if (!root)
  { 
    return *node_ref = NULL;
  }

  else if (CMP (index, root->index) <= 0)  /* index <= root->index */
  { 
    root->left = tree_del_prev (root->left, index, node_ref, table);
    return rebalance (root);
  }
    
  else /* index > root->index */
  { 
    root->right = tree_del_prev (root->right, index, node_ref, table);

    if (*node_ref)
    { 
      return rebalance (root);
    }

    else
    { 
      /* remove this node */

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
	/* remove the node from the far-left of the right-subtree and 
	 * replace root with it. */
     	cm_table_Node* new_root;
	root->right = tree_del_min (root->right, &new_root);
	new_root->left = root->left;
	new_root->right = root->right;
        return rebalance (new_root);
      }
    }
  }
}


static cm_table_Node*
tree_del_next (cm_table_Node* root, void* index, cm_table_Node** node_ref, 
	       cm_Table* table)
{ 
  if (!root)
  { 
    return *node_ref = NULL;
  }

  else if (CMP (index, root->index) >= 0)  /* index >= root->index */
  { 
    root->right = tree_del_next (root->right, index, node_ref, table);
    return rebalance (root);
  }

  else /* index < root->index */
  { 
    root->left = tree_del_next (root->left, index, node_ref, table);

    if (*node_ref)
    { 
      return rebalance (root);
    }
    
    else 
    { 
      /* remove this node */

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
	/* remove the node from the far-left of the right-subtree and 
         * replace root with it. */
        cm_table_Node* new_root;
        root->right = tree_del_min (root->right, &new_root);
        new_root->left = root->left;
        new_root->right = root->right;
        return rebalance (new_root);
      }
    }
  }
}


static cm_table_Node*
tree_free (cm_table_Node* root, cm_Table* table)
{ 
  if (!root) return NULL;
  tree_free (root->left, table);
  tree_free (root->right, table); 
  table->free_node_func ? (*table->free_node_func) (root) : FREE (root);
  return NULL;
}


/*
 * public frontends to the recursive functions above.
 */


cm_Table*
cm_table_init (cm_Table* table, int (*cmp_func) (void*, void*))
{ 
  if (!table) return NULL;
  table->root = NULL;
  table->cmp_func = cmp_func;
  table->new_node_func = NULL;
  table->free_node_func = NULL;
  return table; 
}


cm_Table*
cm_table_new (int (*cmp_func) (void*, void*))
{ 
  return cm_table_init (NEW (cm_Table), cmp_func);
}


cm_Table*
cm_table_empty (cm_Table* table)
{
  if (!table) return NULL;
  table->root = tree_free (table->root, table); 
  return table;
}


cm_Table*
cm_table_clean (cm_Table* table)
{ 
  return cm_table_empty (table);
}


cm_Table*
cm_table_free (cm_Table* table)
{ 
  return FREE (cm_table_clean (table));
}


void*
cm_table_put (cm_Table* table, void* index, void* contents)
{ 
  cm_table_Node* node = table->new_node_func ?
    (*table->new_node_func)() : NEW (cm_table_Node);
  if (!node) return NULL;
  node->index = index;
  node->contents = contents;
  table->root = tree_put (table->root, index, node, table);
  return contents; 
}


void*
cm_table_del (cm_Table* table, void* index)
{ 
  cm_table_Node* node;
  void* contents;
  if (!table) return NULL;
  table->root = tree_del (table->root, index, &node, table);
  if (!node) return NULL;
  contents = node->contents;
  table->free_node_func ? (*table->free_node_func) (node) : FREE (node);
  return contents; 
}


void*
cm_table_get_min (cm_Table* table, void** index_ref)
{ 
  cm_table_Node* node = table ? table->root : NULL;
  while (node && node->left) node = node->left;
  if (!node) return NULL;
  if (index_ref) *index_ref = node->index;
  return node->contents; 
}


void*
cm_table_get_max (cm_Table* table, void** index_ref)
{ 
  cm_table_Node* node = table ? table->root : NULL;
  while (node && node->right) node = node->right;
  if (!node) return NULL;
  if (index_ref) *index_ref = node->index;
  return node->contents; 
}


void*
cm_table_get_prev (cm_Table* table, void** index_ref)
{ 
  cm_table_Node* node;
  if (!table) return NULL;
  node = tree_get_prev (table->root, *index_ref, table);
  if (!node) return NULL;
  *index_ref = node->index;
  return node->contents; 
}


void*
cm_table_get_next (cm_Table* table, void** index_ref)
{ 
  cm_table_Node* node;
  if (!table) return NULL;
  node = tree_get_next (table->root, *index_ref, table);
  if (!node) return NULL;
  *index_ref = node->index;
  return node->contents; 
}


void*
cm_table_del_min (cm_Table* table, void** index_ref)
{ 
  cm_table_Node* node;
  void* contents;
  if (!table) return NULL;
  table->root = tree_del_min (table->root, &node);
  if (!node) return NULL;
  contents = node->contents;
  if (index_ref) *index_ref = node->index;
  table->free_node_func ? (*table->free_node_func) (node) : FREE (node);
  return contents; 
}


void*
cm_table_del_max (cm_Table* table, void** index_ref)
{ 
  cm_table_Node* node;
  void* contents;
  table->root = tree_del_max (table->root, &node);
  if (!node) return NULL;
  contents = node->contents;
  if (index_ref) *index_ref = node->index;
  table->free_node_func ? (*table->free_node_func) (node) : FREE (node);
  return contents; 
}


void*
cm_table_del_prev (cm_Table* table, void** index_ref)
{ 
  cm_table_Node* node;
  void* contents;
  if (!table) return NULL;
  table->root = 
    tree_del_prev (table->root, *index_ref, &node, table);
  if (!node) return NULL;
  contents = node->contents;
  *index_ref = node->index;
  table->free_node_func ? (*table->free_node_func) (node) : FREE (node);
  return contents; 
}


void*
cm_table_del_next (cm_Table* table, void** index_ref)
{ 
  cm_table_Node* node;
  void* contents;
  if (!table) return NULL;
  table->root = 
    tree_del_next (table->root, *index_ref, &node, table);
  if (!node) return NULL;
  contents = node->contents;
  *index_ref = node->index;
  table->free_node_func ? (*table->free_node_func) (node) : FREE (node);
  return contents; 
}


/*
 * get is implemented iteratively.
 */

void*
cm_table_get (cm_Table* table, void* index)
{ 
  cm_table_Node* node = table ? table->root : NULL;
  int cmp;

  while (node && (cmp = CMP (index, node->index)) != 0)
    node = (cmp < 0) ? node->left : node->right;

  return node ? node->contents : NULL; 
}


/*
 * cm_table_check - check tree balance and consistency.  just for testing.
 */

static int /* bool - true means well formed tree */
tree_check (cm_table_Node* root, int* height_ref, int* size_ref)
{
  if (!root) 
  {
    *height_ref = *size_ref = 0;
    return 1;
  }

  else
  {
    int left_height, right_height, left_size, right_size, balance;

    if (!tree_check (root->left, &left_height, &left_size)
	|| !tree_check (root->right, &right_height, &right_size))
      return 0;

    balance = left_height - right_height;
    
    if (left_height != HEIGHT (root->left)
	|| left_size != SIZE (root->left)
	|| right_height != HEIGHT (root->right)
	|| right_size != SIZE (root->right)
	|| balance < -1 
	|| balance > 1)
      return 0;
    
    *height_ref = MAX (left_height, right_height) + 1;
    *size_ref = left_size + right_size + 1;
    return 1;
  }
}


int /* bool - true means well formed tree */
cm_table_check (cm_Table* table)
{
  int height, size;
  return !table ? 1 : tree_check (table->root, &height, &size);
}


/*
 * cm_table_print - a nifty debugging tool.
 */

#include <stdio.h>

/* we don't need to pass these parameters around the recursion */
static FILE* file;
static char format[16];

static void
tree_print (cm_table_Node* node, int level)
{ 
  int i;
  if (!node) return;
  tree_print (node->left, level - 1);
  for (i = 0;  i < level;  i++) fputc ('\t', file);
  fprintf (file, format, node->contents);
  tree_print (node->right, level - 1);
}


void
cm_table_print (cm_Table* table, FILE* file, char char_format)
{ 
  file = file;
  sprintf (format, "%%%c\n", char_format);
  tree_print (table->root, HEIGHT (table->root));
}
