#include "ssa.h"
#include "stdio.h"
#include "prelude.h"

static const char *InstName(SSA_InstKind kind) {
  switch (kind) {
    case kSSA_Inst_Add: return "add";
    case kSSA_Inst_Sub: return "sub";
    default:
      Die("unknown instruction");
  }
}

void GenerateInst(struct SSA_Inst *inst) {
  /* evaluate dependencies */
  for (size_t i=0; i<inst->arity; i++) {
    struct SSA_Val v = inst->args[i];
    switch (v.kind) {
      case kSSA_Val_Imm:
        printf("mov $%zu, %%rcx\n"
               "push %%rcx\n", v.immediate);
        break;
      case kSSA_Val_Result:
        GenerateInst(inst->args[i].result);
        break;
    }
  }

  /* load values into registers */
  printf("pop %%rax\n"
         "pop %%rdx\n"
         "%s %%rdx, %%rax\n"
         "push %%rax\n",
         InstName(inst->kind));
}
