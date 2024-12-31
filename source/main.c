#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <exec.h>
#include <history.h>
#include <builtin.h>
#include <env.h>

int main(int argc, char **argv) {
  read_history_from_fs(0);
  while(1) {
    printf("chell> ");
    char *input=0;
    size_t size;
    size_t read=getline(&input, &size, stdin);
    if(read == -1) {
      printf("\n");
      free(input);
      input=0;
      exit(0);
    }
    int exit_code=run_cmd(input, read);
    last_exit_code=exit_code;
    save_to_history_buffer(input, read);
  }
  return EXIT_SUCCESS;
}
