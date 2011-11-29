#ifndef AVL_INCLUDED
#define AVL_INCLUDED

#define AVL_ITER_FORWARD 	0
#define AVL_ITER_BACKWARD 	1

typedef void* generic_ptr;
typedef void** generic_dptr;
typedef void (*void_func_ptr)();
typedef int (*int_func_ptr)();

typedef struct avl_node_struct avl_node;
typedef avl_node* avl_node_ptr;
typedef avl_node** avl_node_dptr;
typedef avl_node*** avl_node_tptr;

struct avl_node_struct {
  avl_node_ptr left;
  avl_node_ptr right;

  generic_ptr key;
  generic_ptr value;

  int height;
};

typedef struct avl_tree_struct avl_tree;
typedef avl_tree* avl_tree_ptr;
struct avl_tree_struct {

  /* the avl root */
  avl_node_ptr root;

  /* comparison function */
  int_func_ptr cmp;

  int num_entries;
  int modified;
};

typedef struct avl_iterator_struct avl_iterator;
typedef avl_iterator* avl_iterator_ptr;
struct avl_iterator_struct {
    avl_tree_ptr tree;
    avl_node_dptr nodelist;
    int count;
};


/* constructors and destructors */
avl_tree_ptr avl_init(int_func_ptr cmp);
void avl_free (avl_tree_ptr, void (*)(), void (*)());

int avl_delete (avl_tree_ptr, generic_dptr, generic_dptr);
int avl_insert (avl_tree_ptr, generic_ptr, generic_ptr);
int avl_lookup (avl_tree_ptr, generic_ptr, generic_dptr);
int avl_first (avl_tree_ptr, generic_dptr, generic_dptr);
int avl_last (avl_tree_ptr, generic_dptr, generic_dptr);
int avl_find_or_add (avl_tree_ptr, generic_ptr, generic_dptr*);
int avl_count (avl_tree_ptr);
int avl_numcmp (generic_ptr, generic_ptr);
void avl_foreach (avl_tree_ptr, void_func_ptr, int);

/* iterators */
avl_iterator_ptr avl_iter (avl_tree_ptr, int);
int avl_iter_next (avl_iterator_ptr, generic_dptr, generic_dptr);
void avl_iter_free (avl_iterator_ptr);


/* shortcuts */
#define avl_count(tree)				                \
  tree->num_entries

#define avl_is_member(tree, key)		                \
  avl_lookup(tree, key, (generic_dptr) NULL)

#define avl_foreach_item(avl, iter, dir, key_p, value_p) 	\
    for(iter = avl_iter(avl, dir); 			\
	    avl_iter_next(iter, key_p, value_p) || (avl_iter_free(iter),0);)

#endif
