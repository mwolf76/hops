#ifndef HT_INCLUDED
#define HT_INCLUDED

#include "common.h"

#define CHUNK_SIZE 2048

/* -- Typedefs -------------------------------------------------------------- */
typedef struct ht_entry_struct {
  generic_ptr key;
  generic_ptr value;
  struct ht_entry_struct* next;
} ht_entry;
typedef ht_entry* ht_entry_ptr;
typedef ht_entry** ht_entry_dptr;

/* node chunks */
typedef struct ht_chunk_struct {
  unsigned used;
  struct ht_chunk_struct* next;
  ht_entry nodes[CHUNK_SIZE];
} ht_chunk;
typedef ht_chunk* ht_chunk_ptr;

typedef struct ht_struct {
  ht_entry_dptr table;
  size_t table_size;

  /* internal function pointers */
  hash_func_ptr hash_func;
  cmp_func_ptr cmp_func;
  free_func_ptr key_free_func;
  free_func_ptr value_free_func;

  size_t entries;
  size_t next_rehash;

  int prime_index;

  /* for efficient node mgmt */
  ht_chunk_ptr chunks;

} ht;
typedef ht* ht_ptr;
typedef ht** ht_dptr;

typedef struct ht_iterator_struct {
  ht_ptr hash;
  ht_entry_ptr next_entry;
  int next_chain;
} ht_iterator;
typedef ht_iterator* ht_iterator_ptr;
typedef ht_iterator** ht_iterator_dptr;

/* -- Function prototypes --------------------------------------------------- */
ht_ptr ht_init(hash_func_ptr hash_func,
               cmp_func_ptr equal_func,
               free_func_ptr key_free_func,
               free_func_ptr value_free_func);

void ht_deinit(ht_ptr this);

int ht_insert(ht_ptr this,
              generic_ptr key, generic_ptr value);

generic_ptr ht_find(ht_ptr this,
                    generic_ptr key);

int ht_delete(ht_ptr this,
              generic_ptr key);

size_t ht_count(ht_ptr this);

/* iterators */
ht_iterator_ptr ht_iter(ht_ptr hash);
void ht_iter_deinit(ht_iterator_ptr this);
int ht_iter_has_more(ht_iterator_ptr this);
generic_ptr ht_iter_next(ht_iterator_ptr this, generic_dptr value);

#endif
