#ifndef __PARSER_H__
#define __PARSER_H__
#define MAX_STACK_CAPACITY 20
#include <stdint.h>
#include <lexer.h>
#define OP_EVAL 0x0
#define OP_NOEVAL 0x1
#define OP_DEFAULT 0x2

enum node_type_t {
  NODE_PIPE
  ,NODE_AND
  ,NODE_SEMI_COLON
  ,NODE_CMD
  ,NODE_REDIR
  ,NODE_UNSUPPORTED
  ,NODE_ENV
};

struct node_t {
  void *data;
  enum node_type_t type;
  struct node_t *right;
  struct node_t *left;
};

struct env_t {
  char *keys[100];
  char *values[100];
  size_t size;
  struct node_t *cmd;
};

struct cmd_t {
  char *executable;
  char **argv;
  int argc;
};

struct redir_t {
  char *input_file, *output_file, *error_file;
  char *here_tag;
  int efd;
  uint8_t flags;
  struct node_t *cmd;
};

typedef void(*printf_node_func)(struct node_t node);
void printf_node(struct node_t node);
struct node_t *parse(struct token_t *tokens);
void printf_tree(struct node_t *node, int level, printf_node_func printf_func);
void free_node(struct node_t *node);
void free_tree(struct node_t *head);

#endif
