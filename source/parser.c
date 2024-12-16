#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <lexer.h>
#include <parser.h>
#define ASSERT_T(x, t) assert((x)->type == t)

void printf_cmd_node(struct command_t cmd) {
  printf("CMD: (%s, %d, [", cmd.executable, cmd.argc);
  for(int i=0; i<cmd.argc; ++i) {
    printf("%s", cmd.argv[i]);
    if(i+1 != cmd.argc) printf(", ");
  }
  printf("])\n");
}

void printf_tree(struct node_t *head, int level) {
}

size_t parse_cmd(struct token_t *tokens, struct node_t *node) {
  struct command_t *cmd=calloc(1, sizeof(struct command_t));
  cmd->argc=0;
  node->type=COMMAND_NODE;
  node->data=cmd;
  struct token_t *token=tokens;
  ASSERT_T(token, TOKEN_STRING);
  cmd->executable=token->literal;
  while(token->type != TOKEN_PIPE && token->type != TOKEN_EOC && token->type != TOKEN_AND && token->type != TOKEN_SEMI_COLON) {
    ASSERT_T(token, TOKEN_STRING);
    cmd->argc++;
    cmd->argv=realloc(cmd->argv, (cmd->argc-1)*sizeof(char*));
    cmd->argv[cmd->argc-1]=token->literal;
    token++;
  } 
  return token-tokens;
}

size_t parse_pipe(struct token_t *tokens, struct node_t *node) {
  return 0;
}

struct node_t *build_tree(struct token_t *tokens) {
  struct node_t *head=calloc(1, sizeof(struct node_t));
  struct token_t *token=tokens;
  while(token->type != TOKEN_EOC) {
    if(token->type == TOKEN_STRING) {
      size_t read=parse_cmd(token, head);
      printf_cmd_node(*((struct command_t*)head->data));
      token+=read;
    }
    if(token->type == TOKEN_PIPE) {
      printf("PIPE\n");
      token++;
    }
    if(token->type == TOKEN_AND) {
      printf("AND\n");
      token++;
    }
    if(token->type == TOKEN_SEMI_COLON) {
      printf("SEMI_COLON\n");
      token++;
    }
  }
  return head;
}

void run(struct node_t *head) {
}

struct node_t *parse(char *line, size_t len) {
  struct token_t *tokens=lex(line, len);
  struct node_t *head=build_tree(tokens);
  return head;
}
