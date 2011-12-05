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
array_ptr array_init(unsigned size, unsigned number);

/* dctor */
void array_deinit (array_ptr array);

/* getter */
generic_ptr inline array_fetch(array_ptr array, unsigned index)
{ 
  if (index >= array->num) return (generic_ptr)(ARRAY_OUT_OF_BOUNDS);
  return * (array->space + index );
}

/* setter */
int array_insert(array_ptr array, unsigned index, generic_ptr obj);

inline unsigned array_count(const array_ptr array)
{ return array->num; }

inline void
array_insert_last(array_ptr array, generic_ptr obj)
{ array_insert(array, array->num, obj); }

inline generic_ptr array_fetch_first(array_ptr array)      
{ return array_fetch(array, 0); }

inline generic_ptr array_fetch_last(array_ptr array)      
{ return array_fetch(array, ((array)->num)-1); }

int array_resize(array_ptr array, unsigned new_size);

/* iterators */
inline array_iter_ptr array_iter(array_ptr array, int dir)
{
  array_iter_ptr iter;
  
  if (! (iter = (array_iter_ptr)(malloc(sizeof(array_iter)))))
    return NULL;

  iter->array = array;
  iter->next = 0;

  return iter;

}

inline void 
array_iter_deinit(array_iter_ptr iter)
{ free(iter); }

inline int array_iter_next(array_iter_ptr iter, generic_dptr next)
{ 
  if (iter->next == 

return *( iter->array->space + ( iter->next ++ ) ); }


/* aggregate functions */
/* array_ptr array_dup(array_ptr old); */
/* array_ptr array_join (array_ptr array1, array_ptr array2); */
/* int array_append(array_ptr array1, array_ptr array2); */
/* void array_sort (array_ptr array, cmp_func_ptr compare); */
/* void array_uniq (array_ptr array, cmp_func_ptr compare, free_func_ptr free); */


#endif
