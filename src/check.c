#include <stdlib.h>
#include <stdio.h>
#include "ast.h"     
#include "alpha.h"
#include "check.h"
#include "misc.h"

void print_context(context_t * cont) {
    if (cont->length > 0)
    {
        printf("%c : ", cont->data[0].var);
        print_ast(cont->data[0].lambda);
    }
    for (int i = 1; i < cont->length; ++i) {
        printf(", ");
        printf("%c : ", cont->data[i].var);
        print_ast(cont->data[i].lambda);
    }
}

void print_defs(definition_t * defs, int length) {
    for (int i = 0; i < length; ++i) {
        print_context(&(defs[i].cont));
        printf(" |> ");
        printf("%s = ", defs[i].name);
        print_ast(defs[i].l1);
        printf(" : ");
        print_ast(defs[i].l2);
        printf(", ");
    }
}

void print_left(judgement_l_t * le) {
    print_defs(le->defs, le->defs_length);
    printf(";");
    print_context(&(le->context));
}

void print_judgement(judgement_t * judge) {
    print_left(&(judge->left));
    printf(" |- ");
    print_ast(judge->l1);
    printf(" : ");
    print_ast(judge->l2);
}

int delta_eq(ast_t * m, ast_t * n) {
    return 1;
}

int eq_context(context_t * a, context_t * b, int n) {
    if (n == -1) {
        if (a->length != b->length) return 0;
    }
    else {
        if (a->length < n || b->length < n) return 0;
    }

  for(int i = 0; i < a->length; ++i) {
    if(a->data->var != b->data->var) return 0;
    if(!alpha_eq(a->data->lambda, b->data->lambda)) return 0;
  }
  return 1;
}

int eq_definitions(judgement_l_t * a, judgement_l_t * b, int m, int n) {
    if (m == -1) {
        if (a->defs_length != b->defs_length) return 0;
    }
    else {
        if (a->defs_length < m || b->defs_length < m)return 0;
    }

    if (!eq_context(&(a->context), &(b->context), n)) return 0;
    for (int i = 0; i < a->defs_length; ++i) {
        if (!alpha_eq(a->defs[i].l1, b->defs[i].l1) || !alpha_eq(a->defs[i].l2, b->defs[i].l2))
            return 0;
        if (strcmp(a->defs[i].name, b->defs[i].name) == 0) return 0;
        if (!eq_context(&(a->defs[i].cont), &(b->defs[i].cont), -1)) return 0;
    }
    return 1;
}

void context_dup(context_t * dst, context_t * src) {
    *dst = *src;
}

void def_dup(judgement_l_t * dst, judgement_l_t * src) {
    dst->defs = src->defs;
    dst->defs_length = src->defs_length;
}

ast_t * replace_ch(ast_t * n, char from, char to) {
    switch (n->kind) {
    case AST_KIND_VAR:
        return n->value.var == from ? n : ast_var(to);
    case AST_KIND_APP: {
        ast_t * nM = replace_ch(n->value.app.M, from, to);
        ast_t * nN = replace_ch(n->value.app.N, from, to);
        if (nM == n->value.app.M && nN == n->value.app.N) return n;
        nM->rc++; nN->rc++;
        return ast_app(nM, nN);
    }
    case AST_KIND_LAMBDA:
    case AST_KIND_PAI: {
        ast_t * nM = replace_ch(n->value.pai.M, from, to);
        if (n->value.pai.x == from) {
            if (nM == n->value.pai.M) return n;
            nM->rc++;
            return ast_pai(from, nM, n->value.pai.N);
        }
        else {
            ast_t * nN = replace_ch(n->value.pai.N, from, to);
            if (nN == n->value.pai.N && nM == n->value.pai.M) return n;
            nN->rc++; nM->rc++;
            return n->kind == AST_KIND_LAMBDA ? ast_lambda(n->value.lambda.x, nM, nN) : ast_pai(n->value.pai.x, nM, nN);
        }
    }
    case AST_KIND_CONST: {
        ast_list_t * al = nullptr;
        ast_list_t ** las = &al;
        for (ast_list_t * cur = n->value.cont.args; cur != nullptr; cur = cur->next) {
            *las = (ast_list_t *)malloc(sizeof(ast_list_t));
            (*las)->value = replace_ch(cur->value, from, to);
            (*las)->value->rc++;
            (*las)->next = nullptr;
            las = &((*las)->next);
        }
        return ast_const(strdup(n->value.cont.c), al);
    }
    default: return n;
    }
}

