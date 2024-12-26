#ifndef __BUILTIN_H
#define __BUILTIN_H
#include <stdbool.h>

typedef int(*run_builtin_func)(struct cmd_t cmd);
struct builtin_t {
  char cmd[100];
  run_builtin_func run;
};

bool is_builtin(char *cmd);
int run_builtin(struct cmd_t cmd);

#endif

