#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void *Alloc(size_t s) {
  void *ptr = malloc(s);
  Assume(ptr != NULL, "alloc fail");
  return ptr;
}

void *Realloc(void *ptr, size_t s) {
  ptr = realloc(ptr, s);
  Assume(ptr != NULL, "realloc fail");
  return ptr;
}

void Free(void *ptr) {
  free(ptr);
}

void _EnsureLen(void **alloc, size_t elem_size, size_t *cap, size_t len) {
  if (*cap < len) {
    if (*cap == 0) *cap = 1;
    while (*cap < len) {
      *cap *= 2;
    }
    *alloc = realloc(*alloc, (*cap)*elem_size);
  }
}

struct File LoadFile(char *path) {
  FILE *f = fopen(path, "rb");
  if (f == NULL) {
    Die("unable to open file");
  }

  fseek(f, 0, SEEK_END);
  size_t len = ftell(f);
  fseek(f, 0, SEEK_SET);

  struct File ret = {
    // TODO: FIXME this needs to be freed, and allocated using Alloc()
    .path     = strdup(path),
    .len      = len,
    .contents = malloc(len)
  };

  fread(ret.contents, ret.len, 1, f);
  fclose(f);

  return ret;
}

uint64_t MaxUint(uint64_t a, uint64_t b) {
  if (a>=b) return a;
  else return b;
}

uint64_t MinUint(uint64_t a, uint64_t b) {
  if (a<=b) return a;
  else return b;
}
