#ifndef __PARSER_H__
#define __PARSER_H__
#include <tree.h>

struct cmd_t {
  char *executable;
  char **argv;
  int argc;
};

void printf_tree(struct node_t *head, int level);
void run(struct node_t *head);
struct node_t *parse(char *line, size_t len);
#endif
