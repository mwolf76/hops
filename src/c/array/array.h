#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY_ITER_FORWARD 	 1
#define ARRAY_ITER_BACKWARD 	-1

#include "common.h"

#define ARRAY_INIT_SIZE 1024

/* Error constants */
#define ARRAY_OK             0
#define ARRAY_OUT_OF_BOUNDS -1
#define ARRAY_OUT_OF_MEM    -2

typedef struct array_t {
  generic_dptr space;
  cmp_func_ptr cmp;
  free_func_ptr free;

  unsigned num;     /* number of array elements.            */
  size_t n_size;    /* size of 'data' array (in objects)    */
} array_t;
typedef array_t* array_ptr;

typedef struct array_iterator_t {

  generic_dptr next;
  generic_dptr last;

  int dir;
} array_iterator_t;
typedef array_iterator_t* array_iterator_ptr;

/* ctor */
array_ptr array_init(unsigned size, cmp_func_ptr cmp, free_func_ptr free);

/* dctor */
void array_deinit (array_ptr array);

/* getter */
int inline
array_fetch(array_ptr array, unsigned index, generic_dptr out)
{
  if (index >= array->num) return ARRAY_OUT_OF_BOUNDS;

  (*out) = *(array->space + index );
  return ARRAY_OK;
}

/* setter */
int array_insert(array_ptr array, unsigned index, generic_ptr obj);

inline unsigned array_count(const array_ptr array)
{ return array->num; }

/* iterators */
inline array_iterator_ptr array_iter(array_ptr array, int dir)
{
  array_iterator_ptr iter;

  if (! (iter = (array_iterator_ptr)(malloc(sizeof(array_iter)))))
    return NULL;

  iter->dir = dir;
  if (dir == ARRAY_ITER_FORWARD) {
    iter->next = array->space;
    iter->last = array->space + array->num;
  }

  else if (dir == ARRAY_ITER_BACKWARD) {
    iter->next = array->space + array->num -1;
    iter->last = array->space;
  }

  else assert(0); /* unexpected */

  return iter;
}

inline void
array_iter_deinit(array_iterator_ptr iter)
{ free(iter); }

inline int
array_iter_next(array_iterator_ptr iter, generic_dptr next)
{
  if (iter->next == iter->last) return 0;

  (*next) = (*iter->next),
    iter->next += iter->dir;

  return 1;
}

/* -- aggregate functions --------------------------------------------------- */
array_ptr array_dup(array_ptr old);
array_ptr array_join (array_ptr array1, array_ptr array2);
int array_append(array_ptr array1, array_ptr array2);
void array_sort (array_ptr array, cmp_func_ptr compare);
void array_uniq (array_ptr array, cmp_func_ptr compare, free_func_ptr free);

/* -- internal functions ---------------------------------------------------- */
int array_resize(array_ptr array, unsigned new_size);

#endif
