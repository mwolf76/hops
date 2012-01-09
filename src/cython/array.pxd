# file: array.pxd

cdef extern from "array/array.h":

    ctypedef struct array_struct:
        pass
    ctypedef array_struct* array_ptr

    ctypedef struct array_iterator_struct:
        pass
    ctypedef array_iterator_struct* array_iterator_ptr

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
    array_ptr array_init(unsigned size,
                         cmp_func_ptr compare,
                         free_func_ptr free)

    array_iterator_ptr array_iter(array_ptr array,
                              int dir)

    # destructors
    void array_deinit(array_ptr array)
    void array_iter_deinit(array_iterator_ptr iter_)

    # iterators
    int array_iter_next(array_iterator_ptr iter_,
                        generic_dptr value_p)


    # number of entries
    int array_n(array_ptr array)

    # number of occurrences
    int array_count(array_ptr array, generic_ptr key)

    # getter
    int array_fetch (array_ptr array,
                     unsigned index,
                     generic_dptr out)

    # insertion
    int array_insert (array_ptr array,
                      unsigned ndx,
           	      generic_ptr key)

    # deletion
    int array_delete(array_ptr array,
                     unsigned index,
                     generic_dptr item_p)

    # find element
    int array_find (array_ptr array,
                    generic_ptr key)

    # first element
    int array_first (array_ptr array,
                     generic_dptr pkey,
                     generic_dptr pvalue)

    # last element
    int array_last (array_ptr array,
                    generic_dptr pkey,
                    generic_dptr pvalue)
