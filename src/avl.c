/** Highly Optimized Python Structures
 *
 * (c) 2011 Marco Pensallorto <marco DOT pensallorto AT gmail DOT com>
 *
 **/
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "avl.h"

#define MAX(a,b)                                        \
  ((a) > (b) ? (a) : (b))

#define HEIGHT(node)                                    \
  ((!node) ? -1 : (node)->height)

#define BALANCE(node)                                   \
  (HEIGHT((node)->right) - HEIGHT((node)->left))

#define compute_height(node) {                          \
    int x=HEIGHT(node->left), y=HEIGHT(node->right);    \
    (node)->height = MAX(x,y) + 1;                      \
}

#define STACK_SIZE 0x2000

/* static function prototypes*/
static inline avl_node_ptr new_node();
static inline avl_node_ptr find_rightmost();
static inline void do_rebalance(); 
static inline void rotate_left();
static inline void rotate_right();

static inline void 
avl_record_iter_forward(avl_node_ptr node, avl_iterator_ptr iter);

static inline void 
avl_record_iter_backward(avl_node_ptr node, avl_iterator_ptr iter);

static inline void 
avl_walk_forward(avl_node_ptr node, void_func_ptr func);

static inline void 
avl_walk_backward(avl_node_ptr node, void_func_ptr func);

static inline int do_check_tree();
static inline void free_entry(avl_node_ptr node, 
			      void_func_ptr key_free, 
			      void_func_ptr value_free);

/**
   Initialize and return a new avl_tree.  Use the function `compare'
   to compare items in the tree.  `compare' should be of the form:

   int compare(generic_ptr a, generic_ptr b)

   and return a number < 0, == 0, > 0 depending on whether a < b, a ==
   b, or a > b, respectively. 
*/
avl_tree_ptr avl_init(int_func_ptr cmp)
{
  avl_tree_ptr tree = (avl_tree_ptr)(malloc(sizeof(avl_tree)));

  tree->root = NULL;
  tree->cmp = cmp;
  tree->num_entries = 0;

  return tree;
}

/**
   Delete all storage associated with `tree'.  The functions
   key_delete_func and value_delete_func, if non-null, are called to
   free each (key, value) pair.  They are declared as:

   void key_delete_func(generic_ptr key);
   void value_delete_func(generic_ptr value);

   The C-library function free is often suitable as a free function.
*/
void avl_deinit(avl_tree_ptr tree, 
		void_func_ptr key_free,
		void_func_ptr value_free)
{
  free_entry(tree->root, key_free, value_free);
  free(tree);
}

/** 
   Search for an entry matching `key'.  If found, set `value_p' to the
   associated value field and return 1.  If not found, return 0 and
   leave `value_p' unchanged.  
*/
int avl_lookup(avl_tree_ptr tree,
	       generic_ptr key,
	       generic_dptr value_p)
{
   avl_node_ptr node;
   int diff;

  node = tree->root;
  while (node) {
    diff = tree->cmp(key, node->key);
    
    /* got a match ? */
    if (!diff) {
      /* if a non-NULL pointer was given, fetch value */
      if (value_p) { *value_p = node->value; }

      return 1;
    }

    node = (diff < 0) ? node->left : node->right;
  }

  return 0;
}


/**
   Retrieves the smallest element in the tree.  Returns 0 if there are
   no elements in the tree.  
*/
int avl_first(avl_tree_ptr tree, 
	      generic_dptr key_p, 
	      generic_dptr value_p)
{
   avl_node_ptr node;

  if (!tree->root) {
    return 0;           /* no entries */
  } 

  else {
    /* walk down the tree; stop at leftmost leaf */
    for(node = tree->root; node->left != 0; node = node->left) ;

    if (key_p) *key_p = node->key;
    if (value_p) *value_p = node->value;

    return 1;
  }
}

