/* Hash table implementation */
#include <stdlib.h>
#include <string.h>

#include "ht.h"

static const unsigned int ht_primes[] = {
  193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317,
  196613, 393241, 786433, 1572869, 3145739, 6291469,
  12582917, 25165843, 50331653, 100663319, 201326611,
  402653189, 805306457, 1610612741,
};

static const int ht_num_primes = \
  sizeof(ht_primes) / sizeof(unsigned int);


/* -- internal functions ---------------------------------------------------- */
static int ht_allocate_table(ht_ptr this);
static void ht_free_entry(ht_ptr this, ht_entry_ptr entry);
static int ht_grow(ht_ptr this);

ht_ptr ht_init(hash_func_ptr hash_func,
               cmp_func_ptr cmp_func,
               free_func_ptr key_free_func,
               free_func_ptr value_free_func)
{
  ht_ptr this;

  /* Allocate a new hash table structure */
  if (!(this = (ht_ptr) malloc(sizeof(ht))))
    return NULL;

  this->hash_func = hash_func;
  this->cmp_func = cmp_func;
  this->key_free_func = key_free_func;
  this->value_free_func = value_free_func;
  this->entries = 0;
  this->prime_index = 0;

  /* Allocate the table */
  if (!ht_allocate_table(this)) {
    free(this);
    return NULL;
  }

  return this;
}

void ht_deinit(ht_ptr this)
{
  CHECK_INSTANCE(this);

  ht_entry_ptr rover, next;
  int i;

  /* Free all entries in all chains */
  for (i=0; i<this->table_size; ++i) {
    rover = this->table[i];

    while (rover != NULL) {
      next = rover->next;
      ht_free_entry(this, rover);
      rover = next;
    }
  }

  /* Free the table */
  free(this->table);

  /* Free the hash table structure */
  free(this);
}

int ht_insert(ht_ptr this, generic_ptr key, generic_ptr value)
{
  CHECK_INSTANCE(this);

  ht_entry_ptr rover, newentry;
  unsigned index;

  /* If there are too many items in the table with respect to the table
   * size, the number of hash collisions increases and performance
   * decreases. Grow the table size to prevent this happening */
  if ((this->entries * 3) / this->table_size > 0) {

    /* Table is more than 1/3 full */
    if (!ht_grow(this)) {
      /* Failed to grow the table */
      return 0;
    }
  }

  /* Generate the hash of the key and hence the index into the table */
  index = this->hash_func(key) % this->table_size;

  /* Traverse the chain at this location and look for an existing
   * entry with the same key */
  rover = this->table[index];

  while (rover != NULL) {
    if (!(this->cmp_func(rover->key, key))) {

      /* Same key: overwrite this entry with new data */

      /* If there is a value free function, free the old data
       * before adding in the new data */
      if (this->value_free_func != NULL) {
        this->value_free_func(rover->value);
      }

      /* Same with the key: use the new key value and free
       * the old one */
      if (this->key_free_func != NULL) {
        this->key_free_func(rover->key);
      }

      rover->key = key;
      rover->value = value;

      /* Finished */
      return 1;
    }
    rover = rover->next;
  }

  /* Not in the hash table yet.  Create a new entry */
  newentry = (ht_entry_ptr) malloc(sizeof(ht_entry));

  if (newentry == NULL) {
    return 0;
  }

  newentry->key = key;
  newentry->value = value;

  /* Link into the list */
  newentry->next = this->table[index];
  this->table[index] = newentry;

  /* Maintain the count of the number of entries */
  ++this->entries;

  /* Added successfully */
  return 1;
}

generic_ptr ht_find(ht_ptr this, generic_ptr key)
{
  CHECK_INSTANCE(this);

  ht_entry_ptr rover;
  int index;

  /* Generate the hash of the key and hence the index into the table */
  index = this->hash_func(key) % this->table_size;

  /* Walk the chain at this index until the corresponding entry is
   * found */
  rover = this->table[index];

  while (rover != NULL) {
    if (!(this->cmp_func(key, rover->key))) {

      /* Found the entry.  Return the data. */
      return rover->value;
    }

    rover = rover->next;
  }

  /* Not found */
  return NULL;
}

int ht_delete(ht_ptr this, generic_ptr key)
{
  CHECK_INSTANCE(this);

  ht_entry_dptr rover;
  ht_entry_ptr entry;
  int index;
  int result;

  /* Generate the hash of the key and hence the index into the table */
  index = this->hash_func(key) % this->table_size;

  /* Rover points at the pointer which points at the current entry
   * in the chain being inspected.  ie. the entry in the table, or
   * the "next" pointer of the previous entry in the chain.  This
   * allows us to unlink the entry when we find it. */

  result = 0;
  rover = &this->table[index];

  while (*rover != NULL) {

    if (!(this->cmp_func(key, (*rover)->key))) {

      /* This is the entry to delete */
      entry = *rover;

      /* Unlink from the list */
      *rover = entry->next;

      /* Destroy the entry structure */
      ht_free_entry(this, entry);

      /* Track count of entries */
      --this->entries;
      result = 1;
      break;
    }

    /* Advance to the next entry */
    rover = &((*rover)->next);
  }

  return result;
}

