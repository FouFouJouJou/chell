#ifndef __TREE_H__
#define __TREE_H__

enum node_type_t {
   NODE_PIPE
  ,NODE_AND
  ,NODE_SEMI_COLON
  ,NODE_REDIR
  ,NODE_CMD
};

struct node_t {
  void *data;
  enum node_type_t type;
  struct node_t *right;
  struct node_t *left;
};

#endif
