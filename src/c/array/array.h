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
int array_fetch(array_ptr array, unsigned index, generic_dptr out);

/* setter */
int array_insert(array_ptr array, unsigned index, generic_ptr obj);
int array_delete(array_ptr array, unsigned index, generic_dptr obj);

int array_find(array_ptr array, generic_ptr buf);

/* iterators */
array_iterator_ptr array_iter(array_ptr array, int dir);

unsigned array_n(const array_ptr array);

unsigned array_count(const array_ptr array, generic_ptr obj);

void array_iter_deinit(array_iterator_ptr iter);


/* -- aggregate functions --------------------------------------------------- */
array_ptr array_dup(array_ptr old);
array_ptr array_join (array_ptr array1, array_ptr array2);
int array_append(array_ptr array1, array_ptr array2);
void array_sort (array_ptr array, cmp_func_ptr compare);
void array_uniq (array_ptr array, cmp_func_ptr compare, free_func_ptr free);

/* -- internal functions ---------------------------------------------------- */
int array_resize(array_ptr array, unsigned new_size);

#endif
