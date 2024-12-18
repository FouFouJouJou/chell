#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <parser.h>
#include <exec.h>
#include <sys/wait.h>

int run(struct node_t *node) {
  switch(node->type) {
    case NODE_PIPE: {
      int p[2], status;
      if(pipe(p) == -1) exit(71);
      pid_t left=fork();
      if(left == 0) {
        dup2(p[1], STDOUT_FILENO);
        close(p[0]);
        close(p[1]);
        exit(run(node->left));
      }
      pid_t right=fork();
      if(right == 0) {
        dup2(p[0], STDIN_FILENO);
        close(p[0]);
        close(p[1]);
        exit(run(node->right));
      }
      waitpid(right, 0, 1);
      waitpid(left, 0, 1);
      wait(0);
      exit(0);
    }

    case NODE_CMD: {
      pid_t process=fork();
      int status;
      if(process == 0) {
        struct cmd_t *cmd=(struct cmd_t *)node->data;
        if(execvp(cmd->executable, cmd->argv) == -1) {
          printf("Nope\n");
          exit(72);
        }
      }
      wait(0);
      if(WIFEXITED(status)) {
        return(WEXITSTATUS(status));
      }
    }
  }
}
