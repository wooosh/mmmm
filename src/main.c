#include "common/common.h"
#include "frontend/passes.h"
#include "codegen/codegen.h"

#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

void SSA_PrintInst(size_t *inst_id, size_t pass_id, struct SSA_Inst *inst) {
  if (inst->pass_id == pass_id) return;

  for (size_t i=0; i<inst->arity; i++) {
    if (inst->deps[i].kind != kSSA_Dep_Const) {
      SSA_PrintInst(inst_id, pass_id, inst->deps[i].inst);
    }
  }

  inst->pass_id = pass_id;
  inst->pass_value.i = *inst_id;
  (*inst_id)++;

  static const char *inst_name[] = {
    [kSSA_Inst_Entry] = "Entry",
    [kSSA_Inst_ReturnVal] = "Return",
    [kSSA_Inst_Const] = "Const",
    [kSSA_Inst_Add] = "Add",
    [kSSA_Inst_Sub] = "Sub",
    [kSSA_Inst_AllocStack] = "AllocStack",
    [kSSA_Inst_Read] = "Read",
    [kSSA_Inst_Write] = "Write"
  };

  printf("%3zu = %s(", inst->pass_value.i, inst_name[inst->kind]);
  for (size_t i=0; i<inst->arity; i++) {
    struct SSA_Dep dep = inst->deps[i];
    if (dep.kind == kSSA_Dep_Const) {
      printf("const:%zu", dep.value);
    } else {
      static const char *dep_str[] = {
        [kSSA_Dep_Data] = "data",
        [kSSA_Dep_Mem] = "mem",
        [kSSA_Dep_Ctl] = "ctl"
      };

      printf("%s:%zu", dep_str[dep.kind], dep.inst->pass_value.i);
    }

    if (i+1 != inst->arity) {
      printf(", ");
    }
  }
  printf(")\n");
}

void SSA_PrintGraph(struct SSA_Graph *g) {
  // TODO: label control ranges in output
  size_t inst_id = 0;
  g->pass_id++;

  for (size_t i=0; i<g->roots_len; i++) {
    SSA_PrintInst(&inst_id, g->pass_id, g->roots[i]);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    Die("expected exactly one filename argument");
  }

  struct File file = LoadFile(argv[1]);
  struct TokenStream ts = Lexer_Tokenize(file.contents, file.len);
  //for (size_t i=0; i<ts.len; i++) {
  //  printf("%zu %zu %zu: %.*s\n", ts.tokens[i].kind, ts.tokens[i].offset, ts.tokens[i].len, (int)ts.tokens[i].len, file.contents + ts.tokens[i].offset);
  //}

  //printf("func main() ");
  struct AST_Graph ast = Parse(ts);
  //sAST_Print(ast.root, 0);

  struct SSA_Graph ssa = GenerateIr(ast);
  //printf("digraph G {\n");
  //SSA_Print(ssa.roots[0]);
  //SSA_PrintGraph(&ssa);

  CodeGen(&ssa);

  return EXIT_SUCCESS;
}
