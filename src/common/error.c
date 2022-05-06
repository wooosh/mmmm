#include "error.h"

#include <stdlib.h>
#include <stdio.h>

void Die_(const char *filename, int lineno, const char *func,
            const char *msg) {
  fprintf(stderr, "%s @ %s:%d - %s\n", func, filename, lineno, msg);
  exit(EXIT_FAILURE);
}

void Assume_(const char *filename, int lineno, const char *func,
             bool cond, const char *msg) {
  if (!cond) {
    Die_(filename, lineno, func, msg);
  }
}
