/** Highly Optimized Python Structures
 *
 * (c) 2011 Marco Pensallorto <marco DOT pensallorto AT gmail DOT com>
 *
 **/
#include "avl.h"

#define HEIGHT(node)                                    \
  ((!node) ? -1 : (node)->height)

#define BALANCE(node)                                   \
  (HEIGHT((node)->right) - HEIGHT((node)->left))

#define STACK_SIZE 0x2000

/* -- static function prototypes -------------------------------------------- */
static inline avl_node_ptr
new_node(generic_ptr key, generic_ptr value);

static inline avl_node_ptr
find_rightmost(avl_node_dptr node_p);

static inline void
do_rebalance(avl_node_tptr stack_nodep, int stack_n);

static inline void
compute_height(avl_node_ptr node);

static inline void
rotate_left(avl_node_dptr node_p);

static inline void
rotate_right(avl_node_dptr node_p);

static inline void
avl_record_iter_forward(avl_node_ptr node, avl_iterator_ptr iter);

static inline void
avl_record_iter_backward(avl_node_ptr node, avl_iterator_ptr iter);

static inline void
avl_walk_forward(avl_node_ptr node, iter_func_ptr func);

static inline void
avl_walk_backward(avl_node_ptr node, iter_func_ptr func);

#ifndef NDEBUG
static inline int
do_check_tree(avl_node_ptr node, cmp_func_ptr compare, int* error);
#endif

static inline void
free_entry(avl_node_ptr node, free_func_ptr key_free, free_func_ptr value_free);

/* -- public functions ------------------------------------------------------ */

/**
   Initialize and return a new avl_tree.  Use the function `compare'
   to compare items in the tree.  `compare' should be of the form:

   int compare(generic_ptr a, generic_ptr b)

   and return a number < 0, == 0, > 0 depending on whether a < b, a ==
   b, or a > b, respectively.
*/
avl_tree_ptr avl_init(cmp_func_ptr cmp)
{
  avl_tree_ptr this = (avl_tree_ptr)(malloc(sizeof(avl_tree)));
  if (this) {
    this->root = NULL;
    this->cmp = cmp;
    this->num_entries = 0;
  }

  return this;
}

/**
   Delete all storage associated with `tree'.  The functions
   key_delete_func and value_delete_func, if non-null, are called to
   free each (key, value) pair.  They are declared as:

   void key_delete_func(generic_ptr key);
   void value_delete_func(generic_ptr value);

   The C-library function free is often suitable as a free function.
*/
void avl_deinit(avl_tree_ptr this,
		free_func_ptr key_free,
		free_func_ptr value_free)
{
  CHECK_INSTANCE(this);

  free_entry(this->root, key_free, value_free);
  free(this);
}


/**
   Clears all items, leaving the tree empty as if it had been just
   created. The functions key_delete_func and value_delete_func, if
   non-null, are called to free each (key, value) pair.  They are
   declared as:

   void key_delete_func(generic_ptr key);
   void value_delete_func(generic_ptr value);

   The C-library function free is often suitable as a free function.
*/
void avl_clear(avl_tree_ptr this,
		free_func_ptr key_free,
		free_func_ptr value_free)
{
  CHECK_INSTANCE(this);

  free_entry(this->root, key_free, value_free);
  this->root = NULL;
  this->num_entries = 0;
}

/**
    Returns number of entries in the tree
 */
int avl_count(avl_tree_ptr this)
{
  CHECK_INSTANCE(this);

  return this->num_entries;
}


