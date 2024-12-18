#ifndef __PARSER_H__
#define __PARSER_H__
#define MAX_STACK_CAPACITY 20

enum node_type_t {
   NODE_PIPE
  ,NODE_AND
  ,NODE_SEMI_COLON
  ,NODE_REDIR
  ,NODE_CMD
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

struct pipe_t {
  int pipe[2];
};

typedef void(*printf_node_func)(struct node_t node);
void printf_node(struct node_t node);
struct node_t *parse(char *line, size_t len);

void push_head(struct node_t *node, struct node_t **head);
void printf_tree(struct node_t *node, int level, printf_node_func printf_func);

#endif
