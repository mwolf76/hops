# file: ht.pyx
cimport ht

cdef extern from "Python.h":
    cdef void Py_INCREF(obj)
    cdef void Py_DECREF(obj)

cdef int cmp_callback(object a, object b):
    return cmp(a, b)

cdef int hash_callback(object a):
    return hash(a)

cdef void free_callback(object obj):
    Py_DECREF(obj)

cdef class HtIterator(object):
    cdef ht.ht_iterator_ptr _iterator

    def __init__(self, Ht obj):
        self._iterator = ht.ht_iter(obj._hash)
        if self._iterator is NULL:
            raise MemoryError()

    def __dealloc__(self):
        assert self._iterator is not NULL
        ht.ht_iter_deinit(self._iterator)

    def __iter__(self):
        return self

    def __next__(self):
        cdef int res
        cdef generic_ptr key = NULL
        cdef generic_ptr value = NULL
        assert self._iterator is not NULL

        key = ht.ht_iter_next(self._iterator, &value)
        if (key == NULL):
            raise StopIteration()

        return (<object> key, <object> value)

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
         self._hash = ht.ht_init(<hash_func_ptr> hash_callback,
                                 <cmp_func_ptr> cmp_callback,
                                 <free_func_ptr> free_callback,
                                 <free_func_ptr> free_callback)
         if self._hash is NULL:
            raise MemoryError()

     def __dealloc__(self):
         """C dctor
         """
         assert self._hash is not NULL
         ht.ht_deinit(self._hash)

     def __contains__(self, object key):
         """__contains__(k) -> True if T has a key k, else False, O(log(n))
         """
         assert self._hash is not NULL

         if (ht.ht_find(self._hash,
                        <generic_ptr> key) != NULL):
             return True

         return False

     def __getitem__(self, object key):
         """__getitem__(y) <==> T[y], T[s:e], O(log(n))
         """
         cdef generic_ptr res = NULL
         assert self._hash is not NULL

         res = ht.ht_find(self._hash,
                          <generic_ptr> key)

         if (res == NULL):
             raise ValueError()

         return <object> res

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

     def __iter__(self):
         """__iter__() <==> iter(T)
         """
         assert self._hash is not NULL
         return HtIterator(self)

     def clear(self):
         """clear() -> None, remove all items from T, O(n)
         """
         assert self._hash is not NULL
         ht.ht_clear(self._hash)

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

         cdef generic_ptr res = NULL
         assert self._hash is not NULL

         res = ht.ht_find(self._hash, <generic_ptr> key)
         if (res == NULL):
             return default

         return <object> res

     def is_empty(self):
         """is_empty() -> True if len(T) == 0, O(1)
         """
         assert self._hash is not NULL
         return ht.ht_count(self._hash) == 0

     def items(self):
         """items([]) -> list (k, v) items of T, O(n)
         """
         res = []

         iter_ = iter(self)
         try:
             while True:
                 res.append(iter_.next())

         except StopIteration:
             pass

         return res

     def keys(self):
         """keys([]) -> list for keys of T, O(n)
         """
         res = []

         iter_ = iter(self)
         try:
             while True:
                 res.append(iter_.next()[0])

         except StopIteration:
             pass

         return res

     def values(self):
         """values([]) -> generator for values of T, O(n)
         """
         res = []

         iter_ = iter(self)
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
                        <generic_ptr> key) == 0):
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
                        <generic_ptr> key) == 0):
             return default

         value_obj = <object> value

         # explicit reference counting decrement
         Py_DECREF(key)
         Py_DECREF(value_obj)

         return value_obj

     def setdefault(self, k, d=None):
         """setdefault(k[,d]) -> T.get(k, d), also set T[k]=d if k not in T, O(log(n))
         """
         pass

     def update(self, E):
         """update(E) -> None. Update T from dict/iterable E, O(E*log(n))
         """
         for (k, v) in E.iteritems():
             self.__setitem__(k, v)
