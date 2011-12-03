# file: avl.pyx
cimport avl
# from libc.stdlib cimport free

cdef extern from "Python.h":
    cdef void Py_INCREF(obj)
    cdef void Py_DECREF(obj)

cdef int cmp_callback(object a, object b):
    return cmp(a, b)

cdef void free_callback(object obj):
    Py_DECREF(obj)

cdef class AvlForwardIterator(object):
     cdef avl.avl_iterator_ptr _iterator

     def __init__(self, Avl obj):
         self._iterator = avl.avl_iter(obj._tree, 0)  # forward
         if self._iterator is NULL:
            raise MemoryError()

     def __dealloc__(self):
         assert self._iterator is not NULL
         avl.avl_iter_free(self._iterator)

     def __iter__(self):
         return self
     
     def __next__(self):
         cdef int res
         cdef generic_ptr key = NULL
         cdef generic_ptr value = NULL
         assert self._iterator is not NULL

         if (avl.avl_iter_next(self._iterator,
                               &key, &value) == 0):
             raise StopIteration()

         return (<object> key, <object> value)

cdef class AvlBackwardIterator(object):
     cdef avl.avl_iterator_ptr _iterator

     def __init__(self, obj):
         self._iterator = avl.avl_iter(<avl_tree_ptr> obj._tree, 1)  # backward
         if self._iterator is NULL:
            raise MemoryError()

     def __dealloc__(self):
         assert self._iterator is not NULL
         avl.avl_iter_free(self._iterator)

     def __iter__(self):
         return self
     
     def __next__(self):
         cdef int res
         cdef generic_ptr key = NULL
         cdef generic_ptr value = NULL
         assert self._iterator is not NULL

         if (avl.avl_iter_next(self._iterator,
                               &key, &value) == 0):
             raise StopIteration()

         return (<object> key, <object> value)

cdef class Avl(object):
     cdef avl.avl_tree_ptr _tree

     def __cinit__(self):
         self._tree = avl.avl_init(<cmp_func_ptr> cmp_callback)
         if self._tree is NULL:
            raise MemoryError()

     def __dealloc__(self):
         assert self._tree is not NULL
         avl.avl_deinit(self._tree, 
                        <free_func_ptr> free_callback, 
                        <free_func_ptr> free_callback)


     # def __del__(self, object key):
     # """__delitem__(y) <==> del T[y], del[s:e], O(log(n))
     # """
     #     cdef generic_ptr value = NULL
     #     assert self._tree is not NULL
         
     #     if (avl_delete(self._tree,
     #                    &key_p,
     #                    &value_p) == 0):
     #         return None

     #     # explicit reference counting decrement
     #     Py_DECREF(key)
     #     Py_DECREF(value)

     #     return value

     def __contains__(self, object key):
         """__contains__(k) -> True if T has a key k, else False, O(log(n))
         """

         assert self._tree is not NULL

         if (avl.avl_find(self._tree,
                          <generic_ptr> key, NULL) == 1):
             return True

         return False

     def __getitem__(self, object key):
         """__getitem__(y) <==> T[y], T[s:e], O(log(n))
         """
         cdef int res
         cdef generic_ptr value = NULL
         assert self._tree is not NULL

         if (avl.avl_find(self._tree,
                          <generic_ptr> key, 
                          &value) == 0):
             raise ValueError()

         return <object> value

     def __setitem__(self, object key, object value):
         """__setitem__(key, value) <==> T[key] = value, O(log(n))
         """
         assert self._tree is not NULL

         # explicit reference counting increment
         Py_INCREF(key)
         Py_INCREF(value)

         avl.avl_insert(self._tree,
                        <generic_ptr> key, 
                        <generic_ptr> value)

     def insert(self, object key, object value=None):
         
         assert self._tree is not NULL

         # explicit reference counting increment
         Py_INCREF(key)
         Py_INCREF(value)

         avl.avl_insert(self._tree,
                        <generic_ptr> key, 
                        <generic_ptr> value)

     def __len__(self):
         """__len__() <==> len(T), O(1)
         """
         assert self._tree is not NULL
         return avl.avl_count(self._tree)
         
     def __min__(self):
         """__min__() <==> min(T), get min item (k,v) of T, O(log(n))
         """
         cdef generic_ptr key = NULL
         cdef generic_ptr value = NULL
         assert self._tree is not NULL

         if (avl.avl_first(self._tree, &key, &value) == 0):
             raise ValueError()

         return ( <object> key, <object> value )

     def __max__(self):
         """__max__() <==> max(T), get max item (k,v) of T, O(log(n))
         """
         cdef generic_ptr key = NULL
         cdef generic_ptr value = NULL
         assert self._tree is not NULL

         if (avl.avl_last(self._tree, &key, &value) == 0):
             raise ValueError()

         return ( <object> key, <object> value )


     def __and__(self, other):
         """__and__(other) <==> T & other, intersection
         """
         pass

     def __or__(self, other):
         """__or__(other) <==> T | other, union
         """
         pass
     
     def __sub__(self, other):
         """__sub__(other) <==> T - other, difference
         """
         pass

     def __xor__(self, other):
         """__xor__(other) <==> T ^ other, symmetric_difference
         """
         pass

     def __iter__(self):
         """__iter__() <==> iter(T)
         """
         assert self._tree is not NULL
         return AvlForwardIterator(self)

     def __reversed__(self):
         """__reversed__() <==> reversed(T)
         """
         assert self._tree is not NULL
         return AvlBackwardIterator(self)

     def __repr__(self):
         """__repr__() <==> repr(T)
         """
         pass

     def clear(self):
         """clear() -> None, remove all items from T, O(n)
         """
         pass

     def copy(self):
         """copy() -> a shallow copy of T, O(n*log(n))
         """
         pass

     def discard(self, key):
         """discard(k) -> None, remove k from T, if k is present, O(log(n))
         """
         pass

     def get(self, k, d=None):
         """get(k[,d]) -> T[k] if k in T, else d, O(log(n))
         """
         pass

     def is_empty(self):
         """is_empty() -> True if len(T) == 0, O(1)
         """
         pass

     def items(self, reverse=False):
         """items([reverse]) -> generator for (k, v) items of T, O(n)
         """
         if not reverse:
             return AvlForwardIterator(self)
         else:
             return AvlBackwardIterator(self)

     def keys(self, reverse=False):
         """keys([reverse]) -> generator for keys of T, O(n)
         """
         if not reverse:
             return AvlKeysForwardIterator(self)
         else:
             return AvlKeysBackwardIterator(self)

     def values(self, reverse=False):
         """values([reverse]) -> generator for values of T, O(n)
         """
         if not reverse:
             return AvlValuesForwardIterator(self)
         else:
             return AvlValuesBackwardIterator(self)

     def pop(self, k, d=None):
         """pop(k[,d]) -> v, remove specified key and return the corresponding value, O(log(n))
         """
         pass

     def popitem(self, ):
         """popitem() -> (k, v), remove and return some (key, value) pair as a 2-tuple, O(log(n))
         """
         pass

     def setdefault(self,):
         """setdefault(k[,d]) -> T.get(k, d), also set T[k]=d if k not in T, O(log(n))
         """
         pass

     def update(self, E):
         """update(E) -> None. Update T from dict/iterable E, O(E*log(n))
         """
         pass
