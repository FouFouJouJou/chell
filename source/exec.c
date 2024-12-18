#include <stdio.h>
#include <parser.h>
#include <exec.h>

void run(struct node_t *tree) {
  printf_tree(tree, 0, printf_node);
}
