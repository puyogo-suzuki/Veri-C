#include <string.h>
#include <stdio.h>
#include "parse.h"
#include "ast.h"

typedef struct{
  char * consumed;
  ast_t * result;
} consume_context_t;

int isLower(char ch) {
  return ch >= 'a' && ch <= 'z';
}

int isAlphabet(char ch) {
  return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

int isConstChar(char ch) {
  return isAlphabet(ch) || ch == '_' || (ch >= '0' && ch <= '9');
}

consume_context_t parse_expr(char * str) {
  ast_t * a, b;
  consume_context_t ret = { str, nullptr };
  switch(*str) {
  case '\0': return ret;
  case '*':
    ret.consumed = str + 1;
    ret.result = ast_aster();
    return ret;
  case '@':
    ret.consumed = str + 1;
    ret.result = ast_sort();
    return ret;
  case '%': {
    str++;
    if(*str != '(') return ret;
    consume_context_t a = parse_expr(str + 1);
    if(a.result == nullptr) return ret;
    str = a.consumed;
    if(*str != ')') return ret;
    if(str[1] != '(') return ret;
    consume_context_t b = parse_expr(str + 2);
    if(b.result == nullptr) return ret;
    str = b.consumed;
    if(*str != ')') return ret;
    ret.consumed = str + 1;
    ret.result = ast_app(a.result, b.result);
    return ret;
  }
  case '$':
  case '?': {
    char first = *str;
    char x = str[1];
    if(!isLower(x)) return ret;
    if(str[2] != ':') return ret;
    str = str + 3;
    if(*str != '(') return ret;
    consume_context_t a = parse_expr(str + 1);
    if(a.result == nullptr) return ret;
    str = a.consumed;
    if(*str != ')' || str[1] != '.' || str[2] != '(') return ret;
    consume_context_t b = parse_expr(str + 3);
    if(b.result == nullptr) return ret;
    str = b.consumed;
    if(*str != ')') return ret;
    ret.consumed = str + 1;
    ret.result = first == '$' ? ast_lambda(x, a.result, b.result) :
      ast_pai(x, a.result, b.result);
    return ret;
  }
  default: {
    char * s = str;
    if(isLower(*s) && !isAlphabet(s[1])) {
      ret.consumed = str + 1;
      ret.result = ast_var(*s);
      return ret;
    }
    if(!isAlphabet(*s)) return ret;
    while(isAlphabet(*s)) s++;
    if(s == str) return ret;
    if(*s != '[') return ret;
    char * ss = s+1;
    ast_list_t * li = nullptr;
    ast_list_t ** last = &li;
    while(*ss != ']') {
      if(*ss != '(') return ret;
      ss++;
      consume_context_t b = parse_expr(ss);
      if(b.result == nullptr)
	return ret;
      *last = (ast_list_t *)malloc(sizeof(ast_list_t));
      (*last)->value = b.result;
      (*last)->next = nullptr;
      last = &((*last)->next);
      ss = b.consumed;
      if(*ss != ')') return ret;
      if(ss[1] == ']') break;
      if(ss[1] != ',') return ret;
      ss += 2;
    }
    char * co = (char *)malloc(sizeof(char) * (s-str));
    memcpy(s, co, s-str);
    ret.result = ast_const(co, li);
    ret.consumed = ss + 1;
    return ret;
  }
  }
}

void print_ast(ast_t * a) {
  if(a == nullptr) { printf("fail"); return; }
  switch(a->kind) {
  case AST_KIND_VAR: putc(a->value.var, stdout); return;
  case AST_KIND_STAR: putc('*', stdout); return;
  case AST_KIND_SORT: printf("@"); return;
  case AST_KIND_APP:
    putc('(', stdout);
    print_ast(a->value.app.M);
    putc(')', stdout);
    putc('(', stdout);
    print_ast(a->value.app.N);
    putc(')', stdout);
    return;
  case AST_KIND_LAMBDA:
  case AST_KIND_PAI:
    printf(a->kind == AST_KIND_LAMBDA ? "$" : "?");
    putc(a->value.lambda.x, stdout);
    putc(':', stdout);
    putc('(', stdout);
    print_ast(a->value.lambda.M);
    putc(')', stdout);
    putc('.', stdout);
    putc('(', stdout);
    print_ast(a->value.lambda.N);
    putc(')', stdout);
    return;
  case AST_KIND_CONST:
    printf("%s", a->value.cont.c);
    putc('[', stdout);
    ast_list_t * al = a->value.cont.args;
    while(1) {
      putc('(', stdout);
      print_ast(al->value);
      putc(')', stdout);
      al = al->next;
      if(al == nullptr) break;
      putc(',', stdout);
    }
    putc(']', stdout);
    return;
  }
}


ast_t * parse(char *c) {
  consume_context_t cc = parse_expr(c);
  return cc.result;
}
