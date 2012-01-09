/* Hash table implementation */
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

static const unsigned int ht_primes[] = {
  193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317,
  196613, 393241, 786433, 1572869, 3145739, 6291469,
  12582917, 25165843, 50331653, 100663319, 201326611,
  402653189, 805306457, 1610612741,
};

static const int ht_num_primes = sizeof(ht_primes) / sizeof(unsigned int);

/* Internal function used to allocate the table on hash table creation
 * and when enlarging the table */
static int ht_allocate_table(ht_ptr ht)
{
  size_t new_table_size;

  /* Determine the table size based on the current prime index.
   * An attempt is made here to ensure sensible behavior if the
   * maximum prime is exceeded, but in practice other things are
   * likely to break long before that happens. */
  if (ht->prime_index < ht_num_primes) {
    new_table_size = ht_primes[ht->prime_index];
  } else {
    new_table_size = ht->entries * 10;
  }

  ht->table_size = new_table_size;

  /* Allocate the table and initialise to NULL for all entries */
  ht->table = malloc(ht->table_size * sizeof(ht_entry_ptr));

  return ht->table != NULL;
}

/* Free an entry, calling the free functions if there are any registered */
static void ht_free_entry(HashTable *ht, HashTableEntry *entry)
{
  /* /\* If there is a function registered for freeing keys, use it to free */
  /*  * the key *\/ */
  /* if (ht->key_free_func != NULL) { */
  /*   ht->key_free_func(entry->key); */
  /* } */

  /* /\* Likewise with the value *\/ */
  /* if (ht->value_free_func != NULL) { */
  /*   ht->value_free_func(entry->value); */
  /* } */

  /* Free the data structure */
  free(entry);
}

ht_ptr ht_init(hash_func_ptr hash_func,
               cmp_func_ptr cmp_func)
{
  HashTable_ptr ht;

  /* Allocate a new hash table structure */
  if (!(ht = (ht_ptr) malloc(sizeof(ht))))
    return NULL;

  ht->hash_func = hash_func;
  ht->equal_func = equal_func;
  ht->key_free_func = NULL;
  ht->value_free_func = NULL;
  ht->entries = 0;
  ht->prime_index = 0;

  /* Allocate the table */
  if (!ht_allocate_table(ht)) {
    free(ht);
    return NULL;
  }

  return ht;
}

void ht_free(ht_ptr ht)
{
  ht_entry_ptr rover, next;
  int i;

  /* Free all entries in all chains */
  for (i=0; i<ht->table_size; ++i) {
    rover = ht->table[i];
    while (rover != NULL) {
      next = rover->next;
      ht_free_entry(ht, rover);
      rover = next;
    }
  }

  /* Free the table */
  free(ht->table);

  /* Free the hash table structure */
  free(ht);
}

/* void ht_register_free_functions(HashTable *ht, */
/*                                 HashTableKeyFreeFunc key_free_func, */
/*                                 HashTableValueFreeFunc value_free_func) */
/* { */
/*   ht->key_free_func = key_free_func; */
/*   ht->value_free_func = value_free_func; */
/* } */


static inline int ht_enlarge(HashTable *ht)
{
  ht_entry_dptr old_table;
  int old_table_size;
  int old_prime_index;
  ht_entry_ptr rover, next;
  int index, i;

  /* Store a copy of the old table */
  old_table = ht->table;
  old_table_size = ht->table_size;
  old_prime_index = ht->prime_index;

  /* Allocate a new, larger table */
  ++ht->prime_index;

  if (!ht_allocate_table(ht)) {

    /* Failed to allocate the new table */
    ht->table = old_table;
    ht->table_size = old_table_size;
    ht->prime_index = old_prime_index;

    return 0;
  }

  /* Link all entries from all chains into the new table */
  for (i=0; i<old_table_size; ++i) {
    rover = old_table[i];

    while (rover != NULL) {
      next = rover->next;

      /* Find the index into the new table */
      index = ht->hash_func(rover->key) % ht->table_size;

      /* Link this entry into the chain */
      rover->next = ht->table[index];
      ht->table[index] = rover;

      /* Advance to next in the chain */
      rover = next;
    }
  }

  /* Free the old table */
  free(old_table);

  return 1;
}

