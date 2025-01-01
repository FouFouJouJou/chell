#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lexer.h>

char *token_type_to_string(enum token_type_t type) {
  switch(type) {
    case TOKEN_ENV_VAR:
      return "TOKEN_ENV_VAR";
    case TOKEN_LITERAL:
      return "TOKEN_LITERAL";
    case TOKEN_SINGLE_QUOTES_STRING:
      return "TOKEN_SINGLE_QUOTES_STRING";
    case TOKEN_DOUBLE_QUOTES_STRING:
      return "TOKEN_DOUBLE_QUOTES_STRING";
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
    case TOKEN_ERROR_REDIR:
      return "TOKEN_ERROR_REDIR";
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
  if(*cmd == '\'' || *cmd == '"') {
    char quotes=*cmd++;
    size_t len=0;
    // TODO: for some reason strcspn is chaotic (DEFINITELY GDB THIS LATER)
    while(*cmd != quotes) {len++; cmd++;}
    //size_t len=strcspn(cmd+len, &quotes);
    token->literal=strndup(cmd-len, len);
    token->type=quotes=='\'' ? TOKEN_SINGLE_QUOTES_STRING : TOKEN_DOUBLE_QUOTES_STRING;
    token->len=len;
    return len+2;
  }
  else {
    // TODO: " " bug for options of kind option=" "
    char delimiters[]=" &><|;\n";
    size_t len=strcspn(cmd, delimiters);
    token->literal=strndup(cmd, len);
    token->len=len;
    token->type=TOKEN_LITERAL;
    return len;
  }
}

size_t lex_error_redirection(char *cmd, struct token_t *token) {
  if(!strncmp(cmd, "2>", 2)) {
    size_t len=strcspn(cmd+2, " \n");
    char *redirection_stream=calloc(1, len*sizeof(char));
    strncpy(redirection_stream, cmd+2, len);
    token->literal=redirection_stream;
    token->type=TOKEN_ERROR_REDIR;
    token->len=len;
    return token->len+2;
  }
  return 0;
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
  struct token_t *tokens=0;
  size_t idx=0;
  char *cmd_copy=cmd;
  while(cmd_copy < cmd+len) {
    size_t read=0;
    struct token_t token={.type=TOKEN_UNSUPPORTED};
    cmd_copy+=strspn(cmd_copy, " \t\r\n");
    if(cmd_copy == cmd+len) break;
    else if((read=lex_delimiter(cmd_copy, &token)));
    else if((read=lex_error_redirection(cmd_copy, &token)));
    else if((read=lex_output_redirection(cmd_copy, &token)));
    else if((read=lex_input_redirection(cmd_copy, &token)));
    else if((read=lex_string(cmd_copy, &token)));
    if(token.type == TOKEN_UNSUPPORTED) {
      exit(71);
    }
    idx++;
    tokens=realloc(tokens, idx*sizeof(struct token_t));
    tokens[idx-1]=token;
    cmd_copy+=read;
    printf_token(token);
  }
  idx++;
  tokens=realloc(tokens, idx*sizeof(struct token_t));
  tokens[idx-1].type=TOKEN_EOC;
  return tokens;
}
