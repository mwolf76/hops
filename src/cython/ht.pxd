# file: ht.pxd

cdef extern from "ht/ht.h":

    ctypedef struct ht_struct:
        pass
    ctypedef ht_struct* ht_ptr

    ctypedef struct ht_iterator_struct:
        pass
    ctypedef ht_iterator_struct* ht_iterator_ptr

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
    ht_ptr ht_init(cmp_func_ptr compare)
    ht_iterator_ptr ht_iter(ht_ptr hash,
                            int dir)

    # destructors
    void ht_deinit(ht_ptr ht,
                  free_func_ptr free_key,
                  free_func_ptr free_value)
    void ht_iter_free(ht_iterator_ptr iter_)

    # iterators
    int ht_iter_next(ht_iterator_ptr iter_,
                    generic_dptr key_p,
                    generic_dptr value_p)

    # number of entries
    int ht_count(ht_ptr ht)

    # deletion
    int ht_delete (ht_ptr ht,
                   generic_ptr key_p,
                   generic_dptr value_p)

    int ht_delete_pair (ht_ptr ht,
                        generic_ptr key_p,
                        generic_ptr value)

    # insertion
    int ht_insert (ht_ptr hash,
                   generic_ptr key,
                   generic_ptr value)

    # find element
    int ht_find (ht_ptr hash,
                 generic_ptr key,
                 generic_dptr pvalue)

    # smallest element (key)
    int ht_first (ht_ptr hash,
                  generic_dptr pkey,
                  generic_dptr pvalue)

    # bigger element (key)
    int ht_last (ht_ptr hash,
                 generic_dptr pkey,
                 generic_dptr pvalue)
