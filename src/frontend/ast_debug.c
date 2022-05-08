#include "ast.h"

#include <stdint.h>
#include <stdio.h>

static void PrintIndent(size_t indent) {
  indent *= 2;
  while (indent > 0) {
    putc(' ', stdout);
    indent--;
  }
}

void AST_Print(struct AST_Node *node, size_t indent) {
  switch (node->kind) {
    case kAST_NodeInvalid:
      PrintIndent(indent);
      printf("invalid node");
      break;
    case kAST_NodeBlock:
      printf("{\n");
      for (size_t i=0; i<node->block.len; i++) {
        PrintIndent(indent + 1);
        AST_Print(node->block.nodes[i], indent + 1);
        printf(";\n");
      }
      PrintIndent(indent);
      printf("}\n");
      break;
    case kAST_NodeInt:
      printf("%zu", node->int_value);
      break;
    case kAST_NodeRef:
      printf("%.*s", (int)node->ref_name.len, node->ref_name.data);
      break;
    case kAST_NodeLet:
      printf("let %.*s = ", (int)node->let.name.len, node->let.name.data);
      AST_Print(node->let.val, indent);
      break;
    case kAST_NodeAssign:
      printf("%.*s = ", (int)node->assign.name.len, node->assign.name.data);
      AST_Print(node->assign.val, indent);
      break;
    case kAST_NodeBinOp:
      AST_Print(node->binop.lhs, indent);
      char *binop = "<binop>";
      if (node->binop.kind == kAST_BinOpAdd) {
        binop = "+";
      } else if (node->binop.kind == kAST_BinOpSub) {
        binop = "-";
      }
      printf(" %s ", binop);
      AST_Print(node->binop.rhs, indent);
      break;
    case kAST_NodeReturn:
      printf("return ");
      AST_Print(node->return_value, indent);
      break;
  }
}
