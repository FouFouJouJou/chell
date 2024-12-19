#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lexer.h>

char *token_type_to_string(enum token_type_t type) {
  switch(type) {
    case TOKEN_STRING:
      return "TOKEN_STRING";
    case TOKEN_OUT_APPEND_REDIR:
      return "TOKEN_OUT_APPEND_REDIR";
    case TOKEN_OUT_TRUNC_REDIR:
      return "TOKEN_OUT_TRUNC_REDIR";
    case TOKEN_IN_DOUBLE_REDIR:
      return "TOKEN_IN_DOUBLE_REDIR";
    case TOKEN_IN_FILE_REDIR:
      return "TOKEN_IN_FILE_REDIR";
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

size_t lex_string(char *cmd, struct token_t *token) {
  // TODO: double or single quote case
  char delimiters[]=" &><|;";  
  size_t len=strcspn(cmd, delimiters);
  token->literal=strndup(cmd, len);
  token->len=len;
  token->type=TOKEN_STRING;
  return len;
}

size_t lex_output_redirection(char *cmd, struct token_t *token) {
  if(!strncmp(cmd, ">>", 2)) {
    token->type=TOKEN_OUT_APPEND_REDIR;
    token->len=2;
    token->literal=strndup(cmd, 2);
    return token->len;
  }
  if(!strncmp(cmd, ">", 1)) {
    token->type=TOKEN_OUT_TRUNC_REDIR;
    token->len=1;
    token->literal=strndup(cmd, 1);
    return token->len;
  }
  return 0;
}

size_t lex_input_redirection(char *cmd, struct token_t *token) {
  if(!strncmp(cmd, "<<", 2)) {
    token->type=TOKEN_IN_DOUBLE_REDIR;
    token->len=2;
    token->literal=strndup(cmd, 2);
    return token->len;
  }
  if(!strncmp(cmd, "<", 1)) {
    token->type=TOKEN_IN_FILE_REDIR;
    token->len=1;
    token->literal=strndup(cmd, 1);
    return token->len;
  }
  return 0;
}

size_t lex_delimiter(char *cmd, struct token_t *token) {
  if(!strncmp(cmd, "&&", 2)) {
    token->type=TOKEN_AND;
    token->len=2;
    token->literal=strndup(cmd, 2);
    return token->len;
  }
  if(!strncmp(cmd, "|", 1)) {
    token->type=TOKEN_PIPE;
    token->len=1;
    token->literal=strndup(cmd, 1);
    return token->len;
  }
  if(!strncmp(cmd, ";", 1)) {
    token->type=TOKEN_SEMI_COLON;
    token->len=1;
    token->literal=strndup(cmd, 1);
    return token->len;
  }
  return 0;
}

struct token_t *lex(char *cmd, size_t len) {
  printf("%s(%d)\n", cmd, len);
  struct token_t *tokens=0;
  size_t idx=0;
  char *cmd_copy=cmd;
  while(cmd_copy < cmd+len) {
    size_t read=0;
    struct token_t token={.type=TOKEN_UNSUPPORTED};
    cmd_copy+=strspn(cmd_copy, " ");
    if((read=lex_string(cmd_copy, &token)));
    else if((read=lex_output_redirection(cmd_copy, &token)));
    else if((read=lex_input_redirection(cmd_copy, &token)));
    else if((read=lex_delimiter(cmd_copy, &token)));
    if(token.type == TOKEN_UNSUPPORTED) {
      exit(71);
    }
    idx++;
    tokens=realloc(tokens, idx*sizeof(struct token_t));
    tokens[idx-1]=token;
    cmd_copy+=read;
  }
  idx++;
  tokens=realloc(tokens, idx*sizeof(struct token_t));
  tokens[idx-1].type=TOKEN_EOC;
  return tokens;
}
