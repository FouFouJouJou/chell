#include <stdio.h>
#include <string.h>
#include <lexer.h>

int main(int argc, char **argv) {
  struct token_t *tokens=lex(argv[1], strlen(argv[1])), *token=tokens;
  while(token->type != TOKEN_EOC) {
    printf_token(*token);
    token++;
  }
  return 0;
}