size_t ht_count(ht_ptr this)
{
  CHECK_INSTANCE(this);

  return this->entries;
}

ht_iterator_ptr ht_iter(ht_ptr this)
{
  CHECK_INSTANCE(this);

  ht_iterator_ptr res = (ht_iterator_ptr)(malloc(sizeof(ht_iterator)));
  int chain;

  res->hash = this;

  /* Default value of next if no entries are found. */
  res->next_entry = NULL;

  /* Find the first entry */
  for (chain=0; chain<this->table_size; ++chain) {

    if (this->table[chain] != NULL) {
      res->next_entry = this->table[chain];
      res->next_chain = chain;
      break;
    }
  }

  return res;
}

void ht_iter_deinit(ht_iterator_ptr this)
{
  CHECK_INSTANCE(this);

  free(this);
}

int ht_iter_has_more(ht_iterator_ptr this)
{
  CHECK_INSTANCE(this);

  return this->next_entry != NULL;
}

/* returns key as generic_ptr. If value_p is non-NULL stores value in
   (*value_p) before returning. */
generic_ptr ht_iter_next(ht_iterator_ptr this, generic_dptr value_p)
{
  CHECK_INSTANCE(this);

  ht_entry_ptr current_entry;
  ht_ptr hash = this->hash;

  generic_ptr result;
  int chain;

  /* No more entries? */
  if (this->next_entry == NULL) {
    return NULL;
  }

  /* Result is immediately available */
  current_entry = this->next_entry;
  result = current_entry->key;
  if (value_p) (*value_p) = current_entry->value;

  /* Find the next entry */
  if (current_entry->next != NULL) {

    /* Next entry in current chain */
    this->next_entry = current_entry->next;

  } else {

    /* None left in this chain, so advance to the next chain */
    chain = this->next_chain + 1;

    /* Default value if no next chain found */
    this->next_entry = NULL;

    while (chain < hash->table_size) {

      /* Is there anything in this chain? */
      if (hash->table[chain] != NULL) {
        this->next_entry = hash->table[chain];
        break;
      }

      /* Try the next chain */
      ++chain;
    }

    this->next_chain = chain;
  }

  return result;
}

/* Internal function used to allocate the table on hash table creation
 * and when growing the table */
static int ht_allocate_table(ht_ptr this)
{
  size_t sz;

  /* TODO: what if following assertion does not hold? */
  assert (this->prime_index < ht_num_primes);
  this->table_size = ht_primes[this->prime_index];

  /* Allocate the table and initialise to NULL for all entries */
  sz = this->table_size * sizeof(ht_entry_ptr);
  if (this->table = malloc(sz))
    memset(this->table, 0, sz);

  return this->table != NULL;
}

/* Free an entry, calling the free functions if there are any registered */
static void ht_free_entry(ht_ptr this, ht_entry_ptr entry)
{
  /* If there is a function registered for freeing keys, use it to free
   * the key */
  if (this->key_free_func != NULL) {
    this->key_free_func(entry->key);
  }

  /* Likewise with the value */
  if (this->value_free_func != NULL) {
    this->value_free_func(entry->value);
  }

  /* Free the data structure */
  free(entry);
}

static int ht_grow(ht_ptr this)
{
  ht_entry_dptr old_table;
  int old_table_size;
  int old_prime_index;
  ht_entry_ptr rover, next;
  int index, i;

  /* Store a copy of the old table */
  old_table = this->table;
  old_table_size = this->table_size;
  old_prime_index = this->prime_index;

  /* Allocate a new, larger table */
  ++this->prime_index;

  if (!ht_allocate_table(this)) {

    /* Failed to allocate the new table */
    this->table = old_table;
    this->table_size = old_table_size;
    this->prime_index = old_prime_index;

    return 0;
  }

  /* Link all entries from all chains into the new table */
  for (i=0; i<old_table_size; ++i) {
    rover = old_table[i];

    while (rover != NULL) {
      next = rover->next;

      /* Find the index into the new table */
      index = this->hash_func(rover->key) % this->table_size;

      /* Link this entry into the chain */
      rover->next = this->table[index];
      this->table[index] = rover;

      /* Advance to next in the chain */
      rover = next;
    }
  }

  /* Free the old table */
  free(old_table);

  return 1;
}