ast_t * replace(ast_t * n, char from, ast_t * to) {
    switch (n->kind) {
    case AST_KIND_VAR:
        return n->value.var == from ? n : to;
    case AST_KIND_APP: {
        ast_t * nM = replace(n->value.app.M, from, to);
        ast_t * nN = replace(n->value.app.N, from, to);
        if (nM == n->value.app.M && nN == n->value.app.N) return n;
        nM->rc++; nN->rc++;
        return ast_app(nM, nN);
    }
    case AST_KIND_LAMBDA:
    case AST_KIND_PAI: {
        ast_t * nM = replace(n->value.pai.M, from, to);
        if (n->value.pai.x == from) {
            if (nM == n->value.pai.M) return n;
            nM->rc++;
            return ast_pai(from, nM, n->value.pai.N);
        }
        else {
            ast_t * nN = replace(n->value.pai.N, from, to);
            if (nN == n->value.pai.N && nM == n->value.pai.M) return n;
            nN->rc++; nM->rc++;
            return n->kind == AST_KIND_LAMBDA ? ast_lambda(n->value.lambda.x, nM, nN) : ast_pai(n->value.pai.x, nM, nN);
        }
    }
    case AST_KIND_CONST: {
        ast_list_t * al = nullptr;
        ast_list_t ** las = &al;
        for (ast_list_t * cur = n->value.cont.args; cur != nullptr; cur = cur->next) {
            *las = (ast_list_t *)malloc(sizeof(ast_list_t));
            (*las)->value = replace(cur->value, from, to);
            (*las)->value->rc++;
            (*las)->next = nullptr;
            las = &((*las)->next);
        }
        return ast_const(strdup(n->value.cont.c), al);
    }
    default: return n;

    }
}

judgement_t * check_sort(void) {
  judgement_t * ret = (judgement_t * )malloc(sizeof(judgement_t));
  ret->left.defs = NULL;
  ret->left.defs_length = 0;
  ret->left.context.data = NULL;
  ret->left.context.length = 0;
  ret->l1 = ast_aster();
  ret->l1->rc++;
  ret->l2 = ast_sort();
  ret->l2->rc++;
  return ret;
}

judgement_t * check_var(judgement_t * m, char v) {
  judgement_t * ret = (judgement_t *)malloc(sizeof(judgement_t));
  def_dup(&(ret->left), &(m->left));
  ret->left.context.data = malloc(sizeof(var_def_t) * (m->left.context.length + 1));
  ret->left.context.length = m->left.context.length + 1;
  memcpy(ret->left.context.data, m->left.context.data, m->left.context.length * sizeof(var_def_t));
  ret->left.context.data[m->left.context.length].var = v;
  ret->left.context.data[m->left.context.length].lambda = m->l1;
  m->l1->rc++;
  ret->l1 = ast_var(v);
  ret->l2 = m->l1;
  m->l1->rc++;
  return ret;
}

