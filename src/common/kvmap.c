#include "kvmap.h"

#include "common.h"

#include <stdlib.h>
#include <string.h>

struct KVMap KVMap_Init(void) {
  return (struct KVMap) {
    .len = 0,
    .cap = 0,
    .entries = NULL
  };
}

void KVMap_Destroy(struct KVMap *map) {
  Free(map->entries);
  map->entries = NULL;
  map->len = 0;
  map->cap = 0;
}

void KVMap_Set(struct KVMap *m, size_t key_len, char *key, void *val) {
  /* TODO: should key be duplicated/allocated separately */
  m->len++;
  EnsureLen(&m->entries, &m->cap, m->len);
  m->entries[m->len-1] = (struct KVMap_Entry) {
    .key_len = key_len,
    .key = key,
    .val = val
  };
}

void *KVMap_Get(struct KVMap *m, size_t key_len, char *key) {
  for (size_t i=0; i<m->len; i++) {
    struct KVMap_Entry e = m->entries[i];
    if (e.key_len == key_len && memcmp(e.key, key, key_len) == 0) {
      return e.val;
    }
  }
  return NULL;
}

void KVMap_Remove(struct KVMap *m, size_t key_len, char *key) {
  Die("unimplemented");
}

struct KVMap_Iter KVMap_Begin(struct KVMap *m) {
  Die("unimplemented");
}
void KVMap_Next(struct KVMap_Iter *i) {
  Die("unimplemented");
}
bool KVMap_AtEnd(struct KVMap_Iter i) {
  Die("unimplemented");
}