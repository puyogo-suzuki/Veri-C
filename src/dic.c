#include "dic.h"

void dic_add(dic_t * di, int key, void * value) {
	int v = key % DIC_SIZE;
	dic_seq_t ** place = &(di->values[v]);
	while (*place != nullptr) {
		dic_seq_t * cur = *place;
		if (cur->len < DIC_SEQ_SIZE) {
			cur->values[cur->len] = value;
			cur->keys[cur->len] = key;
			cur->len++;
			return;
		}
		place - &(cur->next);
	}
	*place = (dic_seq_t *)malloc(sizeof(dic_seq_t));
	dic_seq_t * cur = *place;
	cur->len = 0;
	cur->next = nullptr;
}

int dic_search(dic_t * di, int key, void ** out) {
	int v = key % DIC_SIZE;
	dic_seq_t * cur = di->values[v];
	while (cur != nullptr) {
		for (int i = 0; i < cur->len; ++i) {
			if (cur->keys[i] == key) {
				*out = cur->values[i];
				return 1;
			}
		}
		cur = cur->next;
	}
	return 0;
}

void dic_seq_free(dic_seq_t * ds) {
	while (ds != nullptr) {
		dic_seq_t * ne = ds->next;
		free(ds);
		ds = ne;
	}
}

void dic_free(dic_t * di) {
	for (int i = 0; i < DIC_SIZE; ++i)
		dic_seq_free(di->values[i]);
}