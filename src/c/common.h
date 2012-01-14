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
typedef void (*free_func_ptr)(generic_ptr data);
typedef void (*iter_func_ptr)(generic_ptr key, generic_ptr value);

typedef unsigned (*hash_func_ptr)(const generic_ptr a);
typedef int (*cmp_func_ptr)(const generic_ptr a, generic_ptr b); /* 0 -> equal */

/* -- Macros ---------------------------------------------------------------- */
#define MAX(a,b)				                               \
  ((a) > (b) ? (a) : (b))

#endif
