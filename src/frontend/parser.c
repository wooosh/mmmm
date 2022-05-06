#include "parser.h"
#include "../ir/ssa.h"
#include "lexer.h"
#include "../common/common.h"

#include <stdlib.h>

struct Parser {
  struct Token *idx;
  struct Token *end;
  struct AST_Graph ast;
};

// TODO: move to a class based architecture for each node type

static struct Token *TryPopToken(struct Parser *p, Lexer_TokenKind kind) {
  if (p->idx < p->end && p->idx[0].kind == kind) {
    p->idx++;
    return p->idx - 1;
  }

  return NULL;
}

static struct AST_Node *Parse_Expression(struct Parser *p) {
  Assume(p->idx != p->end, "unexpected EOF");

  // TODO: allocate node afterward
  struct AST_Node *node = Arena_Alloc(p->ast.arena, sizeof(*node));
  node->kind = kAST_NodeInvalid;

  if (p->idx[0].kind == kLexer_Token_Integer) {
    node->kind = kAST_NodeInt;
    node->int_value = p->idx[0].value.integer;
    p->idx++;
  } else if (p->idx[0].kind == kLexer_Token_Identifier) {
    node->kind = kAST_NodeRef;
    node->ref_name = p->idx[0].value.name;
    p->idx++;
  }

  // detect binary op
  if (node->kind != kAST_NodeInvalid) {
    if (p->idx < p->end && p->idx[0].kind == kLexer_Token_Symbol) {
      AST_BinOpKind operation = kAST_BinOpInvalid;
      switch (p->idx[0].value.sym_kind) {
        case kLexer_Symbol_Plus:
          operation = kAST_BinOpAdd;
          break;
        case kLexer_Symbol_Minus:
          operation = kAST_BinOpSub;
          break;
        default:
          return node;
      }
      p->idx++;

      struct AST_Node *binop = Arena_Alloc(p->ast.arena, sizeof(*node));
      binop->kind = kAST_NodeBinOp;
      binop->binop.kind = operation;
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

  if (p->idx[0].kind == kLexer_Token_Keyword) {
    switch(p->idx[0].value.kw_kind) {
      case kLexer_Keyword_Let: {
        p->idx++;
        struct AST_Node *node = Arena_Alloc(p->ast.arena, sizeof(*node));
        node->kind = kAST_NodeLet;

        struct Token *id = TryPopToken(p, kLexer_Token_Identifier);
        Assume(id != NULL, "expected identifier");
        struct Token *eq = TryPopToken(p, kLexer_Token_Symbol);
        Assume(eq != NULL && eq->value.sym_kind == kLexer_Symbol_Equal, "expected =");
        
        node->let.name = id->value.name;
        node->let.val = Parse_Expression(p);

        struct Token *end = TryPopToken(p, kLexer_Token_Symbol);
        // TODO: combine symbols and keywords into token enum, and have isbinop
        Assume(end != NULL && end->value.sym_kind == kLexer_Symbol_Semicolon, "expected ;");
        return node;
      }
      case kLexer_Keyword_Return: {
        p->idx++;
        struct AST_Node *node = Arena_Alloc(p->ast.arena, sizeof(*node));
        node->kind = kAST_NodeReturn;
        node->return_value = Parse_Expression(p);
        struct Token *end = TryPopToken(p, kLexer_Token_Symbol);
        Assume(end != NULL && end->value.sym_kind == kLexer_Symbol_Semicolon, "expected ;");
        return node;
      }
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

