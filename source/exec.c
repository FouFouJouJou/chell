#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <lexer.h>
#include <parser.h>
#include <exec.h>
#include <history.h>
#include <builtin.h>

int get_here_document(char **buffer, char *tag) {
  size_t size=0;
  while(1) {
    printf("> ");
    char *line=0;
    size_t line_size;
    size_t read=getline(&line, &line_size, stdin);
    if(!strncmp(line, tag, strlen(tag))) {
      free(line);
      line=0;
      break;
    }
    size+=read;
    *buffer=realloc(*buffer, sizeof(char)*(size+1));
    strncpy(*buffer+size-read, line, read);
    free(line);
  }
  return size;
}

int run_cmd(char *cmd, size_t len) {
  struct token_t *tokens=lex(cmd, len);
  struct node_t *head=parse(tokens);
  int exit_code=run(head);
  free_tree(head);
  free(tokens);
  return exit_code;
}

int run(struct node_t *node) {
  switch(node->type) {
    case NODE_ENV: {
      struct env_t *env=(struct env_t *)node->data;
      if(env->cmd == 0) {
        for(int i=0; i<env->size; ++i) {
          setenv(env->keys[i], env->values[i], 1);
        }
        return 0;
      }
      pid_t process=fork();
      if(process == 0) {
        for(int i=0; i<env->size; ++i) {
          setenv(env->keys[i], env->values[i], 1);
        }
        exit(run(env->cmd));
      }
      int status;
      if(waitpid(process, &status, WUNTRACED) == -1) exit(70);
      if(WIFEXITED(status)) {
        return WEXITSTATUS(status);
      }
    }

    case NODE_PIPE: {
      int p[2], left_status, right_status;
      if(pipe(p) == -1) exit(71);

      pid_t left_process=fork();
      if(left_process == 0) {
        if(dup2(p[1], STDOUT_FILENO) == -1) exit(69);
        close(p[0]);
        close(p[1]);
        exit(run(node->left));
      }

      pid_t right_process=fork();
      if(right_process == 0) {
        if(dup2(p[0], STDIN_FILENO) == -1) exit(69);
        close(p[0]);
        close(p[1]);
        exit(run(node->right));
      }
      close(p[0]);
      close(p[1]);

      if(waitpid(left_process, &left_status, WUNTRACED) == -1) exit(70);
      if(WIFEXITED(left_status)) {
        return WEXITSTATUS(left_status);
      }
      if(waitpid(right_process, &right_status, WUNTRACED) == -1) exit(70);
      if(WIFEXITED(right_status)) {
        return WEXITSTATUS(right_status);
      }
    }

    case NODE_SEMI_COLON: {
      int left_status, right_status;
      pid_t left_process=fork();
      if(left_process == 0) {
        exit(run(node->left));
      }
      if(waitpid(left_process, &left_status, WUNTRACED) == -1) exit(70);
      pid_t right_process=fork();
      if(right_process == 0) {
        exit(run(node->right));
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
        exit(run(node->left));
      }
      if(waitpid(left_process, &left_status, WUNTRACED) == -1) exit(70);
      if(WIFEXITED(left_status) && WEXITSTATUS(left_status) != EXIT_SUCCESS) return WEXITSTATUS(left_status); 

      pid_t right_process=fork();
      if(right_process == 0) {
        exit(run(node->right));
      }
      if(waitpid(right_process, &right_status, WUNTRACED) == -1) exit(70);
      if(WIFEXITED(right_status)) {
        return WEXITSTATUS(right_status);
      }
      break;
    }

    // TODO: > overwrites only the first n bytes
    case NODE_REDIR: {
      struct redir_t *redir=(struct redir_t*)node->data;
      pid_t process=fork();
      if(process == 0) {
        if(redir->output_file != 0) {
          int flags=O_WRONLY|(redir->flags & 0x01 ? O_APPEND : O_TRUNC);
          int fd=open(redir->output_file, O_CREAT|flags, S_IWUSR|S_IRUSR);
          if((fd == -1) && (EEXIST == errno)) {
            fd = open(redir->output_file, flags, S_IWUSR|S_IRUSR);
          }
          if(dup2(fd, STDOUT_FILENO) == -1) exit(72);
          close(fd);
        }
        if(redir->input_file != 0) {
          int fd=open(redir->input_file, O_RDONLY, S_IWUSR|S_IRUSR);
          if(fd == -1) exit(80);
          if(dup2(fd, STDIN_FILENO) == -1) exit(72);
          close(fd);
        }
        else if(redir->here_tag != 0) {
          char *input=0;
          size_t read=get_here_document(&input, redir->here_tag);
          // NOTE: not using tmp file because of bad wsl support (O_TMPFILE creation flag)
          int fd=open(".tmp", O_CREAT | O_RDWR, S_IWUSR|S_IRUSR);
          write(fd, input, read);
          free(input);
          close(fd);
          fd=open(".tmp", O_RDONLY);
          if(dup2(fd, STDIN_FILENO) == -1) exit(69);
          if(close(fd) == -1) exit(69);
        }
        if(redir->error_file != 0) {
          int fd=open(redir->error_file, O_CREAT|O_WRONLY|O_TRUNC, S_IWUSR|S_IRUSR);
          if((fd == -1) && (EEXIST == errno)) {
            fd = open(redir->error_file, O_TRUNC|O_WRONLY, S_IWUSR|S_IRUSR);
          }
          if(dup2(fd, STDERR_FILENO) == -1) exit(72);
        }
        else if(redir->flags >> 7 == 1) {
          if(dup2(redir->efd, STDERR_FILENO) == -1) exit(72);
        }
        exit(run(redir->cmd));
      }

      int status;
      if(waitpid(process, &status, WUNTRACED) == -1) exit(70);
      if(redir->here_tag) {
        unlink(".tmp");
      }
      if(WIFEXITED(status)) {
        return WEXITSTATUS(status);
      }
      break;
    }

    case NODE_CMD: {
      struct cmd_t *cmd=(struct cmd_t *)node->data;
      if(is_builtin(cmd->executable))
        return exec_builtin(*cmd);

      pid_t process=fork();
      if(process == 0) {
        if(execvp(cmd->executable, cmd->argv) == -1) {
          write(STDERR_FILENO, "Nope\n", 5);
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
