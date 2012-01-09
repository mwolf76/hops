# file: ht.pyx
cimport ht

cdef extern from "Python.h":
    cdef void Py_INCREF(obj)
    cdef void Py_DECREF(obj)

cdef int cmp_callback(object a, object b):
    return cmp(a, b)

cdef void free_callback(object obj):
    Py_DECREF(obj)

# cdef class HtForwardIterator(object):
#      cdef ht.ht_iterator_ptr _iterator

#      def __init__(self, Ht obj):
#          self._iterator = ht.ht_iter(obj._tree, 0)  # forward
#          if self._iterator is NULL:
#             raise MemoryError()

#      def __dealloc__(self):
#          assert self._iterator is not NULL
#          ht.ht_iter_free(self._iterator)

#      def __iter__(self):
#          return self

#      def __next__(self):
#          cdef int res
#          cdef generic_ptr key = NULL
#          cdef generic_ptr value = NULL
#          assert self._iterator is not NULL

#          if (ht.ht_iter_next(self._iterator,
#                                &key, &value) == 0):
#              raise StopIteration()

#          return (<object> key, <object> value)

# cdef class HtBackwardIterator(object):
#      cdef ht.ht_iterator_ptr _iterator

#      def __init__(self, obj):
#          self._iterator = ht.ht_iter(<ht_tree_ptr> obj._tree, 1)  # backward
#          if self._iterator is NULL:
#             raise MemoryError()

#      def __dealloc__(self):
#          assert self._iterator is not NULL
#          ht.ht_iter_free(self._iterator)

#      def __iter__(self):
#          return self

#      def __next__(self):
#          cdef int res
#          cdef generic_ptr key = NULL
#          cdef generic_ptr value = NULL
#          assert self._iterator is not NULL

#          if (ht.ht_iter_next(self._iterator,
#                                &key, &value) == 0):
#              raise StopIteration()

#          return (<object> key, <object> value)

