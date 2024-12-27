#ifndef __BUILTIN_H
#define __BUILTIN_H
#include <stdbool.h>
#define TOTAL_BUILTINS 3

typedef int(*exec_builtin_func)(struct cmd_t cmd);
struct builtin_t {
  char cmd[100];
  exec_builtin_func run;
};

static int exec_cd(struct cmd_t);
static int exec_history(struct cmd_t cmd);
static int exec_exit(struct cmd_t cmd);

bool is_builtin(char *cmd);
int exec_builtin(struct cmd_t cmd);

#endif