judgement_t * check_weak(judgement_t * m, judgement_t * n, char v) {
  if(!eq_definitions(&m->left, &n->left, -1, -1)) return 0;
  for (int i = 0; i < m->left.context.length; ++i) {
      if (m->left.context.data[i].var == v) return 0;
  }
  judgement_t * ret = (judgement_t *)malloc(sizeof(judgement_t));
  def_dup(&(ret->left), &(m->left));
  ret->left.context.data = malloc(sizeof(var_def_t) * (m->left.context.length + 1));
  ret->left.context.length = m->left.context.length + 1;
  memcpy(ret->left.context.data, m->left.context.data, m->left.context.length * sizeof(var_def_t));
  ret->left.context.data[m->left.context.length].var = v;
  ret->left.context.data[m->left.context.length].lambda = n->l1;
  n->l1->rc++;
  ret->l1 = m->l1;
  m->l1->rc++;
  ret->l2 = m->l2;
  m->l2->rc++;
  return ret;
}

judgement_t * check_form(judgement_t * m, judgement_t * n) {
    if (m->left.context.length + 1 != n->left.context.length)  return 0;
    if (!eq_definitions(&(m->left), &(n->left), -1, m->left.context.length)) return 0;
    judgement_t * ret = (judgement_t *)malloc(sizeof(judgement_t));
    ret->left.defs = memdup(m->left.defs, m->left.defs_length * sizeof(definition_t));
    ret->left.defs_length = m->left.defs_length;
    context_dup(&(ret->left.context), &(m->left.context));

    var_def_t * la = &(n->left.context.data[n->left.context.length - 1]);
    if(!alpha_eq(la->lambda, m->l1)) return 0;
    la->lambda->rc++;
    ret->l1 = ast_pai(la->var, la->lambda, n->l1);
    ret->l2 = n->l2;
    ret->l1->rc++;
    n->l1->rc++;
    n->l2->rc++;
    return ret;
}


judgement_t * check_appl(judgement_t * m, judgement_t * n) {
    if (!eq_definitions(&(m->left), &(n->left), -1, -1)) return 0;
    if (m->l2->kind != AST_KIND_PAI) return 0;
    judgement_t * ret = (judgement_t *)malloc(sizeof(judgement_t));
    def_dup(&(ret->left), &(m->left));
    context_dup(&(ret->left.context), &(m->left.context));
    ret->l1 = ast_app(m->l1, n->l1);
    m->l1->rc++;
    n->l1->rc++;
    ret->l1->rc++;
    ret->l2 = replace(m->l2->value.pai.N, m->l2->value.pai.x, n->l1);
    m->l2->value.pai.N->rc++;
    n->l1->rc++;
    ret->l2->rc++;

    return ret;
}

judgement_t * check_abst(judgement_t * m, judgement_t * n) {
    if (m->left.context.length != n->left.context.length + 1)  return 0;
    if (!eq_definitions(&(m->left), &(n->left), -1, n->left.context.length)) return 0;
    if (m->l1->kind != AST_KIND_PAI) return 0;
    judgement_t * ret = (judgement_t *)malloc(sizeof(judgement_t));
    def_dup(&(ret->left), &(m->left));
    context_dup(&(ret->left.context), &(n->left.context));
    ret->l1 = ast_lambda(m->left.context.data[m->left.context.length-1].var, m->left.context.data[m->left.context.length - 1].lambda, m->l1);
    ret->l1->rc++;
    ret->l1->value.lambda.M->rc++;
    ret->l1->value.lambda.N->rc++;
    ret->l2 = n->l1;
    n->l1->rc++;
    return ret;
}

judgement_t * check_conv(judgement_t * m, judgement_t * n) {
    if (!delta_eq(m->l2, n->l1)) return 0;
    if (!eq_definitions(&(m->left), &(n->left), -1, -1)) return 0;
    judgement_t * ret = (judgement_t *)malloc(sizeof(judgement_t));
    def_dup(&(ret->left), &(m->left));
    context_dup(&(ret->left.context), &(n->left.context));
    ret->l1 = m->l1;
    m->l1->rc++;
    ret->l2 = n->l1;
    n->l1->rc++;
    return ret;
}

char * char_to_str(char v) {
    char * ret = malloc(sizeof(char) * 2);
    ret[0] = v;
    ret[1] = 0;
    return ret;
}

