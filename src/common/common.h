#ifndef COMMON_H
#define COMMON_H

#include "error.h"
#include "arena.h"
#include "hash.h"

#include <sys/types.h>
#include <stdint.h>

void *Alloc(size_t);
void *Realloc(void *, size_t);
void Free(void*);

#define EnsureLen(alloc, cap, len)\
  _EnsureLen((void**)(alloc), sizeof(**alloc), cap, len);
void _EnsureLen(void **alloc, size_t elem_size, size_t *cap, size_t len);

struct Str {
  char *data;
  size_t len;
};

struct File {
  char *path;
  size_t len;
  char *contents;
};
struct File LoadFile(char *path);

uint64_t MaxUint(uint64_t a, uint64_t b);
uint64_t MinUint(uint64_t a, uint64_t b);

#endif
