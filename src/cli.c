#include <stdio.h>
#include <stdlib.h>
#include "core.h"

void printPanel(mm_session *session) {
	unsigned i, j;
	for(i = 0; i < session->config->guesses; i++) {
		putchar('+');
		for(j =0; j < session->config->holes; j++)
			printf("---+");
		printf("\n|");
		for(j=0; j < session->config->holes; j++)
			if(i < session->guessed)
				printf(" %d |", session->panel[i].combination[j]);
			else
				printf("   |");
		if(i < session->guessed)
			printf("  InPlace: %2d, InSecret: %2d, OutSecret: %2d\n",
			      session->panel[i].inplace,
			      session->panel[i].insecret - session->panel[i].inplace,
			      session->config->holes - session->panel[i].insecret);
		else
			putchar('\n');
	}
	putchar('+');
	for(j = 0; j < session->config->holes; j++)
		printf("---+");
	putchar('\n');
}
unsigned char *getCombination(mm_session *session) {
	unsigned i =0;
	unsigned char c;
	unsigned char *T = (unsigned char*)malloc(sizeof(unsigned char) *
			session->config->holes);
	while(i < session->config->holes) {
		c = getchar();
		if(c >= '0' && c <= '9') {
			T[i++] = c - '0';
		} else if(c == ' ' || c == ',' || c == ';' || c == '\n') {
			continue;
		} else {
			free(T);
			T = NULL;
			break;
		}
	}
	while(c != '\n')
		c = getchar();
	fflush(stdin);
	return T;
}
int main() {
	unsigned char *T = NULL, firstInput;
	mm_session *session = mm_session_new();
	printf("Current Config:\n\tguesses: %d\n\tcolors: %d\n\tholes: %d\n",
			session->config->guesses, session->config->colors,
			session->config->holes);
	do {
		printf("Current State:\n\tGuessed: %d\n", session->guessed);
		printPanel(session);
		firstInput = 1;
		do {
			if(!firstInput && T != NULL) {
				printf("You Guesse is not valid!!\n");
				free(T);
				T = NULL;
			} else if(!firstInput) {
				printf("Illigal char on you guesse!!\n");
			}
			printf("Enter you guesse: (%d of [0..%d] nbre) ",
					session->config->holes,
					session->config->colors - 1);
			firstInput = 0;
		} while ((T = getCombination(session)) == NULL ||
				mm_play(session, T) != 0);
	} while (session->state == MM_PLAYING);
	printf("Final State:\n");
	printPanel(session);
	printf("%s\n", (session->state == MM_SUCCESS) ?
			"You successed :)" : "You Failed :(");
	return 0;
}
