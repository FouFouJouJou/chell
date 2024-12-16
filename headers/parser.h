#ifndef __PARSER_H__
#define __PARSER_H__
#include <tree.h>

enum option_type_t {
  OPTION_VARIABLE
  ,OPTION_FLAG
  ,OPTION_COMMAND
};

struct tree_t {
  struct node_t *head;
};

struct option_t {
  enum option_type_t type; 
  struct node_t *node;
};

struct cmd_t {
  char *executable;
  char **argv;
  int argc;
};

void printf_tree(struct node_t *head, int level);
void run(struct node_t *head);
struct node_t *parse(char *line, size_t len);
#endif
