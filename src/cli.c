#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libintl.h>
#include <locale.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "../config.h"
#include "cli-cmd.h"
#include "core.h"

#define LEN(a) (sizeof(a) / sizeof(a[0]))
mm_session *session;
cmd_t cmds[] = {
    {.n = "quit", .e = cmd_quit},
    {.n = "set", .e = cmd_set},
    {.n = "restart", .e = cmd_restart},
    {.n = "savegame", .e = cmd_savegame},
    {.n = "score", .e = cmd_score},
    {.n = "reset", .e = cmd_reset},
    {.n = "help", .e = cmd_help},
    {.n = "account", .e = cmd_account},
}; // "connect", "server", "disconnect", "version"
void printPanel()
{
	unsigned i, j;
	for (i = 0; i < session->config->guesses; i++) {
		putchar('+');
		for (j = 0; j < session->config->holes; j++)
			printf("---+");
		printf("\n|");
		for (j = 0; j < session->config->holes; j++)
			if (i < session->guessed)
				printf(" %d |",
				       session->panel[i].combination[j]);
			else
				printf("   |");
		if (i < session->guessed)
			printf(_("  InPlace: %2d, InSecret: %2d, OutSecret: "
				 "%2d\n"),
			       session->panel[i].inplace,
			       session->panel[i].insecret -
				   session->panel[i].inplace,
			       session->config->holes -
				   session->panel[i].insecret);
		else
			putchar('\n');
	}
	putchar('+');
	for (j = 0; j < session->config->holes; j++)
		printf("---+");
	putchar('\n');
}
unsigned parseBuf(char *buf, char **args)
{
	char *start = NULL;
	unsigned argc = 0;
	while (*buf != '\0') {
		if (*buf == ' ' || *buf == '\t' || *buf == ',') {
			if (start) {
				args[argc] = (char *)malloc(sizeof(char) *
							    (buf - start + 1));
				strncpy(args[argc], start, buf - start);
				args[argc][buf - start] = '\0';
				argc++;
				start = NULL;
			}
		} else if ((*buf >= '0' && *buf <= '9') ||
			   (*buf >= 'a' && *buf <= 'z')) {
			if (start == NULL)
				start = buf;
		} else {
			printf(
			    _("Error: illegal charater on the command '%c'\n"),
			    *buf);
			argc = 0;
			break;
		}
		buf++;
	}
	if (start) {
		args[argc] = (char *)malloc(sizeof(char) * (buf - start + 1));
		strncpy(args[argc], start, buf - start);
		args[argc][buf - start] = '\0';
		argc++;
	}
	return argc;
}
static char **completeCombination(const char *txt, int start, int end)
{
	unsigned l = 0, j, i;
	cmd_t *cmd;
	char **T = (char **)malloc(sizeof(char *) *
				   (LEN(cmds) + session->config->colors + 2));
	char *output = NULL, *buf;
	buf = (char *)malloc(sizeof(char) * (strlen(rl_line_buffer) + 1));
	strcpy(buf, rl_line_buffer);
	while (*buf == ' ')
		buf++;
	T[l] = (char *)malloc(sizeof(char) *
			      (LEN(cmds) + session->config->holes + 2));
	strcpy(T[l], "");
	l++;
	if (*buf == '\0') {
		for (cmd = cmds; cmd < cmds + LEN(cmds); cmd++, l++) {
			T[l] =
			    (char *)malloc(sizeof(char) * (strlen(cmd->n) + 1));
			strcpy(T[l], cmd->n);
		}
	}
	if (*buf == '\0' ||
	    (*buf >= '0' && *buf <= ('0' + session->config->colors))) {
		for (j = 0; j < session->config->colors; j++, l++) {
			T[l] = (char *)malloc(sizeof(char) * 2);
			sprintf(T[l], "%u", j);
		}
	}
	if (*buf >= 'a' && *buf <= 'z') {
		j = l;
		for (cmd = cmds; cmd < cmds + LEN(cmds); cmd++) {
			if (strstr(cmd->n, buf) == cmd->n) {
				T[l] = (char *)malloc(sizeof(char) *
						      (strlen(cmd->n) + 1));
				strcpy(T[l], cmd->n);
				l++;
			}
		}
		if (l == j + 1)
			output = T[j];
	}
	if (output) {
		T[0] = (char *)malloc(sizeof(char) * (strlen(output) + 1));
		strcpy(T[0], output);
		for (i = 1; i < l; i++)
			free(T[i]);
		l = 1;
	}
	free(buf);
	T[l++] = NULL;
	return T;
}
/* return:
 * -1 : input error, redo (do not redraw table)
 *  0 : seccess input, redo if mm_play(T) does not success (do not redraw
 *      table) or next (redraw table)
 * 1  : cmd input, redo (do not redo table)
 * 2  : cmd input, next (redraw table)
 * */
