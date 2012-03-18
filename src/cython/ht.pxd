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
    ctypedef void (*key_free_func_ptr)(generic_ptr key)
    ctypedef void (*value_free_func_ptr)(generic_ptr value)
    ctypedef int (*cmp_func_ptr)(generic_ptr a,
                                 generic_ptr b)
    ctypedef unsigned (*hash_func_ptr)(generic_ptr a,
                                       generic_ptr b)

    # ctors/dctors
    ht_ptr ht_init(hash_func_ptr hash,
                   cmp_func_ptr compare,
                   key_free_func_ptr key_free,
                   key_free_func_ptr value_free)

    void ht_deinit(ht_ptr ht)

    void ht_clear(ht_ptr ht)

    # iterators
    ht_iterator_ptr ht_iter(ht_ptr hash)
    void ht_iter_deinit(ht_iterator_ptr iter_)

    # iterators
    generic_ptr ht_iter_next(ht_iterator_ptr iter_,
                             generic_dptr value_p)

    # number of entries
    size_t ht_count(ht_ptr ht)

    # deletion
    int ht_delete (ht_ptr ht,
                   generic_ptr key_p)

    # insertion
    int ht_insert (ht_ptr hash,
                   generic_ptr key,
                   generic_ptr value)

    # find element
    generic_ptr ht_find (ht_ptr hash,
                         generic_ptr key)
