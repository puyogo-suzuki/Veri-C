#include <stdio.h>
#include "parse.h"
#include "check.h"

int main(int argc, char ** argv) {
	judgement_t * judges[100];
	for (int i = 0; i < 100; ++i) {
		int num;
		char inst[11];
		scanf("%d", &num);
		if (num == -1) return 0;
		char ch = ' '; int ch_itr = 0;
		while (ch == ' ') {
			scanf("%c", &ch);
		}
		while (ch != ' ') {
			inst[ch_itr] = ch;
			ch_itr++;
			scanf("%c", &ch);
			if (ch == '\n') break;
		}
		if (strncmp(inst, "sort", 4) == 0) {
			judges[i] = check_sort();
		}
		else if (strncmp(inst, "var", 3) == 0) {
			int m;
			char varname = ' ';
			scanf("%d", &m);
			while (varname == ' ') {
				scanf("%c", &varname);
			}
			judges[i] = check_var(judges[m], varname);
		}
		else if (strncmp(inst, "weak", 4) == 0) {
			int m;
			int n;
			char varname = ' ';
			scanf("%d%d", &m, &n);
			while (varname == ' ') {
				scanf("%c", &varname);
			}
			judges[i] = check_weak(judges[m], judges[n], varname);
		}
		else if (strncmp(inst, "form", 4) == 0) {
			int m, n;
			scanf("%d%d", &m, &n);
			judges[i] = check_form(judges[m], judges[n]);
		}
		else if (strncmp(inst, "appl", 4) == 0) {
			int m, n;
			scanf("%d%d", &m, &n);
			judges[i] = check_appl(judges[m], judges[n]);
		}
		else if (strncmp(inst, "abst", 4) == 0) {
			int m, n;
			scanf("%d%d", &m, &n);
			judges[i] = check_abst(judges[m], judges[n]);
		}
		else if (strncmp(inst, "def", 3) == 0) {
			int m;
			int n;
			char varname = ' ';
			scanf("%d%d", &m, &n);
			while (varname == ' ') {
				scanf("%c", &varname);
			}
			judges[i] = check_def(judges[m], judges[n], varname);
		}
		else if (strncmp(inst, "inst", 4) == 0) {
			int m;
			int n;
			scanf("%d%d", &m, &n);
			judgement_t ** ks = (judgement_t**)malloc((n + 1) * sizeof(judgement_t*));
			for (int j = 0; j < n; ++j) {
				int k = 0;
				scanf("%d", &k);
				ks[j] = judges[k];
			}
			ks[n] = NULL;
			int p;
			scanf("%d", &p);
			judges[i] = check_inst(judges[m], ks, p);
		}
		else if (strncmp(inst, "conv", 4) == 0) {
			int m, n;
			scanf("%d%d", &m, &n);
			judges[i] = check_conv(judges[m], judges[n]);
		}
		else if (strncmp(inst, "defpr", 5) == 0) {
			int m;
			int n;
			char varname = ' ';
			scanf("%d%d", &m, &n);
			while (varname == ' ') {
				scanf("%c", &varname);
			}
			judges[i] = check_def_prim(judges[m], judges[n], varname);
		}
		else {
			printf("fail\n");
			continue;
		}
		print_judgement(judges[i]);
		printf("\n");
	}
  return 0;
}
