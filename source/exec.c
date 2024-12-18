#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <parser.h>
#include <exec.h>
#include <sys/wait.h>

void run(struct node_t *node) {
  switch(node->type) {
    case NODE_PIPE: {
      int p[2];
      if(pipe(p) == -1) exit(71);
      int status_left, status_right;
      pid_t left_process=fork();
      if(left_process == 0) {
        if(dup2(p[1], STDOUT_FILENO) == -1) exit(71);
        run(node->left);
        return;
      }


      pid_t right_process=fork();
      if(right_process == 0) {
        if(dup2(p[0], STDIN_FILENO) == -1) exit(71);
        run(node->right);
        return;
      }
      waitpid(left_process, &status_left, 1);
      waitpid(right_process, &status_right, 1);
      break;
    }
    case NODE_AND:
      break;
    case NODE_SEMI_COLON: {
      pid_t left_process=fork();
      int status_left, status_right;
      if(left_process == 0) {
        run(node->left);
        return;
      }
      waitpid(left_process, &status_left, 0);
      pid_t right_process=fork();
      if(right_process == 0) {
        run(node->right);
        return;
      }
      waitpid(right_process, &status_right, 0);
      break;
    }

    case NODE_REDIR:
      break;
    case NODE_CMD: {
      struct cmd_t *cmd=(struct cmd_t *)node->data;
      if(execvp(cmd->executable, cmd->argv) == -1) exit(72);
      break;
    }
  }
}
