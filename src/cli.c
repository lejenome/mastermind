#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libintl.h>
#include <locale.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "../config.h"
#include "core.h"

mm_session *session;
char *cmds[] = {"quit", "set", "restart", "savegame", "score", "reset", "help",
	"account", NULL}; // "onnect", "server", "disconnect"

void printPanel() {
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
static char **completeCombination(const char *txt, int start, int end) {
	unsigned l =0, j, i;
	char **T = (char **)malloc(sizeof(char*) * (sizeof(cmds) +
				session->config->colors));
	char * output = NULL;
	while(*txt == ' ')
		txt++;
	T[l] = (char*)malloc(sizeof(char) * 20);
	strcpy(T[l], "");
	l++;
	if(*txt == '\0') {
		i = 0;
		while(cmds[i]) {
			T[l] = (char*)malloc(sizeof(char) * strlen(cmds[i]));
			strcpy(T[l], cmds[i]);
			i++;
			l++;
		}
	}
	if(*txt == '\0' || (*txt >= '0' && *txt <= ('0' + session->config->colors))) {
		for(j=0; j < session->config->colors; j++, l++) {
			T[l] = (char*)malloc(sizeof(char) * 2);
			sprintf(T[l], "%u", j);
		}
	}
	if(*txt >= 'a' && *txt <= 'z') {
		j = l;
		i=0;
		while(cmds[i]) {
			if(strstr(cmds[i], txt) == cmds[i]) {
				T[l] = (char*)malloc(sizeof(char) *
						strlen(cmds[i]));
				strcpy(T[l], cmds[i]);
				l++;
			}
			i++;
		}
		if (l == j+1)
			output = T[j];
	}
	if(output) {
		T[0] = (char*)malloc(sizeof(char) * strlen(output));
		strcpy(T[0], output);
		for(i=1; i < l; i++)
			free(T[i]);
		l =1;
	}
	T[l++] = NULL;
	return T;
}
unsigned char *getCombination() {
	char prmpt[200], *input, *inputorig;
	unsigned i =0, j =0, k;
	unsigned char c;
	snprintf(prmpt, 200, _("Enter you guesse: (%d of [0..%d] nbre) > "),
		session->config->holes,
		session->config->colors - 1);
	rl_attempted_completion_function = completeCombination;
	while((input = readline(prmpt)) == NULL) {};
	inputorig = input;
	char **args = (char**)malloc(sizeof(char*) * (20<session->config->holes?
			(session->config->holes +2): 20));
	unsigned argc = 0, prvSpace = 1;
	while(*input != '\0') {
		if(*input == ' ' || *input == '\t' || *input == ',') {
			if(!prvSpace)
				*input = '\0';
			prvSpace = 1;
		} else if((*input >='0' && *input <= '9') ||
				(*input >='a' && *input <= 'z')) {
			if(prvSpace)
				args[argc++] = input;
			prvSpace = 0;
		} else {
			printf(_("Error: illegal charater on the command '%c'\n"),
					*input);
			argc=0;
			break;
		}
		input++;
	}
	if(argc == 0)
		goto input_err;
	if(args[0][0] >= 'a' && args[0][0] <= 'z') {
		i = 0;
		while(cmds[i] && strstr(cmds[i], args[0]) != cmds[i])
			i++;
		if(cmds[i]) {
			printf(_("Command '%s' excuted\n"), cmds[i]);
		} else {
			printf(_("Command unsupported '%s'\n"), args[0]);
			goto input_err;
		}
		if(strstr("quit", args[0])) {
			printf(_("Bye!\n"));
			exit(0);
		} else if(strstr("savegame", args[0])) {
			printf(_("Saving session\n"));
			mm_session_save(session);
		}
		goto cmd_execed;
	}
	unsigned char *T = (unsigned char*)malloc(sizeof(unsigned char) *
			session->config->holes);
	i = 0;
	j = 0;
	k = 0;
	while((i < session->config->holes) && (args[j][k] != '\0' ||
				(++j < argc && !(k=0)))) {
		c = args[j][k++];
		if(c >= '0' && c <= '9') {
			T[i++] = c - '0';
		} else if(c == ' ' || c == ',' || c == ';' || c == '\n') {
			continue;
		} else {
			printf(_("Illigal char on you guesse!!\n"));
			goto parse_err;
		}
	}
	if(T)
		add_history(inputorig);
	else
		free(inputorig);
	free(args);
	return T;
parse_err:
	free(T);
input_err:
cmd_execed:
	free(inputorig);
	free(args);
	return NULL;
}
int main() {
	unsigned char *T = NULL;
	setlocale(LC_ALL,"");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	if(!(session = mm_session_restore()))
		session = mm_session_new();
	printf(_("Current Config:\n\tguesses: %d\n\tcolors: %d\n\tholes: %d\n"),
			session->config->guesses, session->config->colors,
			session->config->holes);
	do {
		printf(_("Current State:\n\tGuessed: %d\n"), session->guessed);
		printPanel();
		while ((T = getCombination()) == NULL ||
				mm_play(session, T) != 0) {
			if(T != NULL) {
				printf(_("You Guesse is not valid!!\n"));
				free(T);
				T = NULL;
			}
		}
	} while (session->state == MM_PLAYING);
	printf(_("Final State:\n"));
	printPanel();
	printf("%s\n", (session->state == MM_SUCCESS) ?
			_("You successed :)") : _("You Failed :("));
	return 0;
}
