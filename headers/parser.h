#ifndef __PARSER_H__
#define __PARSER_H__

enum node_type_t {
  PIPE_NODE
  ,AND_NODE
  ,COMMAND_NODE
};

struct command_t {
  char *executable;
  char **argv;
  int argc;
};

struct node_t {
  void *data;
  enum node_type_t type;
  struct node_t *right;
  struct node_t *left;
};

#endif
