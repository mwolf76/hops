# file: avl.pxd

cdef extern from "avl/avl.h":

    ctypedef struct avl_tree_struct:
        pass
    ctypedef avl_tree_struct* avl_tree_ptr

    ctypedef struct avl_iterator_struct:
        pass
    ctypedef avl_iterator_struct* avl_iterator_ptr

    # value ptrs
    ctypedef void* generic_ptr
    ctypedef void** generic_dptr
    ctypedef void*** generic_tptr

    # func ptrs
    ctypedef void (*free_func_ptr)(generic_ptr data)
    ctypedef void (*iter_func_ptr)(generic_ptr key,
                                   generic_ptr data)
    ctypedef int (*cmp_func_ptr)(generic_ptr a,
                                 generic_ptr b)

    # constructors
    avl_tree_ptr avl_init(cmp_func_ptr compare)
    avl_iterator_ptr avl_iter(avl_tree_ptr tree,
                              int dir)

    # destructors
    void avl_deinit(avl_tree_ptr avl,
                    free_func_ptr free_key,
                    free_func_ptr free_value)

    void avl_iter_free(avl_iterator_ptr iter_)

    # iterators
    int avl_iter_next(avl_iterator_ptr iter_,
                      generic_dptr key_p,
                      generic_dptr value_p)

    # number of entries
    int avl_count(avl_tree_ptr avl)

    # deletion
    void avl_clear(avl_tree_ptr avl,
                   free_func_ptr free_key,
                   free_func_ptr free_value)

    int avl_delete (avl_tree_ptr avl,
                    generic_ptr key_p,
                    generic_dptr value_p)

    int avl_delete_pair (avl_tree_ptr avl,
                         generic_ptr key_p,
                         generic_ptr value)

    # insertion
    int avl_insert (avl_tree_ptr tree,
                    generic_ptr key,
                    generic_ptr value)

    # find element
    int avl_find (avl_tree_ptr tree,
                  generic_ptr key,
                  generic_dptr pvalue)

    # smallest element (key)
    int avl_first (avl_tree_ptr tree,
                   generic_dptr pkey,
                   generic_dptr pvalue)

    # bigger element (key)
    int avl_last (avl_tree_ptr tree,
                  generic_dptr pkey,
                  generic_dptr pvalue)
