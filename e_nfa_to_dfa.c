/* Epsilon-NFA --> DFA conversion program */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> 
#include <string.h> 
#define STATES 99 
#define SYMBOLS 20 
int N_sym; 
int NFA_states;
char *NFA_table[STATES][SYMBOLS];
char *NFA_finals;
int DFA_states; 
int DFA_table[STATES][SYMBOLS];
char DFA_finals[STATES + 1]; 
char StateName[STATES][STATES + 1];
char Eclosure[STATES][STATES + 1];


void show_NFA(char *tab[][SYMBOLS], int nstates, 	int nsymbols, char *finals){
	int i, j;
	puts("\nNFA: STATE TRANSITION TABLE");
	printf("     | ");
	for (i = 0; i < nsymbols; i++) printf(" %-6c", '0' + i);
	printf(" e\n");
	printf("-----+--");
	for (i = 0; i < nsymbols + 1; i++) printf("-------");
	printf("\n");
	for (i = 0; i < nstates; i++) {
		printf(" %c   | ", '0' + i);
		for (j = 0; j < nsymbols + 1; j++) printf(" %-6s", tab[i][j]);
		printf("\n");
	}
	printf("Final states = %s\n", finals);
}


void show_DFA(char *tab[][SYMBOLS], int nstates, int nsymbols, char *finals){
	int i, j;
	puts("\nDFA: STATE TRANSITION TABLE");
	printf("     | ");
	for (i = 0; i < nsymbols; i++) printf(" %c ", '0' + i);
	printf("\n-----+--");
	for (i = 0; i < nsymbols; i++) printf("-----");
	printf("\n");
	for (i = 0; i < nstates; i++) {
		printf(" %c   | ", 'A' + i);
		/* state */
		for (j = 0; j < nsymbols; j++) printf(" %c ", tab[i][j]);
		printf("\n");
	}
	printf("Final states = %s\n", finals);
}
/* Initialize NFA table. */
void load_NFA() {
	NFA_table[0][0] = "1";
	NFA_table[0][1] = "";
	NFA_table[0][2] = "";
	NFA_table[0][3] = "2";
	NFA_table[1][0] = "";
	NFA_table[1][1] = "3";
	NFA_table[1][2] = "";
	NFA_table[1][3] = "";
	NFA_table[2][0] = "";
	NFA_table[2][1] = "";
	NFA_table[2][2] = "2";
	NFA_table[2][3] = "3";
	NFA_table[3][0] = "";
	NFA_table[3][1] = "";
	NFA_table[3][2] = "";
	NFA_table[3][3] = "";
	N_sym = 3;
	NFA_states = 4;
	NFA_finals = "3";
	DFA_states = 0;
}

// t를 s로 합쳐줍니다. 입실론들을 하나로 연결시켜줍니다.
int string_merge(char *s, char *t) {
	int n = 0;
	char temp[STATES + 1], *r = temp, *p = s;
	while (*p && *t) {
		if (*p == *t) {
			*r++ = *p++;
			t++;
		}
		else if (*p < *t) {
			*r++ = *p++;
		}
		else {
			*r++ = *t++;
			n++;
			/* an item is added to 's' */
		}
	}
	*r = '\0';
	if (*t) {
		strcat(r, t);
		n += strlen(t);
	}
	else if (*p) strcat(r, p);
	strcpy(s, temp);
	return n;
}

// string_merge 메소드를 이용하여 입실론으로 연결된 스테이트들을 하나로 합쳐줍니다.
void get_next_state(char *nextstates, char *cur_states, char *nfa[STATES][SYMBOLS], int symbol) {
	int i;
	char temp[STATES + 1];
	temp[0] = '\0';
	for (i = 0; i < strlen(cur_states); i++) 
		string_merge(temp, nfa[cur_states[i] - '0'][symbol]);
	strcpy(nextstates, temp);
}

int get_state_index(char *state, char stnt[][STATES + 1], int *pn) {
	int i;
	if (!*state) return -1;
	// 스테이트가 없는 경우
	for (i = 0; i < *pn; i++) if (!strcmp(state, stnt[i])) return i;
	strcpy(stnt[i], state);
	// 새로운 스테이트
	return (*pn)++;
}
void get_e_states(int state, char *epstates, char *nfa[][SYMBOLS], int n_sym) {
	int i, n = 0;
	// n = 추가된 스테이트들의 수
	strcpy(epstates, nfa[state][n_sym]);
	do {
		// 입실론 스테이트들을 연결
		for (i = 0; i < strlen(epstates); i++)
			n = string_merge(epstates, nfa[epstates[i] - '0'][n_sym]);
	} while (n);
	/* 더 이상 추가되는 state가 없을 때까지 */
}

// Eclosure 초기화
void init_Eclosure(char eclosure[][STATES + 1], char *nfa[][SYMBOLS], int n_nfa, int n_sym) {
	int i;
	printf("\nEpsilon-accessible states:\n");
	for (i = 0; i < n_nfa; i++) {
		get_e_states(i, eclosure[i], nfa, n_sym);
		printf(" state %d : [%s]\n", i, eclosure[i]);
	}
	printf("\n");
}

// 입실론 클로우져 epstates생성 
void e_closure(char *epstates, char *states, char eclosure[][STATES + 1]) {
	int i;
	strcpy(epstates, states);
	for (i = 0; i < strlen(states); i++) 
		string_merge(epstates, eclosure[states[i] - '0']);
}


int nfa_to_dfa(char *nfa[][SYMBOLS], int n_nfa, int n_sym, int dfa[][SYMBOLS]) {
	int i = 0;
	int n = 1;
	char nextstate[STATES + 1];
	char temp[STATES + 1];
	int j;
	init_Eclosure(Eclosure, nfa, n_nfa, n_sym);
	e_closure(temp, "0", Eclosure);
	strcpy(StateName[0], temp);
	/* initialize start state */
	printf("Epsilon-NFA to DFA conversion\n");
	for (i = 0; i < n; i++) {
		for (j = 0; j < n_sym; j++) {
			get_next_state(nextstate, StateName[i], nfa, j);
			e_closure(temp, nextstate, Eclosure);
			dfa[i][j] = get_state_index(temp, StateName, &n);
			printf(" state %d(%4s) : %d --> state %2d(%4s)\n", i, StateName[i], j, dfa[i][j], temp);
			dfa[i][j] += 'A';
			/* 0/1/2/... --> 'A/B/C/...' */
		}
	}
	return n;

}
// final state테이블을 생성
void get_DFA_finals(char *dfinals,
	// dfa final state
	char *nfinals,
	// NFA final states 
	char stnt[][STATES + 1],	int n_dfa)
	{
	int i, j, k = 0, n = strlen(nfinals);
	for (i = 0; i < n_dfa; i++) {
		for (j = 0; j < n; j++) {
			if (strchr(stnt[i], nfinals[j])) {
				dfinals[k++] = i + 'A';
				break;
			}
		}
	}
	dfinals[k] = '\0';
}
void main() {
	load_NFA();
	show_NFA(NFA_table, NFA_states, N_sym, NFA_finals);
	DFA_states = nfa_to_dfa(NFA_table, NFA_states, N_sym, DFA_table);
	get_DFA_finals(DFA_finals, NFA_finals, StateName, DFA_states);
	show_DFA(DFA_table, DFA_states, N_sym, DFA_finals);

	system("pause");
}