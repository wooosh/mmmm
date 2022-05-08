#ifndef SSA_H
#define SSA_H

#include "../common/common.h"

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t SSA_InstKind;
enum {
  kSSA_Inst_Invalid = 0,
  
  /* () -> (ctl, mem) */
  kSSA_Inst_Entry,

  /* (ctl, mem) -> (ctl, mem) */
  kSSA_Inst_Region,

  /* (ctl, mem) -> void */
  kSSA_Inst_Return,
  /* (ctl, mem, data) -> void */
  kSSA_Inst_ReturnVal,
  /* (ctl, mem) -> (ctl, mem) */
  kSSA_Inst_Jmp,
  /* (ctl, mem, data) -> (ctl, mem) */
  kSSA_Inst_Branch,
  
  /* (ctl, mem, data:target, data:...params) -> data */
  kSSA_Inst_Call,

  /* (ctl, data) -> data */
  kSSA_Inst_Phi,

  /* mem -> (mem, data) */
  kSSA_Inst_Read,
  /* (mem, data) -> */
  kSSA_Inst_Write,
  
  kSSA_Inst_AllocStack,
  kSSA_Inst_AllocStatic,

  kSSA_Inst_Const,

  kSSA_Inst_Add,
  kSSA_Inst_Sub,
  kSSA_Inst_Div,
  kSSA_Inst_Mul
};

typedef uint8_t SSA_DepKind;
enum {
  kSSA_Dep_Invalid = 0,
  kSSA_Dep_Data,
  kSSA_Dep_Ctl,
  kSSA_Dep_Mem,
  // TODO: remove const deps, make union with const ssa variable and set arity to zero
  // or SSA_HasInst
  kSSA_Dep_Const
};

struct SSA_Dep {
  SSA_DepKind kind;
  union {
    struct SSA_Inst *inst;
    uint64_t value;
  };
};

struct SSA_Inst {
  SSA_InstKind kind;
  uint8_t arity;
  struct SSA_Dep *deps;

  /* id of most recent pass to visit a node */
  size_t pass_id;
  union {
    void *p;
    uint64_t i;
  } pass_value;
};

struct SSA_Graph {
  struct Arena *arena;

  size_t pass_id;

  size_t roots_len;
  size_t roots_cap;
  struct SSA_Inst **roots;
};

struct SSA_Dep SSA_Const(uint64_t);
/* TODO: typecheck these */
struct SSA_Dep SSA_DepData(struct SSA_Inst *);
struct SSA_Dep SSA_DepCtl(struct SSA_Inst *);
struct SSA_Dep SSA_DepMem(struct SSA_Inst *);

struct SSA_Inst *SSA_NewInst(
  struct SSA_Graph *ssa,
  SSA_InstKind kind,
  size_t arity,
  struct SSA_Dep deps[]
);

#endif
