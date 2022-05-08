#ifndef KVMAP_H
#define KVMAP_H

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>

/* TODO: change to hashmap */
struct KVMap_Entry {
  size_t key_len;
  const char *key;
  void *val;
};

struct KVMap_Iter {
  struct KVMap_Entry *idx;
  struct KVMap_Entry *end;
};

struct KVMap {
  size_t len;
  size_t cap;
  struct KVMap_Entry *entries;
};

struct KVMap KVMap_Init(void);
void KVMap_Destroy(struct KVMap *);

void KVMap_Set(struct KVMap *, size_t key_len, char *key, void *val);
void *KVMap_Get(struct KVMap *, size_t key_len, char *key);
void KVMap_Remove(struct KVMap *, size_t key_len, char *key);

struct KVMap_Iter KVMap_Begin(struct KVMap *);
void KVMap_Next(struct KVMap_Iter *);
bool KVMap_AtEnd(struct KVMap_Iter);
#endif
