#ifndef COMMON_DEFINED
#define COMMON_DEFINED

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* -- Type definitions ------------------------------------------------------ */
typedef void* generic_ptr;
typedef void** generic_dptr;
typedef void*** generic_tptr;
typedef void (*free_func_ptr)(generic_ptr data);
typedef void (*iter_func_ptr)(generic_ptr key, generic_ptr value);
typedef int (*cmp_func_ptr)(generic_ptr a, generic_ptr b);

/* -- Macros ---------------------------------------------------------------- */
#define MAX(a,b)				                               \
  ((a) > (b) ? (a) : (b))

#endif
