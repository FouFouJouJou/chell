#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <parser.h>
#include <exec.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
  char *input=0;
  size_t size;
  while(1) {
    printf("chell> ");
    size_t read=getline(&input, &size, stdin);
    if(read == -1) {
      printf("\n");
      free(input);
      input=0;
      exit(0);
    }
    input[read-1]='\0';
    if(!strncmp(input, "exit", 4)) exit(0);
    struct node_t *head=parse(input, read-1);
    int exit_code=run(head);
    printf("exit code: %d\n", exit_code);
  }
  return EXIT_SUCCESS;
}
