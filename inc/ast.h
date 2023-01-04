#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define nullptr NULL
struct ast_t;

typedef struct ast_list_t {
  struct ast_t * value;
  struct ast_list_t * next;
} ast_list_t;

typedef enum ast_kind_t {
  AST_KIND_VAR,
  AST_KIND_STAR,
  AST_KIND_SORT,
  AST_KIND_APP,
  AST_KIND_LAMBDA,
  AST_KIND_PAI,
  AST_KIND_CONST
} ast_kind_t;

typedef struct ast_t {
  ast_kind_t kind;
  union {
    char var;
    struct{ struct ast_t * M; struct ast_t * N; } app;
    struct{ char x; struct ast_t * M; struct ast_t * N; } lambda;
    struct{ char x; struct ast_t * M; struct ast_t * N; } pai;
    struct{ char * c; struct ast_list_t * args; } cont;
  } value;
} ast_t;


static inline
ast_t * ast_var(char ch) {
  ast_t * v = (ast_t *)malloc(sizeof(ast_t));
  v->kind = AST_KIND_VAR;
  v->value.var = ch;
  return v;
}

static inline
ast_t * ast_sort(void) {
  ast_t * v = (ast_t *)malloc(sizeof(ast_t));
  v->kind = AST_KIND_SORT;
  return v;
}

static inline
ast_t * ast_aster(void) {
  ast_t * v = (ast_t *)malloc(sizeof(ast_t));
  v->kind = AST_KIND_STAR;
  return v;
}

static inline
ast_t * ast_app(ast_t * M, ast_t * N) {
  ast_t * v = (ast_t *)malloc(sizeof(ast_t));
  v->kind = AST_KIND_APP;
  v->value.app.M = M;
  v->value.app.N = N;
  return v;
}

static inline
ast_t * ast_lambda(char x, ast_t * M, ast_t * N) {
  ast_t * v = (ast_t *)malloc(sizeof(ast_t));
  v->kind = AST_KIND_LAMBDA;
  v->value.lambda.x = x;
  v->value.lambda.M = M;
  v->value.lambda.N = N;
  return v;
}

static inline
ast_t * ast_pai(char x, ast_t * M, ast_t * N) {
  ast_t * v = (ast_t *)malloc(sizeof(ast_t));
  v->kind = AST_KIND_PAI;
  v->value.pai.x = x;
  v->value.pai.M = M;
  v->value.pai.N = N;
  return v;
}

static inline
ast_t * ast_const(char * c, ast_list_t * args) {
  ast_t * v = (ast_t *)malloc(sizeof(ast_t));
  v->kind = AST_KIND_PAI;
  v->value.cont.c = strdup(c);
  v->value.cont.args = args;
  return v;
}

void print_ast(ast_t * a);
