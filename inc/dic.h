#pragma once
#include "misc.h"

#define DIC_SIZE 16
#define DIC_SEQ_SIZE 14

typedef struct dic_seq_t {
	int keys[DIC_SEQ_SIZE];
	void* values[DIC_SEQ_SIZE];
	int len;
	struct dic_seq_t * next;
} dic_seq_t;

typedef struct dic_t {
	dic_seq_t * values[DIC_SIZE];
} dic_t;

static inline void dic_new(dic_t * di) {
	for (int i = 0; i < DIC_SIZE; ++i)
		di->values[i] = nullptr;
}

void dic_add(dic_t * di, int key, void* value);
int dic_search(dic_t * di, int key, void ** out);
void dic_free(dic_t * di);