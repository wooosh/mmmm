#include "passes.h"
#include "../common/common.h"
#include "src/ir/ssa.h"

#include <stdlib.h>
#include <string.h>

struct IrVariable {
  struct Str name;
  struct SSA_Inst *alloc;
};

struct IrGen {
  struct SSA_Graph ssa;

  struct SSA_Inst *mem;
  struct SSA_Inst *ctl;

  size_t vars_len;
  size_t vars_cap;
  struct IrVariable *vars;
};

// TODO: enum for arg numbers of instructions

static struct SSA_Inst *GetVarAlloc(struct IrGen *ir, struct Str name) {
  for (size_t i=0; i<ir->vars_len; i++) {
    if (name.len == ir->vars[i].name.len && memcmp(ir->vars[i].name.data, name.data, name.len) == 0) {
      return ir->vars[i].alloc;
    }
  }
  return NULL;
}

static struct SSA_Inst *GenerateNode(struct IrGen *ir, struct AST_Node *node) {
  switch (node->kind) {
    case kAST_NodeBlock: {
      for (size_t i=0; i<node->block.len; i++) {
        GenerateNode(ir, node->block.nodes[i]);
      }
      return NULL;
    };

    case kAST_NodeInt: {
      return SSA_NewInst(&ir->ssa, kSSA_Inst_Const, 1, (struct SSA_Dep[]) {
        SSA_Const(node->int_value)
      });
    };

    case kAST_NodeReturn: {
      return SSA_NewInst(&ir->ssa, kSSA_Inst_ReturnVal, 3, (struct SSA_Dep[]){
        SSA_DepCtl(ir->ctl),
        SSA_DepMem(ir->mem),
        SSA_DepData(GenerateNode(ir, node->return_value)),
      });
    }

    case kAST_NodeBinOp: {
      SSA_InstKind kind = kSSA_Inst_Invalid;
      switch (node->binop.kind) {
        case kAST_BinOpAdd:
          kind = kSSA_Inst_Add;
          break;
        case kAST_BinOpSub:
          kind = kSSA_Inst_Sub;
          break;
        default:
          Die("unimplemented");
      }

      return SSA_NewInst(&ir->ssa, kind, 2, (struct SSA_Dep[]){
        SSA_DepData(GenerateNode(ir, node->binop.lhs)), 
        SSA_DepData(GenerateNode(ir, node->binop.rhs))
      });
    }

    // TODO: break up let into ir and store at the AST level
    case kAST_NodeLet: {
      Assume(GetVarAlloc(ir, node->let.name) == NULL, "variable already declared");
      struct SSA_Inst *alloc = SSA_NewInst(&ir->ssa, kSSA_Inst_AllocStack, 0, NULL);
      
      struct IrVariable *var = Append(&ir->vars, &ir->vars_cap, &ir->vars_len);
      var->name = node->let.name;
      var->alloc = alloc;

      ir->mem = SSA_NewInst(&ir->ssa, kSSA_Inst_Write, 3, (struct SSA_Dep[]) {
        SSA_DepMem(ir->mem),
        SSA_DepData(alloc),
        SSA_DepData(GenerateNode(ir, node->let.val))
      });

      return NULL;
    }

    case kAST_NodeRef: {
      struct SSA_Inst *alloc = GetVarAlloc(ir, node->ref_name);
      Assume(alloc != NULL, "ref must be to declared variable");

      return SSA_NewInst(&ir->ssa, kSSA_Inst_Read, 2, (struct SSA_Dep[]) {
        SSA_DepMem(ir->mem),
        SSA_DepData(alloc)
      });
    }

    default:
      Die("unimplemented");
  }
}

struct SSA_Graph GenerateIr(struct AST_Graph ast) {
  struct IrGen ir = {
    .ssa = {
      .arena = Arena_Init(),

      .pass_id = 0,

      .roots_len = 0,
      .roots_cap = 0,
      .roots = NULL
    },

    .vars_len = 0,
    .vars_cap = 0,
    .vars = NULL,
  };

  struct SSA_Inst *entry = Arena_Alloc(ir.ssa.arena, sizeof(struct SSA_Inst));
  entry->kind = kSSA_Inst_Entry;
  entry->arity = 0;
  entry->deps = NULL;

  ir.mem = entry;
  ir.ctl = entry;

  GenerateNode(&ir, ast.root);

  return ir.ssa;
}
