#ifndef AVL_INCLUDED
#define AVL_INCLUDED

#define AVL_ITER_FORWARD 	0
#define AVL_ITER_BACKWARD 	1


/* -- Type definitions ------------------------------------------------------ */
typedef void* generic_ptr;
typedef void** generic_dptr;
typedef void*** generic_tptr;
typedef void (*free_func_ptr)(generic_ptr data);
typedef void (*iter_func_ptr)(generic_ptr key, generic_ptr value);
typedef int (*cmp_func_ptr)(generic_ptr a, generic_ptr b);

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
  cmp_func_ptr cmp;

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

/* -- Macros ---------------------------------------------------------------- */

/* number of entries */
#define avl_count(tree)				                               \
  tree->num_entries

/* is member? */
#define avl_is_member(tree, key)		                               \
  avl_find(tree, key, (generic_dptr) NULL)


/**
   Generate over all items in an avl-tree.  This macro iterator
   combines avl_init_gen(), avl_gen(), and avl_free_gen() into a
   single statement iterator.x 
*/
#define avl_foreach_item(avl, iter, dir, key_p, value_p) 	              \
  for(iter = avl_iter(avl, dir);					      \
      avl_iter_next(iter, key_p, value_p) || (avl_iter_free(iter),0);)


/* -- Interface ------------------------------------------------------------- */

/* constructor */
avl_tree_ptr avl_init(cmp_func_ptr cmp);

/* destructor */
void avl_deinit(avl_tree_ptr tree, 
		free_func_ptr free_key, 
		free_func_ptr free_value);

/* deletion */
int avl_delete (avl_tree_ptr tree, 
		generic_dptr key_p, 
		generic_dptr value_p);

/* insertion */
int avl_insert (avl_tree_ptr tree, 
		generic_ptr key, 
		generic_ptr value);

/* find element */
int avl_find (avl_tree_ptr tree, 
	      generic_ptr key, 
	      generic_dptr pvalue);

/* smallest element (key) */
int avl_first (avl_tree_ptr tree, 
	       generic_dptr pkey, 
	       generic_dptr pvalue);

/* biggest element (key) */
int avl_last (avl_tree_ptr tree, 
	      generic_dptr pkey, 
	      generic_dptr pvalue);

/* find or insert */
int avl_find_or_insert (avl_tree_ptr tree, 
			generic_ptr key, 
			generic_tptr slot);

/* iterate over all nodes with a function */
void avl_foreach (avl_tree_ptr tree, 
		  iter_func_ptr func, 
		  int dir);

/* iterator constructor */
avl_iterator_ptr avl_iter (avl_tree_ptr tree, 
			   int dir);

/* iterator destructor */
void avl_iter_free (avl_iterator_ptr);

/* next (key, value) in iteration */
int avl_iter_next (avl_iterator_ptr ,
		   generic_dptr,
		   generic_dptr);

#ifndef NDEBUG
/* tree check (debugging) */
int  avl_check_tree(avl_tree_ptr tree);
#endif

#endif

