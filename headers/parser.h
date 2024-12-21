#ifndef __PARSER_H__
#define __PARSER_H__
#define MAX_STACK_CAPACITY 20
#include <lexer.h>

enum node_type_t {
   NODE_PIPE
  ,NODE_AND
  ,NODE_SEMI_COLON
  ,NODE_CMD
  ,NODE_OUT_TRUNC_REDIR
  ,NODE_UNSUPPORTED
};

struct node_t {
  void *data;
  enum node_type_t type;
  struct node_t *right;
  struct node_t *left;
};

struct cmd_t {
  char *executable;
  char **argv;
  int argc;
};

struct redir_t {
  char *input_file, *output_file, *error_file;
  // TODO: refactoring plan for redirection flags
  // uint8_t input_mode, output_mode, error_mode;
  struct node_t *cmd;
};

typedef void(*printf_node_func)(struct node_t node);
void printf_node(struct node_t node);
struct node_t *parse(struct token_t *tokens);
void printf_tree(struct node_t *node, int level, printf_node_func printf_func);
void free_node(struct node_t *node);
void free_tree(struct node_t *head);

#endif
