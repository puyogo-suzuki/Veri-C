#include <stdio.h>
#include <parse.h>
#include <ast.h>

int main(void) {
  char buf[512];
  fgets(buf, 500, stdin);
  ast_t * a = parse(buf);
  if(a == nullptr) {
    printf("failed.\n");
    return 0;
  }
  print_ast(a);
  return 0;
}