/**
   Retrieves the largest element in the tree.  Returns 0 if there are
   no elements in the tree.  
*/
int avl_last(avl_tree_ptr tree, generic_dptr key_p, generic_dptr value_p)
{
   avl_node_ptr node;

  if (tree->root == 0) {
    return 0;           /* no entries */
  } 

  else {
    /* walk down the tree; stop at rightmost leaf */
    for(node = tree->root; node->right != 0; node = node->right) ;

    if (key_p) *key_p = node->key;
    if (value_p) *value_p = node->value;

    return 1;
  }
}


/**
   Insert the value `value' under the key `key'.  Multiple items are
   allowed with the same value; all are inserted.
 */
int avl_insert(avl_tree_ptr tree, generic_ptr key, generic_ptr value)
{
  avl_node **node_p, *node;
  int stack_n = 0;
  
  avl_node **stack_nodep[STACK_SIZE];
  int diff, status;

  node_p = &tree->root;

  /* walk down the tree (saving the path); stop at insertion point */
  status = 0;
  while ((node = *node_p)) {
    stack_nodep[stack_n++] = node_p;
    diff = tree->cmp(key, node->key);
  
  if (diff == 0) status = 1;
    node_p = (diff < 0) ? &node->left : &node->right;
  }

  /* insert the item and re-balance the tree */
  *node_p = new_node(key, value);
  do_rebalance(stack_nodep, stack_n);
  tree->num_entries++;
  tree->modified = 1;

  return status;
}

/**
   Search for an entry matching key; if not found, insert key and
   return the address of the value slot for this entry.  If found,
   do not insert key, and return the address of the value slot for
   the existing entry.  slot_p can be used to associate a value with
   the key.
*/
int avl_find_or_add(avl_tree_ptr tree, generic_ptr key, generic_tptr slot_p)
{
  avl_node_dptr node_p;
  avl_node_ptr node;
  int stack_n = 0;
  
  avl_node_dptr stack_nodep[STACK_SIZE];
  int diff;

  node_p = &tree->root;

  /* walk down the tree (saving the path); stop at insertion point */
  while ((node = *node_p)) {
    stack_nodep[stack_n++] = node_p;
    diff = tree->cmp(key, node->key);

    /* found ? */
    if (!diff) {
      if (slot_p) { (*slot_p) = &node->value; }
      return 1;           
    }

    node_p = (diff < 0) ? &node->left : &node->right;
  }

  /* insert the item and re-balance the tree */
  *node_p = new_node(key, NULL);

  do_rebalance(stack_nodep, stack_n);

  tree->num_entries++;
  tree->modified = 1;

  if (slot_p != 0) { (*slot_p) = &node->value; }

  return 0;                     /* not already in tree */
}


/**
   Search for the item with key `*key_p' in `tree'.  If found, set
   `key_p' and `value_p' to point to the key and value of item, delete
   the item and return 1.  Otherwise return 0 and leave `key_p' and
   `value_p' unchanged.  

   WARNING: This interface is buggy; in particular, if identical keys
   are in the table, it is not possible to delete a particular (key,
   value) pair.  This will be fixed either with 'handles' or a
   separate delete function.
*/
int avl_delete(avl_tree_ptr tree, generic_dptr key_p, generic_dptr value_p)
{
  avl_node **node_p, *node, *rightmost;
  int stack_n = 0;
  generic_ptr key = *key_p;
  avl_node **stack_nodep[STACK_SIZE];

  int diff;

  node_p = &tree->root;

  /* Walk down the tree saving the path; return if not found */
  while ((node = *node_p)) {
    diff = tree->cmp(key, node->key);
    if (diff == 0) goto delete_item;

    stack_nodep[stack_n++] = node_p;
    node_p = (diff < 0) ? &node->left : &node->right;
  }
  return 0;             /* not found */

  /* prepare to delete node and replace it with rightmost of left tree */
 delete_item:
  *key_p = node->key;
  if (value_p != 0) *value_p = node->value;
  if (!node->left) {
    *node_p = node->right;
  } 

  else {
    rightmost = find_rightmost(&node->left);
    rightmost->left = node->left;
    rightmost->right = node->right;
    rightmost->height = -2;     /* mark bogus height for do_rebal */
    *node_p = rightmost;
    stack_nodep[stack_n++] = node_p;
  }

  free(node);

  /* work our way back up, re-balancing the tree */
  do_rebalance(stack_nodep, stack_n);
  tree->num_entries--;
  tree->modified = 1;

  return 1;
}


