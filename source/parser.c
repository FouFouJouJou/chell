#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <lexer.h>
#include <parser.h>
#define ASSERT_T(x, t) assert((x)->type == t)

struct node_t *create_delim_node(struct node_t *left, struct node_t *right, enum node_type_t type) {
  struct node_t *node=calloc(1, sizeof(struct node_t));
  node->type=type;
  node->right=right;
  node->left=left;
  return node;
}

void printf_tree(struct node_t *node, int level, printf_node_func printf_func) {
  if(node == 0) {
    return;
  }
  for(int i=0; i<level; ++i) printf("  ");
  printf_node(*node);
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

void printf_out_trunc_redir_node(struct node_t node) {
  struct redir_t *redir=(struct redir_t*)node.data;
  printf("> %s\n", redir->output_file);
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
    case NODE_CMD:
      printf_cmd_node(node);
      break;
    case NODE_UNSUPPORTED:
      break;
    case NODE_OUT_TRUNC_REDIR:
      printf_out_trunc_redir_node(node);
      break;
    default:
      printf("nope\n");
      break;
  }
}

size_t parse_cmd(struct token_t *tokens, struct node_t *node) {
  struct cmd_t *cmd=calloc(1, sizeof(struct cmd_t));
  struct redir_t *redir=0;
  struct token_t *token=tokens;
  ASSERT_T(token, TOKEN_STRING);
  cmd->executable=token->literal;
  while(token->type != TOKEN_PIPE 
    && token->type != TOKEN_EOC 
    && token->type != TOKEN_AND 
    && token->type != TOKEN_SEMI_COLON
  ) {
    if(token->type == TOKEN_STRING) {
      cmd->argc++;
      cmd->argv=realloc(cmd->argv, (cmd->argc)*sizeof(char*));
      cmd->argv[cmd->argc-1]=token->literal;
      token++;
    }
    if(token->type == TOKEN_OUT_TRUNC_REDIR) {
      token++;
      ASSERT_T(token, TOKEN_STRING);
      redir=calloc(1, sizeof(struct redir_t));
      redir->output_file=token->literal;
      token++;
    }
  }

  cmd->argc++;
  cmd->argv=realloc(cmd->argv, (cmd->argc)*sizeof(char*));
  cmd->argv[cmd->argc-1]=0;

  if(redir != 0) {
    struct node_t *cmd_node=calloc(1, sizeof(struct node_t));
    cmd_node->data=cmd;
    cmd_node->type=NODE_CMD;
    cmd_node->right=cmd_node->left=0;
    node->type=NODE_OUT_TRUNC_REDIR;
    node->data=redir;
    redir->cmd=cmd_node;
    node->right=node->left=0;
  } else {
    node->type=NODE_CMD;
    node->data=cmd;
    node->right=node->left=0;
  }
  return token-tokens;
}

struct node_t *build_tree(struct token_t *tokens) {
  struct token_t *token=tokens;
  int stack_idx=0;
  struct node_t *stack[MAX_STACK_CAPACITY]={0};
  while(token->type != TOKEN_EOC) {
    switch(token->type) {
      case TOKEN_STRING: {
        struct node_t *node=calloc(1, sizeof(struct node_t));
        size_t read=parse_cmd(token, node);
        stack[stack_idx++]=node;
        token+=read;
        break;
      }
      case TOKEN_PIPE:
      case TOKEN_AND:
      case TOKEN_SEMI_COLON: {
        token++;
        struct node_t *right_cmd=calloc(1, sizeof(struct node_t));
        size_t read=parse_cmd(token, right_cmd);
        struct node_t *left_cmd=stack[stack_idx-1];
        stack_idx--;
        struct node_t *node=create_delim_node(left_cmd, right_cmd, token->type);
        stack[stack_idx++]=node;
        token+=read;
        break;
      }
    }
    assert(stack_idx == 1);
  }
  return stack[0];
}

void free_node(struct node_t *node) {
  switch(node->type) {
    case NODE_CMD: {
      struct cmd_t *cmd=(struct cmd_t *)node->data;
      free(cmd->executable);
      // argc => 
      // argv => {"ls", "-l", 0}
      for(int i=1; i<cmd->argc-1; ++i) {
        free(cmd->argv[i]);
      }
      free(cmd->argv);
      free(cmd);
      break;
    }
    case NODE_OUT_TRUNC_REDIR: {
      struct redir_t *redir=(struct redir_t *)node->data;
      if(redir->input_file)
        free(redir->input_file);
      if(redir->output_file)
        free(redir->output_file);
      if(redir->error_file)
        free(redir->error_file);
      free(redir);
      free_node(redir->cmd);
      break;
    }
  }
  free(node);
}

void free_tree(struct node_t *head) {
  if(head == 0) return;
  free_tree(head->left);
  free_tree(head->right);
  free_node(head);
}

struct node_t *parse(struct token_t *tokens) {
  return build_tree(tokens);
}
