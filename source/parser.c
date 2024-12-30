#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <lexer.h>
#include <parser.h>
#define ASSERT_T(x, t) assert((x)->type == t)

char *eval_(char *literal, size_t len) {
  char result[2048]={0};
  size_t size=0;
  char *character=literal;
  while(character < literal+len) {
    if(*character == '$') {
      character++;
      size_t len_=0;
      while((*character >= 'A' && *character <= 'Z') || (*character >= 'a' && *character <= 'z')) {
        len_++;
        character++;
      }
      char var_name[100]={0};
      strncpy(var_name, character-len_, len_);
      char *env_value=getenv(var_name);
      if(env_value != 0) {
        size_t env_value_len=strlen(env_value);
        size+=env_value_len;
        strncpy(result+size-env_value_len, env_value, env_value_len);
        result[size]='\0';
      }
    } else {
      result[size++]=*character;
      character++;
    }
  }
  return strndup(result, size);
}

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

void printf_redir_node(struct node_t node) {
  struct redir_t *redir=(struct redir_t*)node.data;
  if(redir->output_file) {
    char *out_symbol=redir->flags & 0x01 ? ">>" : ">";
    printf("%d %s %s\n", redir->flags & 0x01, out_symbol, redir->output_file);
  }

  if(redir->here_tag) {
    char *in_symbol=((redir->flags & 0x10)>>4) ? "<<" : "<";
    printf("%d %s %s\n", (redir->flags & 0x10)>>4, in_symbol, redir->here_tag);
  }
}

void printf_env_node(struct node_t node) {
  printf("ENV: [");
  struct env_t *env=(struct env_t*)node.data;
  for(int i=0; i<env->size; ++i) {
    printf("%s=%s", env->keys[i], env->values[i]);
    if(i+1 != env->size) printf(", ");
  }
  printf("]\n");
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
    case NODE_REDIR:
      printf_redir_node(node);
      break;
    case NODE_ENV:
      printf_env_node(node);
      break;
    case NODE_UNSUPPORTED:
      exit(73);
    default:
      exit(74);
  }
}

size_t parse_cmd(struct token_t *tokens, struct node_t *node) {
  struct cmd_t *cmd=calloc(1, sizeof(struct cmd_t));
  struct redir_t *redir=0;
  struct token_t *token=tokens;
  assert(token->type == TOKEN_LITERAL);
  cmd->executable=token->literal;
  while(token->type != TOKEN_PIPE 
    && token->type != TOKEN_EOC 
    && token->type != TOKEN_AND 
    && token->type != TOKEN_SEMI_COLON
  ) {
    switch(token->type) {
      case TOKEN_SINGLE_QUOTES_STRING:
      case TOKEN_DOUBLE_QUOTES_STRING:
      case TOKEN_LITERAL: {
        cmd->argc++;
        cmd->argv=realloc(cmd->argv, (cmd->argc)*sizeof(char*));
        if(token->type != TOKEN_SINGLE_QUOTES_STRING && token-tokens > 0) {
          cmd->argv[cmd->argc-1]=eval_(token->literal, token->len);
          free(token->literal);
        }
        else 
          cmd->argv[cmd->argc-1]=token->literal;
        token++;
        break;
      }
      case TOKEN_IN_FILE_REDIR:
      case TOKEN_IN_DOUBLE_REDIR: {
        ASSERT_T(token+1, TOKEN_LITERAL);
        if(redir==0) {
          redir=calloc(1, sizeof(struct redir_t));
          redir->flags=0;
        }
        if(token->type == TOKEN_IN_DOUBLE_REDIR) {
          token++;
          redir->here_tag=token->literal;
          redir->flags|=0x10;
        }
        else {
          token++;
          redir->input_file=token->literal;
          redir->flags|=0x00;
        }
        token++;
        break;
      }

      case TOKEN_OUT_TRUNC_REDIR:
      case TOKEN_OUT_APPEND_REDIR: {
        ASSERT_T(token+1, TOKEN_LITERAL);
        if(redir==0) {
          redir=calloc(1, sizeof(struct redir_t));
          redir->flags=0;
        }
        if(token->type == TOKEN_OUT_APPEND_REDIR) {
          redir->flags|=0x01;
        }
        else {
          redir->flags|=0x00;
        }

        token++;
        redir->output_file=token->literal;
        token++;
        break;
      }
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
    redir->cmd=cmd_node;
    node->type=NODE_REDIR;
    node->data=redir;
    node->right=node->left=0;
  } else {
    node->type=NODE_CMD;
    node->data=cmd;
    node->right=node->left=0;
  }
  return token-tokens;
}

size_t parse_env(struct token_t *token, struct node_t *node) {
  struct env_t *env=(struct env_t *)node->data;
  char *literal=token->literal;
  size_t key_len=strcspn(literal, "=");
  env->keys[env->size]=calloc(key_len, sizeof(char));
  strncpy(env->keys[env->size], token->literal, key_len);
  literal+=key_len;
  env->values[env->size]=calloc(literal-token->literal, sizeof(char));
  strncpy(env->values[env->size], literal+1, literal-token->literal);
  env->size++;
  return 1;
}

struct node_t *build_tree(struct token_t *tokens) {
  struct token_t *token=tokens;
  int stack_idx=0;
  struct node_t *stack[MAX_STACK_CAPACITY]={0};
  struct node_t *env_node=0;
  while(token->type != TOKEN_EOC) {
    size_t read=0;
    switch(token->type) {
      case TOKEN_LITERAL: {
        if(token->literal[0] != '$' && strchr(token->literal, '=') != 0) {
          if(env_node == 0) {
            env_node=calloc(1, sizeof(struct node_t *));
            struct env_t *env=calloc(1, sizeof(struct env_t));
            env->cmd=0;
            env->size=0;
            env_node->data=env;
            env_node->left=env_node->right=0;
            env_node->type=NODE_ENV;
            stack[stack_idx++]=env_node;
          }
          assert(stack_idx == 1);
          token+=parse_env(token, stack[stack_idx-1]);
          break;
        }

        struct node_t *node=calloc(1, sizeof(struct node_t *));
        read=parse_cmd(token, node);
        if(stack_idx > 0 && stack[stack_idx-1]->type == NODE_ENV) {
          struct env_t *env=(struct env_t *)node->data;
          env->cmd=node;
          env_node=0;
        } else {
          stack[stack_idx++]=node;
        }
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
      for(int i=1; i<cmd->argc-1; ++i) {
        free(cmd->argv[i]);
      }
      free(cmd->argv);
      free(cmd);
      break;
    }
    case NODE_REDIR: {
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
