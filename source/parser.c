#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <lexer.h>
#include <parser.h>
#define YIELD(x) (x)++
#define ASSERT_T(x, t) assert((x)->type == t)

void printf_tree(struct node_t *head, int level) {
}

struct node_t *build_tree(struct token_t *tokens) {
  struct node_t *head=0;
  struct token_t *token=tokens;
  struct token_t *current_token=YIELD(token);
  ASSERT_T(current_token, TOKEN_STRING);
  do {
    printf_token(*current_token);
    current_token=YIELD(token);
  } while(current_token->type != TOKEN_EOC);
  return head;
}

void run(struct node_t *head) {
}

struct node_t *parse(char *line, size_t len) {
  struct token_t *tokens=lex(line, len);
  struct node_t *head=build_tree(tokens);
  return head;
}
