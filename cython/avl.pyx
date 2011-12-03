# file: avl.pyx
cimport avl
from libc.stdlib cimport free

cdef int cmp_callback(a, b):
    return cmp(a, b)

cdef class Avl:
     # wrapped C structure
     cdef avl.avl_tree_ptr _tree

     def __cinit__(self):
         self._tree = avl.avl_init(<cmp_func_ptr> cmp_callback)
         if self._tree is NULL:
            raise MemoryError()

     def __dealloc__(self):
         if self._tree is not NULL:
             avl.avl_deinit(self._tree, 
	     <free_func_ptr> &free, 
	     <free_func_ptr> &free)

     def insert(self, key, value=None):
         assert self._tree is not NULL

         avl.avl_insert(self._tree,
                        <generic_ptr> key, 
                        <generic_ptr> value)

     def __contains__(self, key):
         assert self._tree is not NULL

         if (avl.avl_find(self._tree,
                          <generic_ptr> key, NULL) == 1):
             return True

         return False

     def __getitem__(self, key):
         cdef int res
         cdef generic_ptr value = NULL

         assert self._tree is not NULL

         if (avl.avl_find(self._tree,
                          <generic_ptr> key, 
                          &value) == 0):
             raise ValueError()

         return <object> value



         
     
         
