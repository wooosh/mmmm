#include "common/common.h"
#include "frontend/lexer.h"
#include "frontend/parser.h"

#include <stdlib.h>
#include <stdio.h>


int main(int argc, char **argv) {
  if (argc != 2) {
    Die("expected exactly one filename argument");
  }

  struct File file = LoadFile(argv[1]);
  struct TokenStream ts = Lexer_Tokenize(file.contents, file.len);
  for (size_t i=0; i<ts.len; i++) {
    printf("%zu %zu %zu: %.*s\n", ts.tokens[i].kind, ts.tokens[i].offset, ts.tokens[i].len, (int)ts.tokens[i].len, file.contents + ts.tokens[i].offset);
  }

  printf("func main() ");
  AST_Print(Parse(ts).root, 0);

  return EXIT_SUCCESS;

  //struct SSA_Inst *inst = Parse(ts);

  //printf(".text\n"
  //       ".global main\n"
  //       "main:\n");
  //GenerateInst(inst);
  //printf("pop %%rax\n"
  //       "ret\n");

  return EXIT_SUCCESS;
}