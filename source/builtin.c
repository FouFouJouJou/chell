#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <parser.h>
#include <history.h>
#include <builtin.h>

struct builtin_t builtins[TOTAL_BUILTINS] = {
  { .cmd="cd", .run=exec_cd }
  ,{ .cmd="history", .run=exec_history }
  ,{ .cmd="exit", .run=exec_exit }
};

bool is_builtin(char *cmd) {
  for(int i=0; i< sizeof(builtins)/sizeof(struct builtin_t); ++i) {
    if(!strncmp(builtins[i].cmd, cmd, strlen(builtins[i].cmd))) {
      return true;
    }
  }
  return false;
}

int exec_builtin(struct cmd_t cmd) {
  for(int i=0; i< sizeof(builtins)/sizeof(struct builtin_t); ++i) {
    if(!strncmp(builtins[i].cmd, cmd.executable, strlen(builtins[i].cmd))) {
      return builtins[i].run(cmd);
    }
  }
  return -1;
}

int exec_cd(struct cmd_t cmd) {
  return chdir(cmd.argv[1]);
}

int exec_history(struct cmd_t cmd) {
  log_history();
  return 0;
}

int exec_exit(struct cmd_t cmd) {
  exit(save_history_to_fs(0));
}
