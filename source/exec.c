#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <parser.h>
#include <exec.h>
#include <sys/wait.h>

// TODO: terminal output bug
int run(struct node_t *node) {
  switch(node->type) {
    case NODE_PIPE: {
      int p[2], left_status, right_status;
      if(pipe(p) == -1) exit(71);

      pid_t left_process=fork();
      if(left_process == 0) {
        if(dup2(p[1], STDOUT_FILENO) == -1) exit(69);
        close(p[0]);
        close(p[1]);
        int status=run(node->left);
        exit(status);
      }

      pid_t right_process=fork();
      if(right_process == 0) {
        if(dup2(p[0], STDIN_FILENO) == -1) exit(69);
        close(p[0]);
        close(p[1]);
        int status=run(node->right);
        exit(status);
      }
      close(p[0]);
      close(p[1]);
      if(waitpid(left_process, &left_status, WUNTRACED) == -1) exit(70);
      if(WIFEXITED(left_status)) {
        int status_code=WEXITSTATUS(left_status);
        if(status_code) exit(status_code);
      }
      if(waitpid(right_process, &right_status, WUNTRACED) == -1) exit(70);
      if(WIFEXITED(right_status)) {
        int status_code=WEXITSTATUS(right_status);
        if(status_code) exit(status_code);
      }
      return 0;
    }

    case NODE_SEMI_COLON:
      int left_status, right_status;
      pid_t left_process=fork();
      if(left_process == 0) {
        int status=run(node->left);
        exit(status);
      }
      waitpid(left_process, &left_status, WUNTRACED);

      pid_t right_process=fork();
      if(right_process == 0) {
        int status=run(node->right);
        exit(status);
      }
      waitpid(right_process, &right_status, WUNTRACED);
      if(WIFEXITED(right_status)) {
        return WEXITSTATUS(right_status);
      }
      break;

    case NODE_CMD: {
      pid_t process=fork();
      if(process == 0) {
        struct cmd_t *cmd=(struct cmd_t *)node->data;
        if(execvp(cmd->executable, cmd->argv) == -1) {
          printf("Nope\n");
          exit(72);
        }
      }
      int status=0;
      waitpid(process, &status, WUNTRACED);
      if(WIFEXITED(status)) {
        return(WEXITSTATUS(status));
      }
    }
  }
}
