#if FALSE
#include "get_type.h"

ast_t * nf(ast_t * a) {
  return a;
}

ast_t * assign(ast_t * v, context_t ct, char * fresh) {
  switch(v->kind) {
  case AST_KIND_VAR: {
    for(int i = 0; i < ct.length; ++i) {
      if(v->value.var == ct.data[i].var)
	return ct.data[i].lambda;
    }

    return v;
  }
  case AST_KIND_STAR:
  case AST_KIND_SORT: return v;
  case AST_KIND_LAMBDA: {
    for(int i = 0; i < ct.length; ++i) {
      if(v->value.lambda.x == ct.data[i].var)
	return ast_lambda(v->value.lambda.x, assign(v->value.lambda.M, ct, fresh), v->value.lambda.N);
      /*else if(v->value.lambda.x == ct.data[i].var.{
	

    var_def_t = { v->value.lambda, ast_var(*fresh) };
    context_t cct = {
    }*/
    }
    return ast_lambda(v->value.lambda.x, assign(v->value.lambda.M, ct, fresh), assign(v->value.lambda.N, ct, fresh));
  }
    
  }
}

ast_t * get_type(ast_t * v, judgement_l_t * jl, char * fresh) {
    switch (v->kind) {
    case AST_KIND_VAR:
        for (int i = 0; i < jl->context.length; ++i) {
            if (v->value.var == jl->context.data[i].var) {
                return jl->context.data[i].lambda;
            }
        }
        return nullptr;
    case AST_KIND_STAR: return ast_sort();
    case AST_KIND_SORT: return nullptr;
    case AST_KIND_APP: {
        ast_t * a = get_type(v->value.app.M, jl);
        if (a == nullptr || a->kind != AST_KIND_PAI) return nullptr;
        return nf(a->value.pai.N);
    }
    case AST_KIND_CONST: {
        definition_t * def = nullptr;
        for (int i = 0; i < jl->context.defs_length; ++i) {
            if (strcmp(v->value.cost.c, jl->context.defs[i].var) == 0)
            {
                def = &(jl->context.defs[i]);
                break;
            }
        }
        if (def == nullptr) return nullptr;
        return nf(assign(def->l2, def->context, fresh));
    }
    case AST_KIND_LAMBDA: {
        definition_t * b = get_type(v->value.lambda.N);
        if (b == nullptr) return nullptr;
        return ast_pai(b);
    }
    case AST_KIND_PAI:
        return get_type(v->value.lambda.N);
    }
}
#endif