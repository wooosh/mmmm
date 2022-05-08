#ifndef FRONTEND_PASSES_H
#define FRONTEND_PASSES_H

#include "../common/common.h"

#include <sys/types.h>
#include <stdint.h>

#include "token.h"

struct TokenStream {
  struct Token *tokens;
  size_t len;
  size_t cap;
};
struct TokenStream Lexer_Tokenize(char *text, size_t len);

#include "ast.h"
struct AST_Graph Parse(struct TokenStream ts);

#include "../ir/ssa.h"
struct SSA_Graph GenerateIr(struct AST_Graph);

#endif
