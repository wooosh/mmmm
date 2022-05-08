#include "passes.h"
#include "../ir/ssa.h"
#include "../common/common.h"

#include <stdlib.h>

struct Parser {
  struct Token *idx;
  struct Token *end;
  struct AST_Graph ast;
};

// TODO: move to a class based architecture for each node type
// TODO: TryPopPattern


static struct Token *TryPopToken(struct Parser *p, Token_Kind kind) {
  if (p->idx < p->end && p->idx[0].kind == kind) {
    p->idx++;
    return p->idx - 1;
  }

  return NULL;
}

static AST_BinOpKind TokenToBinOp(struct Token *t) {
  switch (t->kind) {
    case kToken_Symbol_Plus:
      return kAST_BinOpAdd;
    case kToken_Symbol_Minus:
      return kAST_BinOpSub;
    default:
      return kAST_BinOpInvalid;
  }
}

static struct AST_Node *Parse_Expression(struct Parser *p) {
  Assume(p->idx != p->end, "unexpected EOF");

  // TODO: allocate node afterward
  struct AST_Node *node = Arena_Alloc(p->ast.arena, sizeof(*node));
  node->kind = kAST_NodeInvalid;

  if (p->idx[0].kind == kToken_Integer) {
    node->kind = kAST_NodeInt;
    node->int_value = p->idx[0].value.integer;
    p->idx++;
  } else if (p->idx[0].kind == kToken_Identifier) {
    node->kind = kAST_NodeRef;
    node->ref_name = p->idx[0].value.name;
    p->idx++;
  }

  if (node->kind != kAST_NodeInvalid) {
    if (p->idx < p->end && TokenToBinOp(p->idx) != kAST_BinOpInvalid) {
      struct AST_Node *binop = Arena_Alloc(p->ast.arena, sizeof(*node));
      binop->kind = kAST_NodeBinOp;
      binop->binop.kind = TokenToBinOp(p->idx);
      p->idx++;
      binop->binop.lhs = node;
      binop->binop.rhs = Parse_Expression(p);
      return binop;
    }
    return node;
  }

  Die("unrecognized token in expression");
}

static struct AST_Node *Parse_Statement(struct Parser *p) {
  Assume(p->idx != p->end, "unexpected EOF");

  switch(p->idx[0].kind) {
    case kToken_Keyword_Let: {
      p->idx++;
      struct AST_Node *node = Arena_Alloc(p->ast.arena, sizeof(*node));
      node->kind = kAST_NodeLet;

      struct Token *id = TryPopToken(p, kToken_Identifier);
      Assume(id != NULL, "expected identifier");
      struct Token *eq = TryPopToken(p, kToken_Symbol_Equal);
      Assume(eq != NULL, "expected =");
      
      node->let.name = id->value.name;
      node->let.val = Parse_Expression(p);

      struct Token *end = TryPopToken(p, kToken_Symbol_Semicolon);
      Assume(end != NULL, "expected ;");
      return node;
    }
    case kToken_Identifier: {
      struct AST_Node *node = Arena_Alloc(p->ast.arena, sizeof(*node));
      node->kind = kAST_NodeAssign;
      node->assign.name = p->idx->value.name;

      p->idx++;
      struct Token *eq = TryPopToken(p, kToken_Symbol_Equal);
      Assume(eq != NULL, "expected =");

      node->assign.val = Parse_Expression(p);
      struct Token *end = TryPopToken(p, kToken_Symbol_Semicolon);
      Assume(end != NULL,"expected ;");
      return node;
    }
    case kToken_Keyword_Return: {
      p->idx++;
      struct AST_Node *node = Arena_Alloc(p->ast.arena, sizeof(*node));
      node->kind = kAST_NodeReturn;
      node->return_value = Parse_Expression(p);
      struct Token *end = TryPopToken(p, kToken_Symbol_Semicolon);
      Assume(end != NULL,"expected ;");
      return node;
    }
  }

  Die("unrecognized token in statement");
}

static struct AST_Node *Parse_Block(struct Parser *p) {
  struct AST_Node *block = Arena_Alloc(p->ast.arena, sizeof(*block));
  block->kind = kAST_NodeBlock;
  block->block.len   = 0;
  block->block.nodes = NULL;

  while (p->idx < p->end) {
    block->block.len++;
    EnsureLen(&block->block.nodes, &block->block.cap, block->block.len);
    block->block.nodes[block->block.len-1] = Parse_Statement(p);
  }

  return block;
}

struct AST_Graph Parse(struct TokenStream ts) {
  struct Parser p = {
    .idx = ts.tokens,
    .end = ts.tokens + ts.len,
    .ast = {
      .arena = Arena_Init()
    }
  };

  p.ast.root = Parse_Block(&p);

  return p.ast;
}

