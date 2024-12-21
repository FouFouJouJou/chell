#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <lexer.h>
#include <parser.h>
#include <exec.h>

int run_cmd(char *cmd, size_t len) {
  struct token_t *tokens=lex(cmd, len);
  struct node_t *head=parse(tokens);
  int exit_code=run(head);
  free(tokens);
  free_tree(head);
  return exit_code;
}

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

    case NODE_SEMI_COLON: {
      int left_status, right_status;
      pid_t left_process=fork();
      if(left_process == 0) {
        int status=run(node->left);
        exit(status);
      }
      if(waitpid(left_process, &left_status, WUNTRACED) == -1) exit(70);

      pid_t right_process=fork();
      if(right_process == 0) {
        int status=run(node->right);
        exit(status);
      }
      if(waitpid(right_process, &right_status, WUNTRACED) == -1) exit(70);
      if(WIFEXITED(right_status)) {
        return WEXITSTATUS(right_status);
      }
      break;
    }

    case NODE_AND: {
      int left_status, right_status;
      pid_t left_process=fork();
      if(left_process == 0) {
        int status=run(node->left);
        exit(status);
      }
      if(waitpid(left_process, &left_status, WUNTRACED) == -1) exit(70);
      if(WIFEXITED(left_status) && WEXITSTATUS(left_status) != EXIT_SUCCESS) return WEXITSTATUS(left_status); 

      pid_t right_process=fork();
      if(right_process == 0) {
        int status=run(node->right);
        exit(status);
      }
      if(waitpid(right_process, &right_status, WUNTRACED) == -1) exit(70);
      if(WIFEXITED(right_status)) {
        return WEXITSTATUS(right_status);
      }
      break;
    }
    case NODE_OUT_TRUNC_REDIR: {
      pid_t process=fork();
      if(process == 0) {
        struct redir_t *redir=(struct redir_t*)node->data;
        int fd=creat(redir->output_file, S_IWUSR|S_IRUSR);
        printf("%d\n", fd);
        if(dup2(fd, STDOUT_FILENO) == -1) exit(72);
        close(fd);
        exit(run(redir->cmd));
      }
      int status;
      if(waitpid(process, &status, WUNTRACED) == -1) exit(70);
      if(WIFEXITED(status)) {
        return WEXITSTATUS(status);
      }
      break;
    }

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
      if(waitpid(process, &status, WUNTRACED) == -1) exit(70);
      if(WIFEXITED(status)) {
        return(WEXITSTATUS(status));
      }
    }
  }
}
