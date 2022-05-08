#include "../ir/ssa.h"

#include <stdio.h>

// TODO: fix
FILE *out;

// TODO: handle multiple uses of a data instruction
static void GenInst(size_t pass_id, struct SSA_Inst *inst, SSA_DepKind dep) {
  /* ignore virtual instructions */
  if (inst->kind == kSSA_Inst_Entry) {
    return;
  }

  if (inst->kind == kSSA_Inst_Write) {
    if (inst->pass_id == pass_id) return;
    inst->pass_id = pass_id;
    GenInst(pass_id, inst->deps[0].inst, inst->deps[0].kind);
    Assume(inst->deps[1].inst->kind == kSSA_Inst_AllocStack, "");
    GenInst(pass_id, inst->deps[2].inst, inst->deps[0].kind);
    fprintf(out, "pop -%zu(%%rbp)\n", inst->deps[1].inst->pass_value.i);
    return;
  }

  if (dep == kSSA_Dep_Data && inst->kind == kSSA_Inst_Read) {
    GenInst(pass_id, inst->deps[0].inst, inst->deps[0].kind);
    Assume(inst->deps[1].inst->kind == kSSA_Inst_AllocStack, "");
    fprintf(out, "push -%zu(%%rbp)\n", inst->deps[1].inst->pass_value.i);
    return;
  }



  for (size_t i=0; i<inst->arity; i++) {
    if (inst->deps[i].kind != kSSA_Dep_Const) {
      GenInst(pass_id, inst->deps[i].inst, inst->deps[i].kind);
    }
  }

  switch (inst->kind) {
    case kSSA_Inst_Const:
      fprintf(out, "push $%zu\n", inst->deps[0].value);
      break;
    case kSSA_Inst_AllocStack:
      //fprintf(out, "push $%zu\n", inst->pass_value.i);
      break;
    case kSSA_Inst_ReturnVal:
      fprintf(out,
        "pop %%rax\n"
        "mov %%rbp, %%rsp\n"
        "pop %%rbp\n"
        "ret\n"
      );
      break;
    case kSSA_Inst_Add:
      fprintf(out,
        "pop %%rax\n"
        "pop %%rcx\n"
        "add %%rcx, %%rax\n"
        "push %%rax\n"
      );
      break;
    case kSSA_Inst_Sub:
      fprintf(out,
        "pop %%rax\n"
        "pop %%rcx\n"
        "add %%rcx, %%rax\n"
        "push %%rax\n"
      );
      break;
  }
}

/* tags AllocStack instructions with stack offset */
// TODO: rewrite into const node for const propagation?
static void HandleStackAllocation(size_t pass_id, size_t *stack_size, struct SSA_Inst *inst) {
  if (inst->pass_id == pass_id) return;

  inst->pass_id = pass_id;

  if (inst->kind != kSSA_Inst_AllocStack) {
    for (size_t i=0; i<inst->arity; i++) {
      if (inst->deps[i].kind != kSSA_Dep_Const) {
        HandleStackAllocation(pass_id, stack_size, inst->deps[i].inst);
      }
    }
    return;
  }

  // TODO: handle size of stack allocation
  inst->pass_value.i = *stack_size;
  *stack_size += 8;
}

void CodeGen(struct SSA_Graph *ssa) {
  out = stdout;

  /* write prologue */
  fprintf(out,
    ".text\n"
    ".globl main\n"
    "main:\n"
    "push %%rbp\n"
    "mov %%rsp, %%rbp\n"
  );

  /* build stack frame, tag AllocStack instruction with stack offset */
  size_t stack_size = 8;
  ssa->pass_id++;
  
  for (size_t i=0; i<ssa->roots_len; i++) {
    // TODO: probably needs to change when control flow is introduced
    HandleStackAllocation(ssa->pass_id, &stack_size, ssa->roots[i]);
  }

  /* reserve stack space */
  fprintf(out, "sub $%zu, %%rsp\n", stack_size);

  ssa->pass_id++;
  for (size_t i=0; i<ssa->roots_len; i++) {
    GenInst(ssa->pass_id, ssa->roots[i], kSSA_Dep_Ctl);
  }
}