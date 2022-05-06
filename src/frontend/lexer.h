#ifndef LEXER_H
#define LEXER_H

#include "../common/common.h"

#include <sys/types.h>
#include <stdint.h>

typedef uint8_t Lexer_TokenKind;
enum {
  kLexer_Token_Integer,
  kLexer_Token_String,
  kLexer_Token_Symbol,
  kLexer_Token_CharLiteral,
  kLexer_Token_Identifier,
  kLexer_Token_Keyword
};

typedef uint8_t Lexer_SymbolKind;
enum {
  // TODO: describe the symbol, not the function for ambigious
  // symbols (eg bitwise and vs address of)
  kLexer_Symbol_LogicalAnd,
  kLexer_Symbol_LogicalOr,
  kLexer_Symbol_LogicalNot,
  
  kLexer_Symbol_BitwiseAnd,
  kLexer_Symbol_BitwiseOr,
  kLexer_Symbol_BitwiseXor,
  kLexer_Symbol_BitwiseNot,
  kLexer_Symbol_ShiftLeft,
  kLexer_Symbol_ShiftRight,

  kLexer_Symbol_CmpEq,
  kLexer_Symbol_CmpNotEq,
  kLexer_Symbol_CmpLess,
  kLexer_Symbol_CmpLessEq,
  kLexer_Symbol_CmpGreater,
  kLexer_Symbol_CmpGreaterEq,

  kLexer_Symbol_Equal,

  kLexer_Symbol_Increment,
  kLexer_Symbol_Decrement,

  kLexer_Symbol_Plus,
  kLexer_Symbol_Minus,
  kLexer_Symbol_Multiply,
  kLexer_Symbol_Divide,

  kLexer_Symbol_ParenOpen,
  kLexer_Symbol_ParenClose,
  kLexer_Symbol_BracketOpen,
  kLexer_Symbol_BracketClose,
  kLexer_Symbol_BraceOpen,
  kLexer_Symbol_BraceClose,

  kLexer_Symbol_Dot,
  kLexer_Symbol_Comma,
  kLexer_Symbol_Semicolon
};

typedef uint8_t Lexer_KeywordKind;
enum {
  kLexer_Keyword_Let,
  kLexer_Keyword_Return,
  kLexer_Keyword_If,
};

struct Token {
  Lexer_TokenKind kind;
  size_t offset;
  size_t len;
  union {
    struct Str string;
    struct Str name;
    uint32_t   char_val;
    uint64_t   integer;
    Lexer_SymbolKind sym_kind;
    Lexer_KeywordKind kw_kind;
  } value;
};

struct TokenStream {
  struct Token *tokens;
  size_t len;
  size_t cap;
};

struct TokenStream Lexer_Tokenize(char *text, size_t len);

#endif
