#include <string.h>
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
    if(*str != ')') goto err;
    if(str[1] != '(') goto err;
    consume_context_t b = parse_expr(str + 2);
    if(b.result == nullptr) goto err;
    str = b.consumed;
    if(*str != ')') goto err2;
    ret.consumed = str + 1;
    a.result->rc++;
    b.result->rc++;
    ret.result = ast_app(a.result, b.result);
    return ret;
    err2: free_ast(b.result);
    err:  free_ast(a.result);
    break;
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
    if(*str != ')' || str[1] != '.' || str[2] != '(') goto err3;
    consume_context_t b = parse_expr(str + 3);
    if(b.result == nullptr) goto err3;
    str = b.consumed;
    if(*str != ')') goto err4;
    ret.consumed = str + 1;
    a.result->rc++;
    b.result->rc++;
    ret.result = first == '$' ? ast_lambda(x, a.result, b.result) :
      ast_pai(x, a.result, b.result);
    return ret;
    err4: free_ast(b.result);
    err3:  free_ast(a.result);
    break;
  }
  default: {
    char * s = str;
    if(isLower(*s) && !isAlphabet(s[1])) {
      ret.consumed = str + 1;
      ret.result = ast_var(*s);
      return ret;
    }
    if(!isAlphabet(*s)) return ret;
    while(isConstChar(*s)) s++;
    if(s == str) return ret;
    if(*s != '[') return ret;
    char * ss = s+1;
    ast_list_t * li = nullptr;
    ast_list_t ** last = &li;
    while(*ss != ']') {
      if(*ss != '(') goto err5;
      ss++;
      consume_context_t b = parse_expr(ss);
      if(b.result == nullptr)
	goto err;
      *last = (ast_list_t *)malloc(sizeof(ast_list_t));
      (*last)->value = b.result;
      b.result->rc++;
      (*last)->next = nullptr;
      last = &((*last)->next);
      ss = b.consumed;
      if(*ss != ')') goto err5;
      if(ss[1] == ']') break;
      if(ss[1] != ',') goto err5;
      ss += 2;
    }
    char * co = (char *)malloc(sizeof(char) + (s-str));
    memcpy(co, str, (s-str)/sizeof(char));
    co[(s-str)/sizeof(char)+1] = 0;
    ret.result = ast_const(co, li);
    ret.consumed = ss + 1;
    return ret;
    err5: free_ast_list(li);
    break;
  }
  }
  ret.result = nullptr;
  ret.consumed = str;
  return ret;
}

ast_t * parse(char *c) {
  consume_context_t cc = parse_expr(c);
  return cc.result;
}