/**
   Generate the next item from the avl-tree.  

   Returns 0 if there are no more items in the tree.  Deletion of last
   generated item (via avl_delete) is supported.  Insertion of items
   during generation will result in these items never being generated
   (until the next avl_init_gen()). 
*/
avl_iterator_ptr avl_iter(avl_tree_ptr tree, int dir)
{
  avl_iterator *iter;

  /* what a hack */
  iter = (avl_iterator *)(malloc(sizeof(avl_iterator)));
  iter->tree = tree;
  iter->nodelist = (avl_node_dptr)(malloc(sizeof(avl_node) * avl_count(tree)));
  iter->count = 0;

  if (dir == AVL_ITER_FORWARD) {
    avl_record_iter_forward(tree->root, iter);
  } 
  else if (dir ==  AVL_ITER_BACKWARD) {
    avl_record_iter_backward(tree->root, iter);
  }
  else assert(0);

  iter->count = 0;

  /* catch any attempt to modify the tree while we generate */
  tree->modified = 0;
  return iter;
}


int avl_iter_next(avl_iterator_ptr iter, generic_dptr key_p, generic_dptr value_p)
{
  avl_node_ptr node;

  if (iter->count == iter->tree->num_entries) {
    return 0;
  } 

  else {
    node = iter->nodelist[iter->count++];
    if (key_p) *key_p = node->key;
    if (value_p) *value_p = node->value;

    return 1;
  }
}


/**
   Free a generator.
*/
void avl_iter_free(avl_iterator_ptr iter)
{
  free(iter->nodelist);
  free(iter);
}


/**
   Apply `func' to each item in the tree `tree' in turn.  If direction
   is AVL_FORWARD, the tree is traversed from smallest to
   largest. Otherwise it is traversed from largest to smallest.

   func should be of the form:

   void func(generic_ptr key, generic_ptr value);

   where `key' is the key the item was stored under, and `value' the
   value of the item.
*/
void avl_foreach(avl_tree_ptr tree, void_func_ptr func, int direction)
{
  if (direction == AVL_ITER_FORWARD) {
    avl_walk_forward(tree->root, func);
  } 

  else if (direction == AVL_ITER_BACKWARD) {
    avl_walk_backward(tree->root, func);
  }
  
  else assert(0);
}


/* -------------------------- internal functions -------------------------- */
static inline avl_node_ptr 
find_rightmost(avl_node_dptr node_p)
{
   avl_node_ptr node;
   int stack_n = 0;
  avl_node **stack_nodep[STACK_SIZE];

  node = *node_p;
  while (node->right) {
    stack_nodep[stack_n++] = node_p;
    node_p = &node->right;
    node = *node_p;
  }
  *node_p = node->left;

  do_rebalance(stack_nodep, stack_n);
  return node;
}


static inline void 
do_rebalance(avl_node_tptr stack_nodep, int stack_n)
{
   avl_node **node_p, *node;
   int hl, hr;
  int height;

  /* work our way back up, re-balancing the tree */
  while (--stack_n >= 0) {
    node_p = stack_nodep[stack_n];
    node = *node_p;
    hl = HEIGHT(node->left);            /* watch for NIL */
    hr = HEIGHT(node->right);           /* watch for NIL */
    if ((hr - hl) < -1) {
      rotate_right(node_p);
    } 

    else if ((hr - hl) > 1) {
      rotate_left(node_p);
    } 

    else {
      height = MAX(hl, hr) + 1;
      if (height == node->height) break;
      node->height = height;
    }
  }
}


static inline void
rotate_left(avl_node_dptr node_p)
{
   avl_node *old_root = *node_p, *new_root, *new_right;

  if (BALANCE(old_root->right) >= 0) {
    *node_p = new_root = old_root->right;
    old_root->right = new_root->left;
    new_root->left = old_root;
  } 

  else {
    new_right = old_root->right;
    *node_p = new_root = new_right->left;
    old_root->right = new_root->left;
    new_right->left = new_root->right;
    new_root->right = new_right;
    new_root->left = old_root;
    compute_height(new_right);
  }

  compute_height(old_root);
  compute_height(new_root);
}