int ht_insert(ht_ptr ht, generic_ptr key, generic_ptr value)
{
  ht_entry_ptr rover, newentry;
  int index;

  /* If there are too many items in the table with respect to the table
   * size, the number of hash collisions increases and performance
   * decreases. Enlarge the table size to prevent this happening */
  if ((ht->entries * 3) / ht->table_size > 0) {

    /* Table is more than 1/3 full */
    if (!ht_enlarge(ht)) {
      /* Failed to enlarge the table */
      return 0;
    }
  }

  /* Generate the hash of the key and hence the index into the table */
  index = ht->hash_func(key) % ht->table_size;

  /* Traverse the chain at this location and look for an existing
   * entry with the same key */
  rover = ht->table[index];

  while (rover != NULL) {
    if (ht->equal_func(rover->key, key) != 0) {

      /* Same key: overwrite this entry with new data */

      /* If there is a value free function, free the old data
       * before adding in the new data */
      if (ht->value_free_func != NULL) {
        ht->value_free_func(rover->value);
      }

      /* Same with the key: use the new key value and free
       * the old one */
      if (ht->key_free_func != NULL) {
        ht->key_free_func(rover->key);
      }

      rover->key = key;
      rover->value = value;

      /* Finished */
      return 1;
    }
    rover = rover->next;
  }

  /* Not in the hash table yet.  Create a new entry */
  newentry = (HashTableEntry *) malloc(sizeof(HashTableEntry));

  if (newentry == NULL) {
    return 0;
  }

  newentry->key = key;
  newentry->value = value;

  /* Link into the list */
  newentry->next = ht->table[index];
  ht->table[index] = newentry;

  /* Maintain the count of the number of entries */
  ++ht->entries;

  /* Added successfully */
  return 1;
}

generic_ptr ht_lookup(HashTable ht_ptr, generic_ptr key)
{
  ht_entry_ptr rover;
  int index;

  /* Generate the hash of the key and hence the index into the table */
  index = ht->hash_func(key) % ht->table_size;

  /* Walk the chain at this index until the corresponding entry is
   * found */
  rover = ht->table[index];

  while (rover != NULL) {
    if (ht->equal_func(key, rover->key) != 0) {

      /* Found the entry.  Return the data. */
      return rover->value;
    }

    rover = rover->next;
  }

  /* Not found */
  return NULL;
}

int ht_remove(HashTable *ht, HashTableKey key)
{
  HashTableEntry **rover;
  HashTableEntry *entry;
  int index;
  int result;

  /* Generate the hash of the key and hence the index into the table */
  index = ht->hash_func(key) % ht->table_size;

  /* Rover points at the pointer which points at the current entry
   * in the chain being inspected.  ie. the entry in the table, or
   * the "next" pointer of the previous entry in the chain.  This
   * allows us to unlink the entry when we find it. */

  result = 0;
  rover = &ht->table[index];

  while (*rover != NULL) {

    if (ht->equal_func(key, (*rover)->key) != 0) {

      /* This is the entry to remove */
      entry = *rover;

      /* Unlink from the list */
      *rover = entry->next;

      /* Destroy the entry structure */
      ht_free_entry(ht, entry);

      /* Track count of entries */
      --ht->entries;
      result = 1;
      break;
    }

    /* Advance to the next entry */
    rover = &((*rover)->next);
  }

  return result;
}

size_t ht_num_entries(ht_ptr ht)
{
  assert(ht);
  return ht->entries;
}

void ht_iterate(ht_ptr ht, ht_iterator_ptr iterator)
{
  int chain;

  iterator->ht = ht;

  /* Default value of next if no entries are found. */
  iterator->next_entry = NULL;

  /* Find the first entry */
  for (chain=0; chain<ht->table_size; ++chain) {

    if (ht->table[chain] != NULL) {
      iterator->next_entry = ht->table[chain];
      iterator->next_chain = chain;
      break;
    }
  }
}

int ht_iter_has_more(ht_iterator_ptr iterator)
{
  return iterator->next_entry != NULL;
}

generic_ptr ht_iter_next(ht_iterator_ptr iterator)
{
  ht_entry_ptr current_entry;
  HashTable_ptr ht;

  generic_ptr result;
  int chain;

  ht = iterator->ht;

  /* No more entries? */
  if (iterator->next_entry == NULL) {
    return NULL;
  }

  /* Result is immediately available */
  current_entry = iterator->next_entry;
  result = current_entry->value;

  /* Find the next entry */
  if (current_entry->next != NULL) {

    /* Next entry in current chain */
    iterator->next_entry = current_entry->next;

  } else {

    /* None left in this chain, so advance to the next chain */
    chain = iterator->next_chain + 1;

    /* Default value if no next chain found */
    iterator->next_entry = NULL;

    while (chain < ht->table_size) {

      /* Is there anything in this chain? */
      if (ht->table[chain] != NULL) {
        iterator->next_entry = ht->table[chain];
        break;
      }

      /* Try the next chain */
      ++chain;
    }

    iterator->next_chain = chain;
  }

  return result;
}
