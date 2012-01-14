#ifndef HT_INCLUDED
#define HT_INCLUDED

#include "common.h"

typedef struct ht_entry_struct {
  generic_ptr key;
  generic_ptr value;
  struct ht_entry_struct* next;
} ht_entry;
typedef ht_entry* ht_entry_ptr;
typedef ht_entry** ht_entry_dptr;

typedef struct ht_struct {
  ht_entry_dptr table;
  size_t table_size;

  hash_func_ptr hash_func;
  cmp_func_ptr cmp_func;

  size_t entries;
  int prime_index;

} ht;
typedef ht* ht_ptr;
typedef ht** ht_dptr;

typedef struct ht_iterator_struct {
  ht_ptr ht;
  ht_entry_ptr next_entry;
  int next_chain;
} ht_iterator;
typedef ht_iterator* ht_iterator_ptr;
typedef ht_iterator** ht_iterator_dptr;

/* typedef size_t (*ht_hash_func)(generic_ptr value); */
/* typedef int (*ht_equal_func)(generic_ptr value1, */
/*                              generic_ptr value2); */

ht_ptr ht_init(hash_func_ptr hash_func,
               cmp_func_ptr equal_func);

void ht_free(ht_ptr hash);

/* /\** */
/*  * Register functions used to free the key and value when an entry is */
/*  * removed from a hash table. */
/*  * */
/*  * @param ht           The hash table. */
/*  * @param key_free_func        Function used to free keys. */
/*  * @param value_free_func      Function used to free values. */
/*  *\/ */

/* void ht_register_free_functions(Ht *ht, */
/*                                         generic_ptrFreeFunc key_free_func, */
/*                                         generic_ptrFreeFunc value_free_func); */

int ht_insert(ht_ptr hash,
                      generic_ptr key,
                      generic_ptr value);

generic_ptr ht_find(ht_ptr hash,
                            generic_ptr key);

int ht_remove(ht_ptr hash,
                      generic_ptr key);

int ht_count(ht_ptr hash);

/* iterators support */
ht_iterator_ptr ht_iter(ht_ptr hash);
int ht_iter_has_more(ht_iterator_ptr iterator);
generic_ptr ht_iter_next(ht_iterator_ptr iterator);

#endif
