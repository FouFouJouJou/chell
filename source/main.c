#include <stdio.h>
#include <string.h>
#include <lexer.h>

int main(int argc, char **argv) {
  printf("Hello world!\n");
  lex(argv[1], strlen(argv[1]));
  return 0;
}
