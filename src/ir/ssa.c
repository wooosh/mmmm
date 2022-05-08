#include "ssa.h"

#include "../common/common.h"

#include <stdlib.h>

/* TODO: always in order of ctl, mem, data... */
/* TODO: outparams */

#define INST_SIG(...) {\
  .arity = sizeof((SSA_DepKind[]){__VA_ARGS__})/sizeof(SSA_DepKind),\
  .deps_kind = (SSA_DepKind[]){__VA_ARGS__},\
}
#define SIG_VOID {.arity = 0, .deps_kind = NULL}
static struct SSA_InstSig {
  size_t arity;
  SSA_DepKind *deps_kind;
} inst_sig[] = {
  /* control-related instructions */
  [kSSA_Inst_Entry] = SIG_VOID,
  [kSSA_Inst_ReturnVal] = INST_SIG(kSSA_Dep_Ctl, kSSA_Dep_Mem, kSSA_Dep_Data),

  /* memory instructions */
  [kSSA_Inst_Read] = INST_SIG(kSSA_Dep_Mem, kSSA_Dep_Data),
  /* address, value */
  [kSSA_Inst_Write] = INST_SIG(kSSA_Dep_Mem, kSSA_Dep_Data, kSSA_Dep_Data),

  /* ssa "fake" instructions */
  [kSSA_Inst_Const] = INST_SIG(kSSA_Dep_Const),
  [kSSA_Inst_AllocStatic] = SIG_VOID,
  // TODO: phi

#define SIG_BINOP INST_SIG(kSSA_Dep_Data, kSSA_Dep_Data)
  /* bin ops */
  [kSSA_Inst_Add] = SIG_BINOP,
  [kSSA_Inst_Sub] = SIG_BINOP
#undef SIG_BINOP

};
#undef SIG_VOID
#undef INST_SIG

struct SSA_Inst *SSA_NewInst(
  struct SSA_Graph *ssa,
  SSA_InstKind kind,
  size_t arity,
  struct SSA_Dep deps[]
) {
  /* typecheck arguments */
  struct SSA_InstSig sig = inst_sig[kind];
  Assume(sig.arity == arity, "wrong arity for inst");
  for (size_t i=0; i<arity; i++) {
    Assume(sig.deps_kind[i] == deps[i].kind, "mismatched types for inst");
  }

  /* create new instruction */
  struct SSA_Inst *inst = Arena_Alloc(ssa->arena, sizeof(struct SSA_Inst));
  inst->kind = kind;
  inst->arity = arity;
  inst->deps = Arena_Alloc(ssa->arena, arity*sizeof(struct SSA_Dep));
  for (size_t i=0; i<arity; i++) {
    inst->deps[i] = deps[i];
  }

  /* root the instruction if need be */
  if (inst->kind == kSSA_Inst_Return || inst->kind == kSSA_Inst_ReturnVal) {
    struct SSA_Inst **root = Append(&ssa->roots, &ssa->roots_cap, &ssa->roots_len);
    *root = inst;
  }

  return inst;
}

struct SSA_Dep SSA_Const(uint64_t v) {
  return (struct SSA_Dep) {
    .kind = kSSA_Dep_Const,
    .value = v
  };
}

struct SSA_Dep SSA_DepData(struct SSA_Inst *inst) {
  return (struct SSA_Dep) {
    .kind = kSSA_Dep_Data,
    .inst = inst
  };
}

struct SSA_Dep SSA_DepCtl(struct SSA_Inst *inst) {
  return (struct SSA_Dep) {
    .kind = kSSA_Dep_Ctl,
    .inst = inst
  };
}

struct SSA_Dep SSA_DepMem(struct SSA_Inst *inst) {
  return (struct SSA_Dep) {
    .kind = kSSA_Dep_Mem,
    .inst = inst
  };
}