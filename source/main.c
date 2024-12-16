#include <stdio.h>
#include <string.h>
#include <parser.h>

int main(int argc, char **argv) {
  struct node_t *head=parse(argv[1], strlen(argv[1]));
  return 0;
}
