#ifndef HT_INCLUDED
#define HT_INCLUDED

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

  HtHashFunc hash_func;
  HtEqualFunc equal_func;

  HtKeyFreeFunc key_free_func;
  HtValueFreeFunc value_free_func;

  size_t entries;
  int prime_index;

} ht;
typedef ht* ht_ptr;
typedef ht** ht_dptr;

typedef struct ht_iterator_struct {
	ht_ptr hash_table;
	ht_entry_ptr next_entry;
	int next_chain;
} ht_iterator;
typedef ht_iterator* ht_iterator_ptr;
typedef ht_iterator** ht_iterator_dptr;

typedef size_t (*ht_hash_func)(generic_ptr value);
typedef int (*ht_equal_func)(generic_ptr value1,
                             generic_ptr value2);

/* /\** */
/*  * Type of function used to free keys when entries are removed from a */
/*  * hash table. */
/*  *\/ */

/* typedef void (*generic_ptrFreeFunc)(generic_ptr value); */

/* /\** */
/*  * Type of function used to free values when entries are removed from a */
/*  * hash table. */
/*  *\/ */

/* typedef void (*generic_ptrFreeFunc)(generic_ptr value); */

ht_ptr hash_table_new(ht_hash_func hash_func,
                      ht_equal_func equal_func);

void hash_table_free(ht_ptr hash);

/* /\** */
/*  * Register functions used to free the key and value when an entry is */
/*  * removed from a hash table. */
/*  * */
/*  * @param hash_table           The hash table. */
/*  * @param key_free_func        Function used to free keys. */
/*  * @param value_free_func      Function used to free values. */
/*  *\/ */

/* void hash_table_register_free_functions(Ht *hash_table, */
/*                                         generic_ptrFreeFunc key_free_func, */
/*                                         generic_ptrFreeFunc value_free_func); */

int hash_table_insert(ht_ptr hash,
                      generic_ptr key,
                      generic_ptr value);

generic_ptr hash_table_lookup(Ht_ptr hash,
                              generic_ptr key);

int hash_table_remove(ht_ptr hash,
                      generic_ptr key);

int hash_table_num_entries(ht_ptr hash);

/* iterators support */
void hash_table_iterate(ht_ptr hash, ht_iterator_ptr iter);
int hash_table_iter_has_more(ht_iterator_ptr iterator);
generic_ptr hash_table_iter_next(ht_iterator_ptr iterator);

#endif
