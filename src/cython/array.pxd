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
    array_ptr array_init(cmp_func_ptr compare)
    array_iterator_ptr array_iter(array_ptr array, 
                              int dir)

    # destructors
    void array_deinit(array_ptr array, 
                      free_func_ptr free_value)
    void array_iter_free(array_iterator_ptr iter_)

    # iterators
    int array_iter_next(array_iterator_ptr iter_, 
                        generic_dptr key_p, 
                        generic_dptr value_p)

    # number of entries
    int array_count(array_ptr array)

    # deletion
    int array_delete (array_ptr array, 
                    generic_ptr key_p, 
                    generic_dptr value_p)
    
    # insertion
    int array_insert (array_ptr array, 
           	      generic_ptr key)

    # find element
    int array_find (array_ptr array, 
                    generic_ptr key, 
                    generic_dptr pvalue)

    # first element
    int array_first (array_ptr array, 
                     generic_dptr pkey, 
                     generic_dptr pvalue)

    # last element
    int array_last (array_ptr array, 
                    generic_dptr pkey, 
                    generic_dptr pvalue)

    

