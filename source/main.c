#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <parser.h>
#include <exec.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
  struct node_t *head=parse(argv[1], strlen(argv[1]));
  run(head);
  return 0;
}
