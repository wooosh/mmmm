#ifndef TOKEN_H
#define TOKEN_H

#include "../common/common.h"

#include <stdint.h>
#include <sys/types.h>

typedef uint8_t Token_Kind;
enum {
  kToken_Integer,
  kToken_String,
  kToken_CharLiteral,
  kToken_Identifier,

  /* keywords */
  kToken_Keyword_Let,
  kToken_Keyword_If,
  kToken_Keyword_Return,

  /* symbols */
  kToken_Symbol_Plus,
  kToken_Symbol_Minus,
  kToken_Symbol_Comma,
  kToken_Symbol_Equal,
  kToken_Symbol_Semicolon
};

struct Token {
  Token_Kind kind;
  size_t offset;
  size_t len;
  union {
    struct Str string;
    struct Str name;
    uint32_t   char_val;
    uint64_t   integer;
  } value;
};

#endif
