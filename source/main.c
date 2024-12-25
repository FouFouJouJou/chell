#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <exec.h>
#include <history.h>

int main(int argc, char **argv) {
  read_from_fs(0);
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
    if(!strncmp(input, "exit", 4)) {
      save_to_fs();
      exit(0);
    }

    if(!strncmp(input, "history", 7)) {
      log_history();
      continue;
    }
    int exit_code=run_cmd(input, read);
    printf("exit code: %d\n", exit_code);
    save_to_buffer(input, read);
  }
  return EXIT_SUCCESS;
}