int getCombination(unsigned char *T)
{
	unsigned ret = -1;
	char prmpt[200], *input;
	unsigned i = 0, j = 0, k, argc;
	unsigned char c;
	cmd_t *cmd;
	snprintf(prmpt, 200, _("Enter you guesse: (%d of [0..%d] nbre) > "),
		 session->config->holes, session->config->colors - 1);
	rl_attempted_completion_function = completeCombination;
	while ((input = readline(prmpt)) == NULL) {
	};
	char **args = (char **)malloc(
	    sizeof(char *) *
	    (20 < session->config->holes ? (session->config->holes + 2) : 20));
	argc = parseBuf(input, args);
	if (argc == 0)
		goto input_err;
	if (args[0][0] >= 'a' && args[0][0] <= 'z') {
		cmd = cmds;
		while (cmd < cmds + LEN(cmds) &&
		       strstr(cmd->n, args[0]) != cmd->n)
			cmd++;
		if (cmd < cmds + LEN(cmds)) {
			ret = cmd->e(argc, (const char **)args, session);
			char *cmd_txt =
			    (char *)malloc(sizeof(char) * (strlen(input) + 1));
			strcpy(cmd_txt, input);
			add_history(cmd_txt);
			ret = ret == 1 ? 2 : 1;
		} else {
			printf(_("Command unsupported '%s'\n"), args[0]);
			goto input_err;
		}
		goto cmd_execed;
	}
	i = 0;
	j = 0;
	k = 0;
	while ((i < session->config->holes) &&
	       (args[j][k] != '\0' || (++j < argc && !(k = 0)))) {
		c = args[j][k++];
		if (c >= '0' && c <= '9') {
			T[i++] = c - '0';
		} else if (c == ' ' || c == ',' || c == ';' || c == '\n') {
			continue;
		} else {
			printf(_("Illigal char on you guesse!!\n"));
			goto parse_err;
		}
	}
	if (T)
		add_history(input);
	else
		free(input);
	while (argc--)
		free(args[argc]);
	free(args);
	return 0;
parse_err:
input_err:
cmd_execed:
	free(input);
	while (argc--)
		free(args[argc]);
	free(args);
	return ret;
}
int main()
{
	unsigned char *T = NULL, rst;
	unsigned ret;
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	session = mm_session_restore();
	do {
		if (!session)
			session = mm_session_new();
		session = mm_session_new();
		printf(_("Current Config:\n\tguesses: %d\n\tcolors: "
			 "%d\n\tholes: %d\n"),
		       session->config->guesses, session->config->colors,
		       session->config->holes);
		do {
			printf(_("Current State:\n\tGuessed: %d\n"),
			       session->guessed);
			printPanel();
			T = (unsigned char *)malloc(sizeof(unsigned char) *
						    session->config->holes);
			while ((ret = getCombination(T)) == -1 || ret == 1 ||
			       (ret == 0 && mm_play(session, T) != 0)) {
				if (ret == 0) {
					printf(
					    _("You Guesse is not valid!!\n"));
				}
			}
			if (ret != 0)
				free(T);
		} while (session->state == MM_PLAYING ||
			 session->state == MM_NEW);
		printf(_("Final State:\n"));
		printPanel();
		printf("%s\n", (session->state == MM_SUCCESS)
				   ? _("You successed :)")
				   : _("You Failed :("));
		mm_session_free(session);
		printf(_("restart? (Y/N): "));
		scanf("%c", &rst);
	} while (rst == 'y' || rst == 'Y');
	return 0;
}
