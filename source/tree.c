#include <stdlib.h>
#include <parser.h>
#include <tree.h>

struct node_t *create_empty_node() {
  struct node_t *node=calloc(1, sizeof(struct node_t));
  node->data=0;
  node->type=NODE_UNSUPPORTED;
  node->right=node->left=0;
  return node;
}

struct node_t *create_cmd_node(struct cmd_t *cmd) {
  struct node_t *node = create_empty_node();
  node->type = NODE_CMD;
  node->data=cmd;
  return node;
}