/**
   Search for an entry matching `key'.  If found, set `value_p' to the
   associated value field and return 1.  If not found, return 0 and
   leave `value_p' unchanged.
*/
int avl_find(avl_tree_ptr this,
	     generic_ptr key,
	     generic_dptr value_p)
{
  CHECK_INSTANCE(this);

  avl_node_ptr node;
  int diff;

  node = this->root;
  while (node) {

    /* got a match ? */
    if (! (diff = this->cmp(key, node->key))) {
      /* if a non-NULL pointer was given, fetch value */
      if (value_p) (*value_p) = node->value;

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
int avl_first(avl_tree_ptr this,
	      generic_dptr key_p,
	      generic_dptr value_p)
{
  CHECK_INSTANCE(this);

  if (!this->root) return 0;

  else {
    avl_node_ptr node;

    /* walk down the tree; stop at leftmost leaf */
    for(node = this->root; node->left != 0; node = node->left) ;

    if (key_p) (*key_p) = node->key;
    if (value_p) (*value_p) = node->value;

    return 1;
  }
}

/**
   Retrieves the largest element in the tree.  Returns 0 if there are
   no elements in the tree.
*/
int avl_last(avl_tree_ptr this, generic_dptr key_p, generic_dptr value_p)
{
  CHECK_INSTANCE(this);

  if (this->root == 0) return 0;

  else {
    avl_node_ptr node;


    /* walk down the tree; stop at rightmost leaf */
    for(node = this->root; node->right != 0; node = node->right) ;

    if (key_p) (*key_p) = node->key;
    if (value_p) (*value_p) = node->value;

    return 1;
  }
}


/**
   Insert the value `value' under the key `key'.  Multiple items are
   allowed with the same value; all are inserted.
 */
int avl_insert(avl_tree_ptr this, generic_ptr key, generic_ptr value)
{
  CHECK_INSTANCE(this);

  avl_node_dptr node_p;
  avl_node_ptr node;

  avl_node_dptr stack_nodep[STACK_SIZE];
  int stack_n = 0;

  int diff, status;

  node_p = &this->root;

  /* walk down the tree (saving the path); stop at insertion point */
  status = 0;
  while ((node = (*node_p))) {
    stack_nodep[stack_n++] = node_p;
    if (! (diff = this->cmp(key, node->key))) status = 1;

    node_p = (diff < 0) ? &node->left : &node->right;
  }

  /* insert the item and re-balance the tree */
  (*node_p) = new_node(key, value);
  do_rebalance(stack_nodep, stack_n);

  this->num_entries++;
  this->modified = 1;

  return status;
}

/**
   Search for an entry matching key; if not found, insert key and
   return the address of the value slot for this entry.  If found,
   do not insert key, and return the address of the value slot for
   the existing entry.  slot_p can be used to associate a value with
   the key.
*/
int avl_find_or_insert(avl_tree_ptr this, generic_ptr key, generic_tptr slot_p)
{
  CHECK_INSTANCE(this);

  avl_node_dptr node_p;
  avl_node_ptr node;
  int stack_n = 0;

  avl_node_dptr stack_nodep[STACK_SIZE];
  int diff;

  node_p = &this->root;

  /* walk down the tree (saving the path); stop at insertion point */
  while ((node = (*node_p))) {
    stack_nodep[stack_n++] = node_p;

    /* found ? */
    if (! (diff = this->cmp(key, node->key))) {
      if (slot_p) (*slot_p) = &node->value;
      return 1;
    }

    node_p = (diff < 0) ? &node->left : &node->right;
  }

  /* insert the item and re-balance the tree */
  (*node_p) = new_node(key, NULL);

  do_rebalance(stack_nodep, stack_n);

  this->num_entries++;
  this->modified = 1;

  if (slot_p != 0) (*slot_p) = &node->value;

  return 0;                     /* not already in tree */
}


/**
   TODO: review this

   Search for the item with key `*key_p' in `tree'.  If found, set
   `key_p' and `value_p' to point to the key and value of item, delete
   the item and return 1.  Otherwise return 0 and leave `key_p' and
   `value_p' unchanged.
*/
int avl_delete(avl_tree_ptr this, generic_ptr key, generic_dptr value_p)
{
  CHECK_INSTANCE(this);

  avl_node_dptr node_p;
  avl_node_ptr node;
  avl_node_ptr rightmost;
  avl_node_dptr stack_nodep[STACK_SIZE];

  int diff, stack_n = 0;

  node_p = &this->root;

  /* Walk down the tree saving the path; return if not found */
  while ((node = (*node_p))) {
    if (! (diff = this->cmp(key, node->key))) goto delete_item;

    stack_nodep[stack_n++] = node_p;
    node_p = (diff < 0) ? &node->left : &node->right;
  }
  return 0;             /* not found */

  /* prepare to delete node and replace it with rightmost of left tree */
 delete_item:
  if (value_p != 0) (*value_p) = node->value;

  if (!node->left) (*node_p) = node->right;
  else {
    rightmost = find_rightmost(&node->left);
    rightmost->left = node->left;
    rightmost->right = node->right;
    rightmost->height = -2;     /* mark bogus height for do_rebal */

    (*node_p) = rightmost;
    stack_nodep[stack_n++] = node_p;
  }

  free(node);

  /* work our way back up, re-balancing the tree */
  do_rebalance(stack_nodep, stack_n);
  this->num_entries--;
  this->modified = 1;

  return 1;
}

/**
   TODO: review this

   Search for the item with key `*key_p' in `tree'.  If found, set
   `key_p' and `value_p' to point to the key and value of item, delete
   the item and return 1.  Otherwise return 0 and leave `key_p' and
   `value_p' unchanged.
*/
int avl_delete_pair(avl_tree_ptr this, generic_ptr key, generic_ptr value_p)
{
  CHECK_INSTANCE(this);

  avl_node_dptr node_p;
  avl_node_ptr node;
  avl_node_ptr rightmost;
  avl_node_dptr stack_nodep[STACK_SIZE];

  int diff, stack_n = 0;

  node_p = &this->root;

  /* Walk down the tree saving the path; return if not found */
  while ((node = (*node_p))) {
    if (! (diff = this->cmp(key, node->key))) goto delete_item;

    stack_nodep[stack_n++] = node_p;
    node_p = (diff < 0) ? &node->left : &node->right;
  }
  return 0;             /* not found */

  /* prepare to delete node and replace it with rightmost of left tree */
 delete_item:
  if (!node->left) (*node_p) = node->right;
  else {
    rightmost = find_rightmost(&node->left);
    rightmost->left = node->left;
    rightmost->right = node->right;
    rightmost->height = -2;     /* mark bogus height for do_rebal */

    (*node_p) = rightmost;
    stack_nodep[stack_n++] = node_p;
  }

  free(node);

  /* work our way back up, re-balancing the tree */
  do_rebalance(stack_nodep, stack_n);
  this->num_entries--;
  this->modified = 1;

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
  CHECK_INSTANCE(tree);
  avl_iterator_ptr this;

  /* what a hack */
  this = (avl_iterator_ptr)(malloc(sizeof(avl_iterator)));
  this->tree = tree;
  this->nodelist = (avl_node_dptr)(malloc(sizeof(avl_node) * tree->num_entries));
  this->count = 0;

  if (dir == AVL_ITER_FORWARD) {
    avl_record_iter_forward(tree->root, this);
  }
  else if (dir ==  AVL_ITER_BACKWARD) {
    avl_record_iter_backward(tree->root, this);
  }
  else assert(0);

  this->count = 0; /* FIXME: twice? */

  /* catch any attempt to modify the tree while we generate */
  tree->modified = 0;

  return this;
}


int avl_iter_next(avl_iterator_ptr this, generic_dptr key_p, generic_dptr value_p)
{
  CHECK_INSTANCE(this);
  avl_node_ptr node;

  if (this->count == this->tree->num_entries) return 0;

  else {
    node = this->nodelist[this->count++];
    if (key_p) (*key_p) = node->key;
    if (value_p) (*value_p) = node->value;

    return 1;
  }
}


/**
   Free an iterator.
*/
void avl_iter_free(avl_iterator_ptr this)
{
  CHECK_INSTANCE(this);

  free(this->nodelist);
  free(this);
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
void avl_foreach(avl_tree_ptr this, iter_func_ptr func, int direction)
{
  CHECK_INSTANCE(this);

  if (direction == AVL_ITER_FORWARD) {
    avl_walk_forward(this->root, func);
  }

  else if (direction == AVL_ITER_BACKWARD) {
    avl_walk_backward(this->root, func);
  }

  else assert(0);
}


/* -------------------------- internal functions -------------------------- */
static inline avl_node_ptr
find_rightmost(avl_node_dptr node_p)
{
  avl_node_ptr node;
  int stack_n = 0;
  avl_node_dptr stack_nodep[STACK_SIZE];

  node = (*node_p);
  while (node->right) {
    stack_nodep[stack_n++] = node_p;
    node_p = &node->right;
    node = (*node_p);
  }
  (*node_p) = node->left;

  do_rebalance(stack_nodep, stack_n);
  return node;
}


static inline void
do_rebalance(avl_node_tptr stack_nodep, int stack_n)
{
  avl_node_dptr node_p;
  avl_node_ptr node;

  int hl, hr;
  int height;

  /* work our way back up, re-balancing the tree */
  while (--stack_n >= 0) {
    node_p = stack_nodep[stack_n];
    node = (*node_p);

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
  avl_node_ptr old_root = (*node_p);
  avl_node_ptr new_root;
  avl_node_ptr new_right;

  if (BALANCE(old_root->right) >= 0) {
    (*node_p) = new_root = old_root->right;
    old_root->right = new_root->left;
    new_root->left = old_root;
  }

  else {
    new_right = old_root->right;
    (*node_p) = new_root = new_right->left;
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
  avl_node_ptr old_root = (*node_p);
  avl_node_ptr new_root;
  avl_node_ptr new_left;

  if (BALANCE(old_root->left) <= 0) {
    (*node_p) = new_root = old_root->left;
    old_root->left = new_root->right;
    new_root->right = old_root;
  }

  else {
    new_left = old_root->left;
    (*node_p) = new_root = new_left->right;
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
avl_walk_forward(avl_node_ptr node, iter_func_ptr func)
{
  if (node) {
    avl_walk_forward(node->left, func);
    func(node->key, node->value);
    avl_walk_forward(node->right, func);
  }
}


static inline void
avl_walk_backward(avl_node_ptr node, iter_func_ptr func)
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
compute_height(avl_node_ptr node) {
  node->height = 1 + MAX(HEIGHT(node->left),
			 HEIGHT(node->right));
}

static inline void
free_entry(avl_node_ptr node,
           free_func_ptr key_free, free_func_ptr value_free)
{
  if (node) {
    free_entry(node->left, key_free, value_free);
    free_entry(node->right, key_free, value_free);

    if (key_free != 0) (*key_free)(node->key);
    if (value_free != 0) (*value_free)(node->value);
    free(node);
  }
}

/* Allocate a new AVL node */
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

#ifndef NDEBUG
/* Check if the tree is well-formed (this is for debugging purposes
   only) */
int
avl_check_tree(avl_tree_ptr this)
{
  int error = 0;
  (void) do_check_tree(this->root, this->cmp, &error);

  return error;
}

/* Internal service of avl_check_tree */
static inline int
do_check_tree(avl_node_ptr node, cmp_func_ptr compare, int* error)
{
  int l_height, r_height, comp_height, bal;

  if (!node) return -1;

  r_height = do_check_tree(node->right, compare, error);
  l_height = do_check_tree(node->left, compare, error);

  comp_height = MAX(l_height, r_height) + 1;
  bal = r_height - l_height;

  if (comp_height != node->height) {
    printf("Bad height for 0x%p: computed=%d stored=%d\n",
	   (void*) node, comp_height, node->height);
    ++(*error);
  }

  if (bal > 1 || bal < -1) {
    (void) printf("Out of balance at node 0x%p, balance = %d\n",
                  (void*) node, bal);
    ++(*error);
  }

  if ((node->left) &&
      compare(node->left->key, node->key) > 0) {
    (void) printf("Bad ordering between 0x%p and 0x%p",
                  (void*) node, (void*) node->left);
    ++(*error);
  }

  if ((node->right) &&
      compare(node->key, node->right->key) > 0) {
    (void) printf("Bad ordering between 0x%p and 0x%p",
                  (void*) node, (void*) node->right);
    ++(*error);
  }

  return comp_height;
}
#endif