judgement_t * check_def(judgement_t * m, judgement_t * n, char v) {
    if (!eq_definitions(&(m->left), &(n->left), -1, -1)) return 0;
    for (int i = 0; i < m->left.defs_length; ++i) {
        if (m->left.defs[i].name[0] == v && m->left.defs[i].name[1] == 0) return 0;
    }
    judgement_t * ret = (judgement_t *)malloc(sizeof(judgement_t));
    ret->left.defs = (definition_t *)malloc(sizeof(definition_t) * (m->left.defs_length + 1));
    ret->left.defs_length = m->left.defs_length + 1;
    context_dup(&(ret->left.defs[m->left.defs_length].cont), &(n->left.context));
    ret->left.defs[m->left.defs_length].name = char_to_str(v);
    ret->left.defs[m->left.defs_length].l1 = n->l1;
    ret->left.defs[m->left.defs_length].l2 = n->l2;
    n->l1->rc++;
    n->l2->rc++;
    context_dup(&(ret->left.context), &(m->left.context));
    ret->l1 = m->l1;
    m->l1->rc++;
    ret->l2 = m->l2;
    m->l2->rc++;
    return ret;
}

judgement_t * check_def_prim(judgement_t * m, judgement_t * n, char v) {
    if (!eq_definitions(&(m->left), &(n->left), -1, -1)) return 0;
    for (int i = 0; i < m->left.defs_length; ++i) {
        if (m->left.defs[i].name[0] == v && m->left.defs[i].name[1] == 0) return 0;
    }
    judgement_t * ret = (judgement_t *)malloc(sizeof(judgement_t));
    ret->left.defs = (definition_t *)malloc(sizeof(definition_t) * (m->left.defs_length + 1));
    ret->left.defs_length = m->left.defs_length + 1;
    context_dup(&(ret->left.defs[m->left.defs_length].cont), &(n->left.context));
    ret->left.defs[m->left.defs_length].name = char_to_str(v);
    ret->left.defs[m->left.defs_length].l1 = nullptr;
    ret->left.defs[m->left.defs_length].l2 = n->l1;
    n->l1->rc++;
    context_dup(&(ret->left.context), &(m->left.context));
    ret->l1 = m->l1;
    m->l1->rc++;
    ret->l2 = m->l2;
    m->l2->rc++;
    return ret;
}

judgement_t * check_inst(judgement_t * m, judgement_t ** n, int v) {
    if (v >= m->left.defs_length) return 0;
    for (judgement_t ** cur = n; cur != nullptr; cur++) {
        if (!eq_definitions(&(m->left), &((*cur)->left), -1, -1)) return 0;
        if (!eq_context(&(m->left.context), &((*cur)->left.context), -1)) return 0;
    }
    if (m->l1->kind != AST_KIND_STAR || m->l2->kind != AST_KIND_SORT) return 0;

    judgement_t * ret = (judgement_t *)malloc(sizeof(judgement_t));
    def_dup(&(ret->left), &(m->left));
    context_dup(&(ret->left.context), &(m->left.context));
    ast_t * nn = m->left.defs[v].l2;
    nn->rc++;
    ast_list_t * al = nullptr;  
    ast_list_t ** las = &al;
    for (judgement_t ** cur = n; cur != nullptr; cur++) {
        *las = (ast_list_t *)malloc(sizeof(ast_list_t));
        (*las)->value = (*cur)->l1;
        (*las)->next = nullptr;
        (*cur)->l1->rc++;
        las = &((*las)->next);
    }
    ret->l1 = ast_const(strdup(m->left.defs[v].name), al);
    for (int i = 0; i < m->left.defs[v].cont.length; ++i)
    {
        ast_t * nnn = replace(nn, m->left.defs[v].cont.data[i].var, m->left.defs[v].cont.data[i].lambda);
        nnn->rc++;
        free_ast(nn);
        nn = nnn;
    }
    ret->l2 = nn;
    return ret;
}