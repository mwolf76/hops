#ifndef ARRAY_H
#define ARRAY_H

#include "common.h"

/* Error constants */
#define ARRAY_OK             0
#define ARRAY_OUT_OF_BOUNDS -1
#define ARRAY_OUT_OF_MEM    -2

typedef struct array_t {
  generic_dptr space;
  
  unsigned num;     /* number of array elements.            */
  size_t n_size;    /* size of 'data' array (in objects)    */
} array_t;
typedef array_t* array_ptr;

typedef struct array_iter_t {
  array_ptr array;
  unsigned next;
} array_iter_t;
typedef array_iter_t* array_iter_ptr;

/* ctor */
array_ptr array_init(unsigned size, unsigned number)

/* dctor */
int array_deinit (array_ptr array);

/* getter */
generic_ptr inline array_fetch(array_ptr array, unsigned index)
{ 
  if (index >= array->num) return (generic_ptr)(ARRAY_OUT_OF_BOUNDS);
  return * (generic_ptr) array->space + index * sizeof(generic_ptr) ); 
}

/* setter */
int array_insert(array_ptr array, unsigned index, generic_ptr buf);

inline unsigned array_n(array_ptr array)
{ return array->num; }

inline void
array_insert_last(array_ptr array, generic_ptr buf)
{ array_insert(array, array->num, buf); }

inline generic_ptr array_fetch_last(array_ptr array)      
{ array_fetch(array, ((array)->num)-1); }

int array_resize(array_ptr array, int new_size);

/* iterators */
inline array_iter_ptr array_iter(array_ptr array)
{
  array_iter_ptr iter;
  
  if (! iter = (array_ptr)(malloc(sizeof(array_iter)))) 
    return NULL;

  iter->array = array;
  iter->next = 0;

  return iter;

}

inline void 
array_iter_deinit(array_iter_ptr iter)
{ free(iter); }

inline int 
array_iter_has_next(array_iter_ptr iter)
{ return iter->array->num < iter->next; }

inline generic_ptr array_iter_next(array_iter_ptr iter)
{ return iter->array->space + }


/* aggregate functions */
/* array_ptr array_dup(array_ptr old); */
/* array_ptr array_join (array_ptr array1, array_ptr array2); */
/* int array_append(array_ptr array1, array_ptr array2); */
/* void array_sort (array_ptr array, cmp_func_ptr compare); */
/* void array_uniq (array_ptr array, cmp_func_ptr compare, free_func_ptr free); */


