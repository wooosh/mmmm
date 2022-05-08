#ifndef PARSER_H
#define PARSER_H
#include "../common/common.h"

#include "../ir/ssa.h"

#include <stdint.h>

typedef uint8_t AST_NodeKind;
enum {
  kAST_NodeInvalid = 0,
  kAST_NodeBlock,
  kAST_NodeInt,
  kAST_NodeRef,
  kAST_NodeLet,
  kAST_NodeAssign,
  kAST_NodeBinOp,
  kAST_NodeReturn
};

typedef uint8_t AST_BinOpKind;
enum {
  kAST_BinOpInvalid = 0,
  kAST_BinOpAdd,
  kAST_BinOpSub,
};

struct AST_Node {
  AST_NodeKind kind;
  union {
    // kAST_NodeBlock
    struct {
      size_t len;
      size_t cap;
      struct AST_Node **nodes;
    } block;
    // kAST_NodeInt
    uint64_t int_value;
    // kAST_NodeRef
    struct Str ref_name;
    // kAST_NodeLet
    struct {
      struct Str name;
      struct AST_Node *val;
    } let;
    // kAST_NodeAssign
    struct {
      struct Str name;
      struct AST_Node *val;
    } assign;
    // kAST_NodeBinOP
    struct {
      AST_BinOpKind kind;
      struct AST_Node *lhs;
      struct AST_Node *rhs;
    } binop;
    // kAST_NodeReturn
    struct AST_Node *return_value;
  };
};

struct AST_Graph {
  struct Arena *arena;
  struct AST_Node *root;
};

void AST_Print(struct AST_Node *node, size_t indent);
#endif
