/* NFA --> DFA conversion program */ 
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> 
#include <string.h> 
#define STATES 256 
#define SYMBOLS 20 
int N_sym;
int NFA_state;
char *NFA_table[STATES][SYMBOLS]; 
int DFA_state;
int DFA_table[STATES][SYMBOLS];


void show_NFA(char *tab[][SYMBOLS], int nstates, int nsymbols)
{
	int i, j;
	puts("\nNFA: STATE TRANSITION TABLE"); 
	printf("     | ");

	for (i = 0; i < nsymbols; i++)
		printf(" %-6c", '0' + i);

	printf("\n"); 
	printf("-----+--");

	for (i = 0; i < nsymbols + 1; i++)
		printf("-------");

	printf("\n");

	for (i = 0; i < nstates; i++) {
		printf(" %c   | ", '0' + i); 
		for (j = 0; j < nsymbols; j++)
			printf(" %-6s", tab[i][j]);
		printf("\n");
	}
	puts("");
}

void show_DFA( int tab[][SYMBOLS], 	int nstates, int nsymbols) { 
	int i, j; 
	puts("STATE TRANSITION TABLE"); 
	printf("     | "); 
	
	for (i = 0; i < nsymbols; i++) 
		printf(" %c ", '0'+i); 
	
	printf("\n-----+--"); 
	
	for (i = 0; i < nsymbols; i++) 
		printf("-----"); 
	
	printf("\n"); 
	
	for (i = 0; i < nstates; i++) { 
		printf(" %c   | ", 'A'+i); 
		for (j = 0; j < nsymbols; j++) 
			printf(" %c ", 'A'+tab[i][j]); 
		printf("\n"); 
	} 
} 


void load_NFA() { 
	NFA_table[0][0] = "12"; 
	NFA_table[0][1] = "13"; 
	NFA_table[1][0] = "12"; 
	NFA_table[1][1] = "13"; 
	NFA_table[2][0] = "4"; 
	NFA_table[2][1] = ""; 
	NFA_table[3][0] = ""; 
	NFA_table[3][1] = "4"; 
	NFA_table[4][0] = "4"; 
	NFA_table[4][1] = "4"; 
	NFA_state = 5; 
	DFA_state = 0; 
	N_sym = 2; 
} 

void string_merge(char *s, char *t) { 
	char temp[STATES], *r=temp, *p=s; 
	while (*p && *t) { // 다음스테이트가 계속 존재하는 경우 
		if (*p == *t) { // 다음 스테이트와 현재의 스테이트가 동일할 경우
			*r++ = *p++; // 다음스테이트를 현재 스테이트로 저장
			t++; // 다음스테이트로 이동
		} 
		else if (*p < *t) { // 현재스테이트가 TEMP에 저장된 것보다 크다면 다음 스테이트들을 TEMP에 저장 
			*r++ = *p++; 
		} 
		else *r++ = *t++; // TEMP에 현재 SATE를 젖아 
	} 
	*r = '\0'; // NULL문자 입력하여 STRING의 끝을 저장 
	if (*p) 
		strcat(r, p); 
	else if (*t) 
		strcat(r, t); 
	strcpy(s, temp); // NONDETERMISTIC한 스테이트들을 MERGE하여 다음스테이트로 지정 
} 

// 다음 스테이트들을 merge시킴.
void get_next_state(char *nextstates, char *cur_states, char *nfa[STATES][SYMBOLS],	int n_nfa, int symbol) { 
	int i; 
	char temp[STATES]; 
	temp[0] = '\0'; 
	for (i = 0; i < strlen(cur_states); i++) 
		string_merge(temp, nfa[cur_states[i]-'0'][symbol]); 
	strcpy(nextstates, temp); 
} 


int get_state_index(char *state, char statename[][STATES], int *pn) { 
	int i; 
	if (!*state) 
		return -1;
	
	for (i = 0; i < *pn; i++) 
		if (!strcmp(state, statename[i])) 
			return i; 

	strcpy(statename[i], state);
	return (*pn)++; 
} 


// nfa -> dfa
int nfa_to_dfa(char *nfa[STATES][SYMBOLS], int n_nfa, int n_sym, int dfa[][SYMBOLS]) { 
	char statename[STATES][STATES]; 
	int i = 0;
	int n = 1;
	char nextstate[STATES]; 
	int j; 
	strcpy(statename[0], "0"); 
	
	for (i = 0; i < n; i++) { 
		for (j = 0; j < n_sym; j++) {
			get_next_state(nextstate, statename[i], nfa, n_nfa, j); 
			dfa[i][j] = get_state_index(nextstate, statename, &n); 
		} 
	} 
	
	return n; 
} 

void main() { 
	load_NFA(); 
	show_NFA(NFA_table, NFA_state, N_sym);
	DFA_state = nfa_to_dfa(NFA_table, NFA_state, N_sym, DFA_table); 
	show_DFA(DFA_table, DFA_state, N_sym); 
	system("pause")
} 