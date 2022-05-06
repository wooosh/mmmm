#include "arena.h"
#include "common.h"

#include <stdlib.h>
#include <string.h>

struct Arena *Arena_Init(void) {
  struct Arena *arena = Alloc(sizeof(*arena));
  arena->start = (uintptr_t) Alloc(ARENA_BLOCK_SIZE);
  arena->cursor = arena->start + ARENA_BLOCK_SIZE;
  arena->prev = NULL;
  return arena;
}

void *Arena_Alloc(struct Arena *arena, size_t size) {
  /* subtract size from current allocation and align to 8 bytes */
  uintptr_t ptr = (arena->cursor - size) & ~(8 - 1);

  if (ptr > arena->start) {
    arena->cursor = ptr;
    return (void*) ptr;
  } else {
    struct Arena *prev_arena = Alloc(sizeof(*prev_arena));
    memcpy(prev_arena, arena, sizeof(*arena));
    arena->prev = prev_arena;

    arena->start = (uintptr_t) Alloc(ARENA_BLOCK_SIZE);
    arena->cursor = arena->start + ARENA_BLOCK_SIZE;
    
    return Arena_Alloc(arena, size);
  }
}

void Arena_Destroy(struct Arena *arena) {
  do {
    Free((void*) arena->start);
    struct Arena *tmp = arena->prev;
    Free(arena);
    arena = tmp;
  } while (arena->prev != NULL);
}
