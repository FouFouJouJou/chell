#ifndef __LEXER_H__
#define __LEXER_H__
#include <stddef.h>

enum token_type_t {
  TOKEN_LITERAL
  ,TOKEN_SINGLE_QUOTES_STRING
  ,TOKEN_DOUBLE_QUOTES_STRING
  ,TOKEN_OUT_TRUNC_REDIR
  ,TOKEN_OUT_APPEND_REDIR
  ,TOKEN_IN_DOUBLE_REDIR
  ,TOKEN_IN_FILE_REDIR
  ,TOKEN_PIPE
  ,TOKEN_SEMI_COLON
  ,TOKEN_AND
  ,TOKEN_UNSUPPORTED
  ,TOKEN_ENV_VAR
  ,TOKEN_EOC
};

struct token_t {
  char *literal;
  size_t len;
  enum token_type_t type;
};

void printf_token(struct token_t token);
struct token_t *lex(char *cmd, size_t len);
void free_tokens(struct token_t *tokens);
#endif
