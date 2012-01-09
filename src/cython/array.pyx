# file: array.pyx
cimport array

cdef extern from "Python.h":
    cdef void Py_INCREF(obj)
    cdef void Py_DECREF(obj)

cdef int cmp_callback(object a, object b):
    return cmp(a, b)

cdef void free_callback(object obj):
    Py_DECREF(obj)

cdef class ArrayForwardIterator(object):
     cdef array.array_iterator_ptr _iterator

     def __init__(self, Array obj):
         self._iterator = array.array_iter(obj._array, 1)  # forward
         if self._iterator is NULL:
            raise MemoryError()

     def __dealloc__(self):
         assert self._iterator is not NULL
         array.array_iter_deinit(self._iterator)

     def __iter__(self):
         return self

     def __next__(self):
         cdef int res
         cdef generic_ptr item = NULL

         assert self._iterator is not NULL

         if (array.array_iter_next(self._iterator,
                               &item) == 0):
             raise StopIteration()

         return (<object> item)

cdef class ArrayBackwardIterator(object):
     cdef array.array_iterator_ptr _iterator

     def __init__(self, Array obj):
         self._iterator = array.array_iter(obj._array, -1)  # backward
         if self._iterator is NULL:
            raise MemoryError()

     def __dealloc__(self):
         assert self._iterator is not NULL
         array.array_iter_deinit(self._iterator)

     def __iter__(self):
         return self

     def __next__(self):
         cdef int res
         cdef generic_ptr item = NULL
         assert self._iterator is not NULL

         if (array.array_iter_next(self._iterator,
                               &item) == 0):
             raise StopIteration()

         return (<object> item)

cdef class Array(object):
     cdef array.array_ptr _array

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
         self._array = array.array_init(0, # pick default initial size
                                        <cmp_func_ptr> cmp_callback,
                                        <free_func_ptr> free_callback)

         if self._array is NULL:
            raise MemoryError()

     def __dealloc__(self):
         """C dctor
         """
         assert self._array is not NULL
         array.array_deinit(self._array)


     def __contains__(self, object key):
         """__contains__(k) -> True if T has a key k, else False, O(log(n))
         """

         assert self._array is not NULL

         if (array.array_find(self._array,
                              <generic_ptr> key) == -1):
             return False

         return True

     def __getitem__(self, unsigned ndx):
         """__getitem__(y) <==> T[y], T[s:e], O(log(n))
         """
         cdef int res
         cdef generic_ptr value = NULL
         assert self._array is not NULL

         if (array.array_fetch(self._array, ndx,
                               &value) != 0):
             raise ValueError()

         return <object> value

     def __setitem__(self, unsigned ndx, object value):
         """__setitem__(key, value) <==> T[key] = value, O(log(n))
         """
         assert self._array is not NULL

         # explicit reference counting increment
         Py_INCREF(value)
         array.array_insert(self._array, ndx,
                            <generic_ptr> value)

     def __len__(self):
         """__len__() <==> len(T), O(1)
         """
         assert self._array is not NULL
         return array.array_n(self._array)

     def __min__(self):
         """__min__() <==> min(T), get min item (k,v) of T, O(log(n))
         """
         cdef generic_ptr key = NULL
         cdef generic_ptr value = NULL
         assert self._array is not NULL

         if (array.array_first(self._array, &key, &value) == 0):
             raise ValueError()

         return ( <object> key, <object> value )

     def __max__(self):
         """__max__() <==> max(T), get max item (k,v) of T, O(log(n))
         """
         cdef generic_ptr key = NULL
         cdef generic_ptr value = NULL
         assert self._array is not NULL

         if (array.array_last(self._array, &key, &value) == 0):
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
         assert self._array is not NULL
         return ArrayForwardIterator(self)

     def __reversed__(self):
         """__reversed__() <==> reversed(T)
         """
         assert self._array is not NULL
         return ArrayBackwardIterator(self)

     def append(self, object obj):
         """a.append(object) -- append object to end
         """
         pass

     def count(self, object value):
         """a.count(value) -> integer -- return number of occurrences of value
         """
         pass

     def extend(self, object iterable):
         """a.extend(iterable) -- extend list by appending elements from the iterable
         """
         pass

     def index(self, object obj):
         """a.index(value, [start, [stop]]) -> integer -- return first
         index of value.  Raises ValueError if the value is not
         present.
         """
         pass

     def insert(self, unsigned index, object obj):
         """a.insert(index, object) -- insert object before index
         """
         pass

     def pop(self, index=None):
         """L.pop([index]) -> item -- remove and return item at index
         (default last).  Raises IndexError if list is empty or
         index is out of range.
         """
         pass

     def remove(self, value):
         """L.remove(value) -- remove first occurrence of value.
         Raises ValueError if the value is not present.
         """
         pass

     def reverse(self, ):
         """L.reverse() -- reverse *IN PLACE*
         """
         pass

     def sort(self, cmp=None, reverse=False):
         """L.sort(cmp=None, key=None, reverse=False) -- stable sort
         *IN PLACE*; cmp(x, y) -> -1, 0, 1
         """
         pass

     def is_empty(self):
         """is_empty() -> True if len(T) == 0, O(1)
         """
         assert self._array is not NULL
         return array.array_n(self._array) == 0

     def __delitem__(self, object key):
         """__delitem__(y) <==> del T[y], del[s:e], O(log(n))
         """
         cdef generic_ptr value = NULL
         assert self._array is not NULL

         if (array_delete(self._array,
                        <generic_ptr> key, &value) == 0):
             return

         # explicit reference counting decrement
         Py_DECREF(key)
         Py_DECREF(<object> value)

         return

     # def pop(self, key, default=None):
     #     """pop(k[,d]) -> v, remove specified key and return the corresponding value, O(log(n))
     #     """
     #     cdef generic_ptr value = NULL
     #     assert self._array is not NULL

     #     if (array_delete(self._array,
     #                    <generic_ptr> key, &value) == 0):
     #         return default

     #     value_obj = <object> value

     #     # explicit reference counting decrement
     #     Py_DECREF(key)
     #     Py_DECREF(value_obj)

     #     return value_obj

     # def setdefault(self, k, d=None):
     #     """setdefault(k[,d]) -> T.get(k, d), also set T[k]=d if k not in T, O(log(n))
     #     """
     #     pass

     # def update(self, E):
     #     """update(E) -> None. Update T from dict/iterable E, O(E*log(n))
     #     """
     #     for (k, v) in E.iteritems():
     #         self.__setitem__(k, v)
