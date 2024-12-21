#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <lexer.h>
#include <parser.h>
#include <exec.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
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
    input[read-1]='\0';
    if(!strncmp(input, "exit", 4)) exit(0);
    struct token_t *tokens=lex(input, read-1);
    struct node_t *head=parse(tokens);
    int exit_code=run(head);
    printf("exit code: %d\n", exit_code);
    free(tokens);
    free_tree(head);
    free(input);
  }
  return EXIT_SUCCESS;
}
