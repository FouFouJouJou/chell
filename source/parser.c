#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <tree.h>
#include <lexer.h>
#include <parser.h>
#define ASSERT_T(x, t) assert((x)->type == t)

struct node_t *create_empty_node() {
  struct node_t *node=calloc(1, sizeof(struct node_t));
  node->data=0;
  node->type=NODE_UNSUPPORTED;
  node->right=node->left=0;
  return node;
}

struct node_t *create_cmd_node(struct cmd_t *cmd) {
  struct node_t *node = calloc(1, sizeof(struct node_t));
  node->type = NODE_CMD;
  node->data=cmd;
  return node;
}

void push_head(struct node_t *node, struct node_t **head) {
  if(*head == 0) {
    *head=node;
    return;
  }
  node->left=*head;
  *head=node;
}

void printf_tree(struct node_t *node, int level, printf_node_func printf_func) {
  if(node == 0) {
    return;
  }
  for(int i=0; i<level; ++i) printf("  ");
  printf_func(*node);
  printf_tree(node->left, level+1, printf_func);
  printf_tree(node->right, level+1, printf_func);
}

void printf_cmd_node(struct node_t node) {
  struct cmd_t cmd=*(struct cmd_t *)node.data;
  printf("CMD: (%s, %d, [", cmd.executable, cmd.argc);
  for(int i=0; i<cmd.argc; ++i) {
    printf("%s", cmd.argv[i]);
    if(i+1 != cmd.argc) printf(", ");
  }
  printf("])\n");
}

void printf_node(struct node_t node) {
  switch(node.type) {
    case NODE_PIPE:
      printf("|\n");
      break;
    case NODE_AND:
      printf("&&\n");
      break;
    case NODE_SEMI_COLON:
      printf(";\n");
      break;
    case NODE_REDIR:
      break;
    case NODE_CMD:
      printf_cmd_node(node);
      break;
    case NODE_UNSUPPORTED:
      break;
    default:
      printf("nope\n");
      break;
  }
}

struct node_t *create_pipe_node(struct node_t *left, struct node_t *right) {
  struct node_t *pipe_node=calloc(1, sizeof(struct node_t));
  pipe_node->type=NODE_PIPE;
  pipe_node->right=right;
  pipe_node->left=left;
  return pipe_node;
}

size_t parse_cmd(struct token_t *tokens, struct node_t *node) {
  struct cmd_t *cmd=calloc(1, sizeof(struct cmd_t));
  cmd->argc=0;
  node->type=NODE_CMD;
  node->data=cmd;
  struct token_t *token=tokens;
  ASSERT_T(token, TOKEN_STRING);
  cmd->executable=token->literal;
  while(token->type != TOKEN_PIPE 
    && token->type != TOKEN_EOC 
    && token->type != TOKEN_AND 
    && token->type != TOKEN_SEMI_COLON
  ) {
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
  struct node_t *head=0;
  struct token_t *token=tokens;
  int stack_idx=0;
  struct node_t *stack[20];
  while(token->type != TOKEN_EOC) {
    if(token->type == TOKEN_STRING) {
      struct node_t *node=calloc(1, sizeof(struct node_t));
      size_t read=parse_cmd(token, node);
      stack[stack_idx++]=node;
      assert(stack_idx==1);
      token+=read;
    }

    if(token->type == TOKEN_PIPE) {
      token++;
      struct node_t *right_cmd=calloc(1, sizeof(struct node_t));
      size_t read=parse_cmd(token, right_cmd);
      struct node_t *left_cmd=stack[stack_idx-1];
      struct node_t *pipe_node=create_pipe_node(left_cmd, right_cmd);
      push_head(pipe_node, &head);
      stack[stack_idx]=pipe_node;
      token+=read;
    }
  }
  return head;
}

void run(struct node_t *head) {
}

struct node_t *parse(char *line, size_t len) {
  struct token_t *tokens=lex(line, len);
  struct node_t *head=build_tree(tokens);
  printf_tree(head, 0, printf_node);
  return head;
}
