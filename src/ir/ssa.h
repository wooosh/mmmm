#ifndef SSA_H
#define SSA_H

#include "../common/common.h"

#include <stdint.h>
#include <stdbool.h>

// TODO: use void types in ssa

typedef uint8_t SSA_InstKind;
enum {
  kSSA_Inst_Invalid = 0,
  
  /* Binding Instructions
   * These instructions "bind" a value of a given type from an instruction
   * that returns a tuple of values. This allows the IR to maintain one value
   * per instruction.
   *
   * These instructions are also known as projection nodes.
   */
  /* (...) -> data */
  kSSA_Inst_BindData,
  /* (...) -> ctl */
  kSSA_Inst_BindCtl,
  /* (...) -> mem */
  kSSA_Inst_BindMem,

  /* (ctl, mem) -> (ctl, mem) */
  kSSA_Inst_Region,

  /* Terminal Instructions
   * These instructions pass the memory and control state to the next block or
   * terminate the function.
   */
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
  /* (mem, datprelude.h) -> */
  kSSA_Inst_Write,
  
  kSSA_Inst_AllocStack,
  kSSA_Inst_AllocStatic,

  kSSA_Inst_Add,
  kSSA_Inst_Sub,
  kSSA_Inst_Div,
  kSSA_Inst_Mul
};

typedef uint8_t SSA_DepKind;
enum {
  kSSA_Dep_Data,
  kSSA_Dep_Ctl,
  kSSA_Dep_Mem
};

struct SSA_Dep {
  SSA_DepKind kind;
  struct SSA_Inst *inst;
};

struct SSA_Inst {
  SSA_InstKind kind;
  uint8_t arity;
  struct SSA_Dep *deps;
};

struct SSA_Graph {
  struct Arena *arena;
  
  size_t roots_len;
  size_t roots_cap;
  struct SSA_Inst *roots;
};

#endif
