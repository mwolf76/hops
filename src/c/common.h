#ifndef COMMON_DEFINED
#define COMMON_DEFINED

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* -- Type definitions ------------------------------------------------------ */

/* generic pointer at different levels of indirection */
typedef void* generic_ptr;
typedef void** generic_dptr;
typedef void*** generic_tptr;

/* function pointer types */
typedef void (*free_func_ptr)(void* data);
typedef void (*iter_func_ptr)(void* key, void* value);

typedef int (*cmp_func_ptr)(const void* a, const void* b);

/* -- Macros ---------------------------------------------------------------- */
#define MAX(a,b)				                               \
  ((a) > (b) ? (a) : (b))

#endif
