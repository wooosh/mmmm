#include "../common/common.h"
#include "passes.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct Lexer {
  char *start;
  char *idx;
  char *end;
};

/* any of these functions will return true if they advance idx */
static bool Lexer_SkipSpace(struct Lexer *l);
static bool Lexer_SkipComment(struct Lexer *l);
static bool Lexer_TrySymbol(struct Lexer*, struct Token*);
static bool Lexer_TryCharLiteral(struct Lexer*, struct Token*);
static bool Lexer_TryString(struct Lexer*, struct Token*);
static bool Lexer_TryNumber(struct Lexer*, struct Token*);
static bool Lexer_TryKeyword(struct Lexer*, struct Token*);
static bool Lexer_TryIdentifier(struct Lexer*, struct Token*);

struct TokenStream Lexer_Tokenize(char *txt, size_t len) {
  struct Lexer l = {
    .start = txt,
    .idx   = txt,
    .end   = txt + len,
  };

  struct TokenStream out = {
    .tokens = NULL,
    .len    = 0,
    .cap    = 0
  };

  while (l.idx < l.end) {
    if (Lexer_SkipSpace(&l) || Lexer_SkipComment(&l)) continue;

    struct Token token;
    if (Lexer_TrySymbol(&l, &token)
      || Lexer_TryCharLiteral(&l, &token)
      || Lexer_TryString(&l, &token)
      || Lexer_TryNumber(&l, &token)
      || Lexer_TryKeyword(&l, &token)
      || Lexer_TryIdentifier(&l, &token)) {
      out.len++;
      EnsureLen(&out.tokens, &out.cap, out.len);
      out.tokens[out.len-1] = token;
    } else {
      l.idx++;
    }
  }

  return out;
}

static bool Lexer_SkipSpace(struct Lexer *l) {
  char *original_idx = l->idx;
  while (l->idx != l->end) {
    char c = *l->idx;
    if (!(c == ' ' || c == '\n' || c == '\t')) {
      return original_idx != l->idx;
    }
    l->idx++;
  }

  return false;
}

static bool Lexer_SkipComment(struct Lexer *l) {
  if (l->idx - l->start < 2) return false;

  if (memcmp(l->idx, "//", 2) == 0) {
    char *end_of_line = memchr(l->idx, '\n', l->idx - l->start);

    if (end_of_line != NULL) l->idx = end_of_line;
    else l->idx = l->end;

    return true;
  }

  if (memcmp(l->idx, "/*", 2) == 0) {
    while (l->end - l->idx >= 2) {
      if (memcmp(l->idx, "*/", 2) == 0) {
        l->idx += 2; // account for the */ length
        return true;
      }
      l->idx++;
    }
    // TODO: better error handling based on index and filename
    Die("Expected closing */ for /*");
  }

  return false;
}

static bool Lexer_TrySymbol(struct Lexer *l, struct Token *token) {
  static struct Lexer_SymbolRule {
    char *symbol;
    Token_Kind kind;
  } symbol_rules[] = {
    /* sorted so that symbols sharing a prefix have the longest symbol first */
    {"+", kToken_Symbol_Plus},
    {"-", kToken_Symbol_Minus},
    {";", kToken_Symbol_Semicolon},
    {"=", kToken_Symbol_Equal},
    {",", kToken_Symbol_Comma},
    {NULL, 0},
  };
  size_t len = 2;
  if ((size_t)(l->end - l->idx) < len) {
    len = l->end - l->idx;
  }
  if (len == 0) return false;

  for (size_t i=0; symbol_rules[i].symbol != 0; i++) {
    size_t symbol_len = strlen(symbol_rules[i].symbol);
    if (memcmp(l->idx, symbol_rules[i].symbol, MinUint(symbol_len, len)) == 0) {
      token->kind = symbol_rules[i].kind;
      token->offset = l->idx - l->start;
      token->len = symbol_len;

      l->idx += token->len;
      return true;
    }
  }

  return false;
}

static bool Lexer_TryCharLiteral(struct Lexer *l, struct Token *token) {
  // TODO: handle escapes
  // TODO: probably broken
  size_t len = l->end - l->start; 
  if (len >= 1 && *l->idx == '\'') {
    char *closing_quote = memchr(l->idx + 1, '\'', len-1);
    if (closing_quote == NULL || closing_quote - l->idx != 2) {
      Die("malformed character literal");
    }
    token->kind = kToken_CharLiteral;
    token->value.char_val = l->idx[1];
    token->offset = l->idx - l->start;
    token->len = closing_quote - l->idx + 1;
    l->idx = closing_quote + 1;
    return true;
  }

  return false;
}

static bool Lexer_TryString(struct Lexer *l, struct Token *token) {
  // TODO: handle escapes
  // TODO: probably broken
  size_t len = l->end - l->start; 
  if (len >= 1 && *l->idx == '"') {
    char *closing_quote = memchr(l->idx + 1, '"', len-1);
    if (closing_quote == NULL) {
      Die("malformed string");
    }
    token->kind = kToken_String;
    // TODO: duplicate string
    token->value.string.data = l->idx + 1;
    token->value.string.len  = closing_quote - l->idx;
    token->offset = l->idx - l->start;
    token->len = closing_quote - l->idx + 1;
    l->idx = closing_quote + 1;
    return true;
  }

  return false;
}

static bool Lexer_TryNumber(struct Lexer *l, struct Token *token) {
  // TODO: only works for integers, add float support
  // TODO: support for non decimal bases
  char *tail = l->idx;
  while (tail != l->end && *tail >= '0' && *tail <= '9') tail++;

  if (l->idx == tail) return false;

  // TODO: use a bounded str to int function that does not require a null
  // terminator
  errno = 0;
  token->value.integer = strtoull(l->idx, NULL, 10);
  if (errno == ERANGE) {
    Die("integer out of range");
  }

  token->kind   = kToken_Integer;
  token->offset = l->idx - l->start;
  token->len    = tail - l->idx;
  l->idx = tail;
  return true;
}



static bool Lexer_TryKeyword(struct Lexer *l, struct Token *token) {
  static struct KeywordRule {
    char *keyword;
    Token_Kind kind;
  } keyword_rules[] = {
    {"if", kToken_Keyword_If},
    {"return", kToken_Keyword_Return},
    {"let", kToken_Keyword_Let},
    {NULL, 0}
  };

  size_t len = 0;
  while (l->idx + len != l->end && l->idx[len] >= 'a' && l->idx[len] <= 'z') len++;
  if (len == 0) return false;

  for (size_t i = 0; keyword_rules[i].keyword != NULL; i++) {
    if (strlen(keyword_rules[i].keyword) == len
        && strncmp(l->idx, keyword_rules[i].keyword, len) == 0) {
      token->kind = keyword_rules[i].kind;
      token->offset = l->idx - l->start;
      token->len = len;
      l->idx += len;
      return true;
    }
  }

  return false;
}

static bool Lexer_TryIdentifier(struct Lexer *l, struct Token *token) {
  char *tail = l->idx;

  if (l->end - tail < 1
      || !((*tail >= 'a' && *tail <= 'z') || (*tail >= 'A' && *tail <= 'Z'))) {
    return false;
  }

  while (tail != l->end
       && ((*tail >= 'a' && *tail <= 'z') || (*tail >= 'A' && *tail <= 'Z')
          || (*tail >= '0' && *tail <= '9'))) tail++;

  token->kind   = kToken_Identifier;
  token->offset = l->idx - l->start;
  token->len    = tail - l->idx;
  token->value.name.data = l->idx;
  token->value.name.len  = token->len;
  l->idx = tail;
  return true;
}
