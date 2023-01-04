#include <string.h>
#include "alpha.h"

int alpha_eq_go(ast_t * a, ast_t * b, char (* dicA)[128], char (* dicB)[128], char * fresh) {
  if(a->kind != b->kind) return 0;
  switch(a->kind) {
  case AST_KIND_VAR: return (*dicA)[a->value.var] == (*dicB)[b->value.var];
  case AST_KIND_APP:
    return alpha_eq_go(a->value.app.M, b->value.app.M, dicA, dicB, fresh)
      && alpha_eq_go(a->value.app.N, b->value.app.M, dicA, dicB, fresh);
  case AST_KIND_LAMBDA:
  case AST_KIND_PAI:
    if(!alpha_eq_go(a->value.lambda.M, b->value.lambda.M, dicA, dicB, fresh)) return 0;
    (*dicA)[a->value.lambda.x] = *fresh;
    (*dicB)[b->value.lambda.x] = *fresh;
    *fresh = *fresh + 1;
    return alpha_eq_go(a->value.lambda.N, b->value.lambda.N, dicA, dicB, fresh);
  case AST_KIND_CONST:
    if(strcmp(a->value.cont.c, b->value.cont.c) != 0) return 0;
    ast_list_t * aa = a->value.cont.args, * bb = b->value.cont.args;
    while(1) {
      if(aa == nullptr && bb == nullptr) return 1;
      if(aa == nullptr || bb == nullptr) return 0;
      if(!alpha_eq_go(aa->value, bb->value, dicA, dicB, fresh)) return 0;
      aa = aa->next;
      bb = bb->next;
    }
    return 0;
  }
  return 1;
}

int alpha_eq(ast_t * a, ast_t * b) {
  char dicA[0x80] = {0};
  char dicB[0x80] = {0};
  char fresh = 0x80;
  for(int i = 0; i < 0x80; ++i) { dicA[i] = i; dicB[i] = i; }
  return alpha_eq_go(a, b, &dicA, &dicB, &fresh);
}
