/** Highly Optimized Python Structures
 *
 * (c) 2011 Marco Pensallorto <marco DOT pensallorto AT gmail DOT com>
 *
 **/
#include "array.h"

/* Allocate an array of 'number' elements,each of which require 'size' bytes */
array_ptr array_init(unsigned size, cmp_func_ptr cmp, free_func_ptr free)
{
  size_t fullsize;
  array_ptr array;

  if (! (array = (array_ptr)(malloc(sizeof(array_t)))))
    return NULL;

  array->cmp = cmp;
  array->free = free;

  array->num = 0;
  array->n_size = MAX(ARRAY_INIT_SIZE, size);

  fullsize = array->n_size * sizeof(generic_ptr);
  if (! (array->space = (generic_dptr) malloc(fullsize))) {
    free(array);
    return NULL;
  }

  memset(array->space, 0, fullsize);
  return array;
}


int array_find(array_ptr array, generic_ptr key)
{
  unsigned i;
  generic_dptr elem;

  /* Scans for key */
  for (i=0, elem = array->space;
       i < array->n_size; i ++, elem ++) {

    if ((*elem) && (! array->cmp(*elem, key)))
      return i;
  }

  return -1;
}

int array_fetch(array_ptr array, unsigned index, generic_dptr out)
{
  if (index >= array->num) return ARRAY_OUT_OF_BOUNDS;

  (*out) = *(array->space + index );
  return ARRAY_OK;
}


int array_insert(array_ptr array, unsigned index, generic_ptr buf)
{
  int res = ARRAY_OK;
  assert(array);

  if ((index >= array->n_size) &&
      ((res = array_resize(array, 1 + index)) != ARRAY_OK))
    return res;

  memcpy(array->space + index, buf, sizeof(generic_ptr));
  if (index >= array->num) array->num = index + 1;

  return ARRAY_OK;
}

// ????
/* int array_delete(array_ptr array, unsigned index, generic_dptr item_p) */
/* { */
/*   generic_ptr elem; */
/*   int res = ARRAY_OK; */
/*   assert(array); */

/*   if ((index >= array->n_size) && */
/*       ((res = array_resize(array, 1 + index)) != ARRAY_OK)) */
/*     return res; */

/*   elem = *(array->space + index); */
/*   if (!elem) return 0; /\* not found, nothing to delete *\/ */

/*   (*item_p) = elem; */

/*   memcpy(array->space + index, buf, sizeof(generic_ptr)); */
/*   if (index >= array->num) array->num = index + 1; */

/*   return ARRAY_OK; */

/* } */


unsigned array_n(const array_ptr array)
{
  return array->num;
}

unsigned array_count(const array_ptr array, generic_ptr key)
{
  unsigned res = 0, i;
  generic_dptr elem;

  /* Scans for key */
  for (i=0, elem = array->space;
       i < array->n_size; i ++, elem ++) {

    if ((*elem) && (! array->cmp(*elem, key)))
      res ++ ;
  }

  return res;
}

int array_iter_next(array_iterator_ptr iter, generic_dptr next)
{
  if (iter->next == iter->last) return 0;

  (*next) = (*iter->next),
    iter->next += iter->dir;

  return 1;
}


void array_iter_deinit(array_iterator_ptr iter)
{
  free(iter);
}


void array_deinit(array_ptr array)
{
  assert(array);
  unsigned i;
  generic_dptr elem;


  /* Free all non-null objects */
  for (i=0, elem = array->space;
       i < array->n_size; i ++, elem ++) {
    if (*elem) array->free(*elem);
  }

  free(array->space);
  free(array);
}


/* array_ptr array_dup(array_ptr old) */
/* { */
/*   array_pyt new; */
/*   size_t fullsize; */

/*   if (! (new = (array_ptr)(malloc(sizeof(array_t))))) */
/*     return NULL; */

/*   new->num = old->num; */
/*   new->n_size = old->num; */
/*   new->obj_size = old->obj_size; */
/*   new->index = -new->obj_size; */

/*   fullsize = old->n_size * old->obj_size; */
/*   if (! (new->space = malloc(fullsize))) */
/*     return NULL; */

/*   memcpy(new->space, old->space, fullsize); */
/*   return new; */
/* } */


/* /\* append the elements of array2 to the end of array1 *\/ */
/* int array_append(array_ptr array1, array_ptr array2) */
/* { */
/*   char *pos; */

/*   if (array1->index >= 0) return ARRAY_NESTED_OPERATION; */
/*   if (array1->obj_size != array2->obj_size) return -2; */

/*   /\* make sure array1 has enough room *\/ */
/*   if (array1->n_size < array1->num + array2->num) { */
/*     if (array_resize(array1, array1->num + array2->num) == ARRAY_OUT_OF_MEM) { */
/*       return ARRAY_OUT_OF_MEM; */
/*     } */
/*   } */

/*   pos = array1->space + array1->num * array1->obj_size; */
/*   memcpy(pos, array2->space, array2->num * array2->obj_size); */
/*   array1->num += array2->num; */

/*   return 1; */
/* } */


/* /\* join array1 and array2, returning a new array *\/ */
/* array_ptr array_join(array_ptr array1, array_ptr array2) */
/* { */
/*   array_ptr array; */
/*   char *pos; */

/*   if (array1->obj_size != array2->obj_size) */
/*     return ARRAY_NESTED_OPERATION; */

/*   if (! array = (array_ptr)(malloc(sizeof(array_t)))) */
/*     return NULL; */

/*   array->num = array1->num + array2->num; */
/*   array->n_size = array->num; */
/*   sizeof(generic_ptr) = array1->obj_size; */
/*   array->index = -sizeof(generic_ptr); */

/*   if (! (array->space = malloc(array->n_size * sizeof(generic_ptr)))) { */
/*     free(array); */
/*     return NULL; */
/*   } */

/*   memcpy(array->space, array1->space, array1->num * array1->obj_size); */
/*   pos = array->space + array1->num * array1->obj_size; */

/*   memcpy(pos, array2->space, array2->num * array2->obj_size); */
/*   return array; */
/* } */



int array_resize(array_ptr array, unsigned new_size)
{
  unsigned old_size;
  generic_dptr newspace;

  old_size = array->n_size;
  array->n_size = MAX(array->n_size * 2, new_size);

  if (! (newspace = \
	 (generic_dptr) realloc(array->space,
				array->n_size * sizeof(generic_ptr)))) {
    array->n_size = old_size;
    return ARRAY_OUT_OF_MEM;
  }

  array->space = newspace;
  memset(array->space + old_size * sizeof(generic_ptr), 0,
	 (array->n_size - old_size) * sizeof(generic_ptr));

  return ARRAY_OK;
}

void array_sort(array_ptr array, cmp_func_ptr compare)
{
  qsort((generic_dptr)array->space, array->num, sizeof(generic_ptr), compare);
}


void array_uniq(array_ptr array, cmp_func_ptr compare, free_func_ptr free_func)
{
  int i, last;
  generic_ptr dest, obj1, obj2;

  dest = array->space;
  obj1 = array->space;
  obj2 = array->space + 1;
  last = array->num;

  for(i = 1; i < last; i++) {
    if ((*compare)((char **) obj1, (char **) obj2) != 0) {
      if (dest != obj1) {
	memcpy(dest, obj1, sizeof(generic_ptr));
      }
      dest ++ ;
    }
    else {
      if (free_func != NULL) (*free_func)(obj1);
      array->num--;
    }
    obj1 ++ ;
    obj2 ++ ;
  }

  if (dest != obj1) {
    memcpy(dest, obj1, sizeof(generic_ptr));
  }
}
