#include <stdio.h>
#include "parse.h"
#include "ast.h"
#include "alpha.h"

int main(int argc, char ** argv) {
  if(argc != 3) {
    printf("usage: %s <expression1> <expression2>\n", argc == 0 ? "./veri-alpha" : argv[0]);
    printf("or you can try\n  $ cat <filename> | xargs %s",  argc == 0 ? "./veri-alpha" : argv[0]);
    return 1;
  }
  ast_t * a = parse(argv[1]);
  ast_t * b = parse(argv[2]);
  if(a == nullptr || b == nullptr)
    printf("parse fail.\n");
  
  return alpha_eq(a, b);
}
