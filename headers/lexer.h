#ifndef __LEXER_H__
#define __LEXER_H__

enum token_type_t {
  TOKEN_STRING 
  ,TOKEN_OUT_REDIR
  ,TOKEN_IN_REDIR
  ,TOKEN_PIPE
  ,TOKEN_SEMI_COLON
  ,TOKEN_AND
  ,TOKEN_UNSUPPORTED
};

struct token_t {
  char *literal;
  size_t len;
  enum token_type_t type;
};

struct token_t *lex(char *cmd, size_t len);
#endif
