/* arena.h
 * copyright 2022 wooosh
 * 
 * simple arena allocator
 * - all allocations are aligned to 8 bytes
 * - allocations grow downward
 * - the allocator maintains a singly linked list of blocks to deallocate when
 *   it is destroyed
 */

#ifndef ARENA_H
#define ARENA_H

#include <stdint.h>
#include <sys/types.h>

#define ARENA_BLOCK_SIZE 4096

struct Arena {
  uintptr_t start;
  uintptr_t cursor;
  struct Arena *prev;
};

struct Arena *Arena_Init(void);
void *Arena_Alloc(struct Arena *, size_t);
void Arena_Destroy(struct Arena *);

#endif