static inline void
rotate_right( avl_node_dptr node_p)
{
   avl_node *old_root = *node_p, *new_root, *new_left;

  if (BALANCE(old_root->left) <= 0) {
    *node_p = new_root = old_root->left;
    old_root->left = new_root->right;
    new_root->right = old_root;
  } 

  else {
    new_left = old_root->left;
    *node_p = new_root = new_left->right;
    old_root->left = new_root->right;
    new_left->right = new_root->left;
    new_root->left = new_left;
    new_root->right = old_root;
    compute_height(new_left);
  }

  compute_height(old_root);
  compute_height(new_root);
}


static inline void 
avl_walk_forward(avl_node_ptr node, void_func_ptr func)
{
  if (node) {
    avl_walk_forward(node->left, func);
    func(node->key, node->value);
    avl_walk_forward(node->right, func);
  }
}


static inline void 
avl_walk_backward(avl_node_ptr node, void_func_ptr func)
{
  if (node) {
    avl_walk_backward(node->right, func);
    func(node->key, node->value);
    avl_walk_backward(node->left, func);
  }
}

static inline void 
avl_record_iter_forward(avl_node_ptr node, avl_iterator_ptr iter)
{
  if (node) {
    avl_record_iter_forward(node->left, iter);
    iter->nodelist[iter->count++] = node;
    avl_record_iter_forward(node->right, iter);
  }
}


static inline void 
avl_record_iter_backward(avl_node_ptr node, avl_iterator_ptr iter)
{
  if (node) {
    avl_record_iter_backward(node->right, iter);
    iter->nodelist[iter->count++] = node;
    avl_record_iter_backward(node->left, iter);
  }
}


static inline void 
free_entry(avl_node_ptr node, 
           void_func_ptr key_free, void_func_ptr value_free)
{
  if (node) {
    free_entry(node->left, key_free, value_free);
    free_entry(node->right, key_free, value_free);

    if (key_free != 0) (*key_free)(node->key);
    if (value_free != 0) (*value_free)(node->value);
    free(node);
  }
}
    

static inline avl_node_ptr 
new_node(generic_ptr key, generic_ptr value)
{
  avl_node_ptr new = (avl_node_ptr)(malloc(sizeof(avl_node)));

  new->key = key;
  new->value = value;
  new->height = 0;
  new->left = NULL;
  new->right = NULL;

  return new;
}

/* Check if the tree is well-formed (this is for debugging purposes
   only) */
static int avl_check_tree(avl_tree_ptr tree)
{
  int error = 0;
  (void) do_check_tree(tree->root, tree->cmp, &error);

  return error;
}

/* Internal service of avl_check_tree */
static inline int do_check_tree(avl_node_ptr node, int_func_ptr compare, int* error)
{
  int l_height, r_height, comp_height, bal;
    
  if (!node) {
    return -1;
  }

  r_height = do_check_tree(node->right, compare, error);
  l_height = do_check_tree(node->left, compare, error);

  comp_height = MAX(l_height, r_height) + 1;
  bal = r_height - l_height;
    
  if (comp_height != node->height) {
    (void) printf("Bad height for 0x%p: computed=%d stored=%d\n",
                  (void*) node, comp_height, node->height);
    ++*error;
  }

  if (bal > 1 || bal < -1) {
    (void) printf("Out of balance at node 0x%p, balance = %d\n", 
                  (void*) node, bal);
    ++*error;
  }

  if ((node->left) && 
      compare(node->left->key, node->key) > 0) {
    (void) printf("Bad ordering between 0x%p and 0x%p", 
                  (void*) node, (void*) node->left);
    ++*error;
  }
    
  if ((node->right) && 
      compare(node->key, node->right->key) > 0) {
    (void) printf("Bad ordering between 0x%p and 0x%p", 
                  (void*) node, (void*) node->right);
    ++*error;
  }

  return comp_height;
}
