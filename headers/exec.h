#ifndef __EXEC_H__
#define __EXEC_H__
#include <parser.h>

int run(struct node_t *tree);
int run_cmd(char *cmd, size_t len);
#endif
