#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <locale.h>

#include "../config.h"
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
			printf(_("  InPlace: %2d, InSecret: %2d, OutSecret: %2d\n"),
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
	setlocale(LC_ALL,"");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#ifdef DEBUG
	printf("LOCALDIR: %s\n", LOCALEDIR);
#endif
	mm_session *session = mm_session_new();
	printf(_("Current Config:\n\tguesses: %d\n\tcolors: %d\n\tholes: %d\n"),
			session->config->guesses, session->config->colors,
			session->config->holes);
	do {
		printf(_("Current State:\n\tGuessed: %d\n"), session->guessed);
		printPanel(session);
		firstInput = 1;
		do {
			if(!firstInput && T != NULL) {
				printf(_("You Guesse is not valid!!\n"));
				free(T);
				T = NULL;
			} else if(!firstInput) {
				printf(_("Illigal char on you guesse!!\n"));
			}
			printf(_("Enter you guesse: (%d of [0..%d] nbre) "),
					session->config->holes,
					session->config->colors - 1);
			firstInput = 0;
		} while ((T = getCombination(session)) == NULL ||
				mm_play(session, T) != 0);
	} while (session->state == MM_PLAYING);
	printf(_("Final State:\n"));
	printPanel(session);
	printf("%s\n", (session->state == MM_SUCCESS) ?
			_("You successed :)") : _("You Failed :("));
	return 0;
}
