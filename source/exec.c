#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <parser.h>
#include <exec.h>
#include <sys/wait.h>

int run(struct node_t *node, int ifd, int ofd) {
  switch(node->type) {
    case NODE_PIPE: {
      int p[2];
      if(pipe(p) == -1) exit(71);
      
      if(ifd != STDIN_FILENO) {
        if(dup2(ifd, STDIN_FILENO) == -1) exit(69);
        close(ifd);
      }
      if(ofd != STDOUT_FILENO) {
        if(dup2(ofd, STDOUT_FILENO) == -1) exit(69);
        close(ofd);
      }

      int status1=run(node->left, STDIN_FILENO, p[1]);
      close(p[1]);
      int status2=run(node->right, p[0], STDOUT_FILENO);
      close(p[0]);
      exit(!status1 && !status2);
    }

    case NODE_CMD: {
      pid_t process=fork();
      int status;
      if(process == 0) {
        struct cmd_t *cmd=(struct cmd_t *)node->data;
        if(ofd != STDOUT_FILENO) {
          if(dup2(ofd, STDOUT_FILENO) == -1) exit(69);
          close(ofd);
        }
        if(ifd != STDIN_FILENO) {
          if(dup2(ifd, STDIN_FILENO) == -1) exit(69);
          close(ifd);
        }

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
