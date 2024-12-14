#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lexer.h>

char *token_type_to_string(enum token_type_t type) {
  switch(type) {
    case TOKEN_STRING:
      return "TOKEN_STRING";
    case TOKEN_OUT_REDIR:
      return "TOKEN_OUT_REDIR";
    case TOKEN_IN_REDIR:
      return "TOKEN_IN_REDIR";
    case TOKEN_PIPE:
      return "TOKEN_PIPE";
    case TOKEN_SEMI_COLON:
      return "TOKEN_SEMI_COLON";
    case TOKEN_AND:
      return "TOKEN_AND";
    case TOKEN_UNSUPPORTED:
      return "TOKEN_UNSUPPORTED";
    default:
      exit(70);
  }
}

void printf_token(struct token_t token) {
  printf("TKN: (literal=%s, type=%s, len=%ld)\n", token.literal, token_type_to_string(token.type), token.len);
}

struct token_t make_token(char *literal, size_t len, enum token_type_t type) {
  return (struct token_t){
    .literal=literal
      , .len=len
      , .type=type
  };
}

size_t lex_string(char *cmd, struct token_t *token) {
  char delimiters[]=" &><|";  
  size_t len=strcspn(cmd, delimiters);
  token->literal=strndup(cmd, len);
  token->len=len;
  token->type=TOKEN_STRING;
  return len;
}

size_t output_redirection(char *cmd, size_t len, struct token_t *token) {
  return 0;
}

size_t input_redirection(char *cmd, size_t len, struct token_t *token) {
  return 0;
}

size_t lex_pipe(char *cmd, size_t len, struct token_t *token) {
  return 0;
}

struct token_t *lex(char *cmd, size_t len) {
  printf("%s(%d)\n", cmd, len);
  struct token_t *tokens=0;
  char *cmd_copy=cmd;
  while(cmd_copy < cmd+len) {
    struct token_t token={.type=TOKEN_UNSUPPORTED};
    size_t read=0;
    cmd_copy+=strspn(cmd_copy, " ");
    if((read=lex_string(cmd, &token)));
    cmd_copy+=read;
    printf_token(token);
  }
  return tokens;
}
