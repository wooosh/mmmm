#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>

#define Die(msg)\
  Die_(__FILE__, __LINE__, __func__, (msg))
void __attribute__((noreturn)) Die_(const char *filename, int lineno,
                                    const char *func, const char *msg);

#define Assume(condition, msg)\
  Assume_(__FILE__, __LINE__, __func__, (bool)(condition), (msg))
void Assume_(const char *filename, int lineno, const char *func,
             bool cond, const char *msg);

#endif