cdef class Ht(object):
     cdef ht.ht_ptr _hash

     def __init__(self, seq=None):
         """Python ctor
         """
         if seq is not None:
             try:
                 for (k, v) in seq.__getattribute__('iteritems')():
                     self.__setitem__(k, v)

             except AttributeError:
                 try:
                     for (k, v) in seq.__getattribute__('__iter__')():
                         self.__setitem__(k, v)

                 except AttributeError:
                     raise ValueError("Iterable sequence expected")

     def __cinit__(self):
         """C ctor
         """
         self._hash = ht.ht_init(<cmp_func_ptr> cmp_callback)
         if self._hash is NULL:
            raise MemoryError()

     def __dealloc__(self):
         """C dctor
         """
         assert self._hash is not NULL
         ht.ht_deinit(self._hash,
                        <free_func_ptr> free_callback,
                        <free_func_ptr> free_callback)


     def __contains__(self, object key):
         """__contains__(k) -> True if T has a key k, else False, O(log(n))
         """

         assert self._hash is not NULL

         if (ht.ht_find(self._hash,
                          <generic_ptr> key, NULL) == 1):
             return True

         return False

     def __getitem__(self, object key):
         """__getitem__(y) <==> T[y], T[s:e], O(log(n))
         """
         cdef int res
         cdef generic_ptr value = NULL
         assert self._hash is not NULL

         if (ht.ht_find(self._hash,
                          <generic_ptr> key,
                          &value) == 0):
             raise ValueError()

         return <object> value

     def __setitem__(self, object key, object value):
         """__setitem__(key, value) <==> T[key] = value, O(log(n))
         """
         assert self._hash is not NULL

         # explicit reference counting increment
         Py_INCREF(key)
         Py_INCREF(value)

         ht.ht_insert(self._hash,
                        <generic_ptr> key,
                        <generic_ptr> value)

     def insert(self, object key, object value=None):

         assert self._hash is not NULL

         # explicit reference counting increment
         Py_INCREF(key)
         Py_INCREF(value)

         ht.ht_insert(self._hash,
                        <generic_ptr> key,
                        <generic_ptr> value)

     def __len__(self):
         """__len__() <==> len(T), O(1)
         """
         assert self._hash is not NULL
         return ht.ht_count(self._hash)

     def __min__(self):
         """__min__() <==> min(T), get min item (k,v) of T, O(log(n))
         """
         cdef generic_ptr key = NULL
         cdef generic_ptr value = NULL
         assert self._hash is not NULL

         if (ht.ht_first(self._hash, &key, &value) == 0):
             raise ValueError()

         return ( <object> key, <object> value )

     def __max__(self):
         """__max__() <==> max(T), get max item (k,v) of T, O(log(n))
         """
         cdef generic_ptr key = NULL
         cdef generic_ptr value = NULL
         assert self._hash is not NULL

         if (ht.ht_last(self._hash, &key, &value) == 0):
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
         assert self._hash is not NULL
         return HtForwardIterator(self)

     def __reversed__(self):
         """__reversed__() <==> reversed(T)
         """
         assert self._hash is not NULL
         return HtBackwardIterator(self)

     def clear(self):
         """clear() -> None, remove all items from T, O(n)
         """
         assert self._hash is not NULL
         pass

     def copy(self):
         """copy() -> a shallow copy of T, O(n*log(n))
         """
         assert self._hash is not NULL
         pass

     def discard(self, key):
         """discard(k) -> None, remove k from T, if k is present, O(log(n))
         """
         assert self._hash is not NULL
         pass

     def get(self, key, default=None):
         """get(k[,d]) -> T[k] if k in T, else d, O(log(n))
         """
         assert self._hash is not NULL

         cdef int res
         cdef generic_ptr value = NULL
         assert self._hash is not NULL

         if (ht.ht_find(self._hash,
                          <generic_ptr> key,
                          &value) == 0):
             return default

         return <object> value

     def is_empty(self):
         """is_empty() -> True if len(T) == 0, O(1)
         """
         assert self._hash is not NULL
         return ht.ht_count(self._hash) == 0

     def items(self, reverse=False):
         """items([reverse]) -> list (k, v) items of T, O(n)
         """
         res = []

         if not reverse:
             iter_ = iter(self)
         else:
             iter_ = reversed(self)

         try:
             while True:
                 res.append(iter_.next())

         except StopIteration:
             pass

         return res

     def keys(self, reverse=False):
         """keys([reverse]) -> list for keys of T, O(n)
         """
         res = []

         if not reverse:
             iter_ = iter(self)
         else:
             iter_ = reversed(self)

         try:
             while True:
                 res.append(iter_.next()[0])

         except StopIteration:
             pass

         return res

     def values(self, reverse=False):
         """values([reverse]) -> generator for values of T, O(n)
         """
         res = []

         if not reverse:
             iter_ = iter(self)
         else:
             iter_ = reversed(self)

         try:
             while True:
                 res.append(iter_.next()[1])

         except StopIteration:
             pass

         return res

     def __delitem__(self, object key):
         """__delitem__(y) <==> del T[y], del[s:e], O(log(n))
         """
         cdef generic_ptr value = NULL
         assert self._hash is not NULL

         if (ht_delete(self._hash,
                        <generic_ptr> key, &value) == 0):
             return

         # explicit reference counting decrement
         Py_DECREF(key)
         Py_DECREF(<object> value)

         return

     def pop(self, key, default=None):
         """pop(k[,d]) -> v, remove specified key and return the corresponding value, O(log(n))
         """
         cdef generic_ptr value = NULL
         assert self._hash is not NULL

         if (ht_delete(self._hash,
                        <generic_ptr> key, &value) == 0):
             return default

         value_obj = <object> value

         # explicit reference counting decrement
         Py_DECREF(key)
         Py_DECREF(value_obj)

         return value_obj

     def popitem(self, key, value):
         """popitem() -> (k, v), remove and return some (key, value) pair as a 2-tuple, O(log(n))
         """
         assert self._hash is not NULL

         if (ht_delete_pair(self._hash,
                             <generic_ptr> key,
                             <generic_ptr> value) == 0):
             return None

         # explicit reference counting decrement
         Py_DECREF(key)
         Py_DECREF(value)

         return (key, value)


     def setdefault(self, k, d=None):
         """setdefault(k[,d]) -> T.get(k, d), also set T[k]=d if k not in T, O(log(n))
         """
         pass

     def update(self, E):
         """update(E) -> None. Update T from dict/iterable E, O(E*log(n))
         """
         for (k, v) in E.iteritems():
             self.__setitem__(k, v)
