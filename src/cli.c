#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "util.h"
#include "cli-cmd.h"
#include "core.h"

#ifdef MM_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif // MM_READLINE

/*!
 * \file cli.c
 * \brief command line interface implemetation of mastermind
 */

mm_session *session;
/// draw session panel
void printPanel()
{
	unsigned i, j;
	for (i = 0; i < session->config->guesses; i++) {
		putchar('+');
		for (j = 0; j < session->config->holes; j++)
			printf("---+");
		printf("  +-----+-----+-----+\n|");
		for (j = 0; j < session->config->holes; j++)
			if (i < session->guessed)
				printf(" %d |",
				       session->panel[i].combination[j]);
			else
				printf("   |");
		if (i < session->guessed)
			printf(_("  | Y%2d | A%2d | N%2d |\n"),

			       session->panel[i].right_pos,
			       session->panel[i].wrong_pos,
			       session->config->holes -
				   session->panel[i].right_pos -
				   session->panel[i].wrong_pos);
		else
			printf("  |     |     |     |\n");
	}
	putchar('+');
	for (j = 0; j < session->config->holes; j++)
		printf("---+");
	printf("  +-----+-----+-----+");
	putchar('\n');
}
#ifdef MM_READLINE
/*! Tab button click handler
 * @param txt	current buffer
 * @param start	buffer start position
 * @param end	bufffer end postion
 * @return	array of guessed completions
 */
static char **completeInput(const char *txt, int start, int end)
{
	if (rl_point != rl_end) // we support completing only the last argument
		return (char **)NULL;
	unsigned l = 0, j, i, argc;
	char *output = NULL; // value of the only string to complete else NULL
	char **args;	 // input buffer arguments
	cmd_t *cmd, *cmpltCmd = NULL;
	mm_conf_t *conf, *cmpltCnf = NULL;
	// get cmds array length (without counting NULL element)
	for (i = 0; cmds[i].e != NULL; i++) {
	};
	// array of completion strings
	char **T =
	    (char **)malloc(sizeof(char *) * (i + session->config->colors + 2));
	T[l++] = strdup("");
	args = parseBuf(rl_line_buffer, &argc);
	if (argc == 0) // if not args on buffer, add all commands to T
		for (cmd = cmds; cmd->n != NULL; cmd++, l++)
			T[l] = strdup(cmd->n);
	// if no args on buffer or first arg first char is number, add all
	// possible numbers
	if (argc == 0 || (args[0][0] >= '0' &&
			  args[0][0] < ('0' + session->config->colors))) {
		j = 0;
		// check if input buffer only contains valid numbers
		for (i = 0; i < strlen(rl_line_buffer); i++)
			if (rl_line_buffer[i] >= '0' &&
			    rl_line_buffer[i] < '0' + session->config->colors)
				j++;
			else if (strchr(IFS, rl_line_buffer[i]) == NULL)
				goto no_more;
		if (j >= session->config->holes)
			goto no_more;
		// add all possible valid numbers to the completion array
		for (j = 0; j < session->config->colors; j++, l++) {
			T[l] = (char *)malloc(sizeof(char) * 2);
			sprintf(T[l], "%u", j);
		}
	}
	// if only one arg is in the input buffer and it's a command
	if (argc == 1 && args[0][0] >= 'a' && args[0][0] <= 'z') {
		j = l;
		for (cmd = cmds; cmd->n != NULL; cmd++) {
			if (strstr(cmd->n, args[0]) == cmd->n) {
				if (strcmp(cmd->n, args[0]) == 0)
					cmpltCmd = cmd;
				else
					T[l++] = strdup(cmd->n);
			}
		}
		if (l == j + 1)
			output = T[j];
	}
	if (cmpltCmd && strcmp(cmpltCmd->n, "set") == 0) {
		for (conf = mm_confs; conf < mm_confs + MM_POS_LEN; conf++, l++)
			T[l] = strdup(conf->common.name);
	}
	if (cmpltCmd && strcmp(cmpltCmd->n, "account") == 0)
		output = strdup(session->account);
	if (argc == 2 && strcmp(args[0], "set") == 0) {
		j = l;
		for (conf = mm_confs; conf < mm_confs + MM_POS_LEN; conf++) {
			if (strstr(conf->common.name, args[1]) ==
			    conf->common.name) {
				if (strcmp(conf->common.name, args[1]) == 0)
					cmpltCnf = conf;
				else
					T[l++] = strdup(conf->common.name);
			}
		}
		if (l == j + 1)
			output = T[j];
		if (cmpltCnf) {
			output = (char *)malloc(sizeof(char) * 20);
			switch (cmpltCnf->type) {
			case MM_CONF_INT:
				sprintf(output, "%d", cmpltCnf->nbre.val);
				break;
			case MM_CONF_BOOL:
				sprintf(output, "%u", cmpltCnf->bool.val);
				break;
			case MM_CONF_STR:
				strcpy(output, cmpltCnf->str.val);
				break;
			}
		}
	}
no_more:
	if (output) {
		T[0] = strdup(output);
		for (i = 1; i < l; i++)
			free(T[i]);
		l = 1;
		if (cmpltCnf)
			free(output);
	} else if (l == 1 && rl_line_buffer[rl_point - 1] != ' ') {
		free(T[0]);
		free(T);
		T = (char **)NULL;
	}
	if (T)
		T[l++] = NULL;
	while (argc--)
		free(args[argc]);
	free(args);
	return T;
}
#endif // MM_READLINE

/*! get guessed combination and handle input buffer commands
 * @return
 *	-1 : input error, redo (do not redraw table)
 *	0 : seccess input, redo if mm_play(T) does not success (do not redraw
 *      table) or next (redraw table)
 * 	1  : cmd input, redo (do not redo table)
 * 	2  : cmd input, next (redraw table)
 */
int parseInput(uint8_t *T)
{
	unsigned ret = -1;
	char prmpt[200], *input, **args;
	unsigned i, j, argc;
	cmd_t *cmd;
	snprintf(prmpt, 200, _("Enter you guesse: (%d of [0..%d] nbre) > "),
		 session->config->holes, session->config->colors - 1);
#ifdef MM_READLINE
	rl_attempted_completion_function = completeInput;
	do {
		input = readline(prmpt);
	} while (input == NULL);
#else
	printf("%s", prmpt);
	input = (char *)malloc(sizeof(char) * 4096);
	fgets(input, 4095, stdin);
#endif // MM_READLINE
	args = parseBuf(input, &argc);
	if (argc == 0)
		goto input_err;
	if (args[0][0] >= 'a' && args[0][0] <= 'z') {
		// find first command matching the first argument, and excute it
		cmd = cmds;
		while (cmd->n != NULL && strstr(cmd->n, args[0]) != cmd->n)
			cmd++;
		if (cmd->n != NULL) {
			ret = cmd->e(argc, (const char **)args, session);
#ifdef MM_READLINE
			add_history(strdup(input));
#endif // MM_READLINE
			if (ret & MM_CMD_NEW_SESSION)
				session = mm_session_new();
			ret = (ret & MM_CMD_REDESIGN) ? 2 : 1;
		} else {
			printf(_("Command unsupported '%s'\n"), args[0]);
			goto input_err;
		}
		goto cmd_execed;
	}
	i = 0;
	j = 0;
	// get combination from input buffer
	while (i < session->config->holes && input[j] != '\0') {
		if (input[j] >= '0' && input[j] <= '9') {
			T[i++] = input[j] - '0';
		} else if (strchr(IFS, input[j]) == NULL) {
			printf(_("Illigal char on you guesse!!\n"));
			goto parse_err;
		}
		j++;
	}
#ifdef MM_READLINE
	if (T)
		add_history(input);
	else
#endif // MM_READLINE
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
int main(int argc, char *argv[])
{
	uint8_t *T = NULL, c;
	unsigned ret;
#ifdef MM_LOCALE
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#endif // MM_LOCALE
	session = mm_session_restore();
	if (session == NULL)
		session = mm_session_new();
	else
		printf(_("Restoring old session\n"));
#ifdef MM_GETOPT
	ret = execArgs(argc, argv, session);
	if (ret & MM_CMD_ERROR)
		exit(EXIT_FAILURE);
	else if (ret & MM_CMD_OPT_EXIT)
		goto quit;
	if (ret & MM_CMD_NEW_SESSION)
		session = mm_session_new();
#endif // MM_GETOPT
	mm_cmd_mode = MM_CMD_MODE_CLI;
	do {
		printf(_("Current Config:\n\tguesses: %d\n\tcolors: "
			 "%d\n\tholes: %d\n\tremise: %d\n"),
		       session->config->guesses, session->config->colors,
		       session->config->holes, session->config->remise);
		do {
			printPanel();
			T = (uint8_t *)malloc(sizeof(uint8_t) *
					      session->config->holes);
			while ((ret = parseInput(T)) == -1 || ret == 1 ||
			       (ret == 0 && mm_play(session, T) != 0)) {
				if (ret == 0) {
					printf(
					    _("Your Guess is not valid!!\n"));
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
		if (session->state == MM_SUCCESS)
			printf(_("Your score is: %ld\n"), mm_score(session));
		printf(_("Secret Key is: "));
		for (c = 0; c < session->config->holes; c++)
			putchar('0' + session->secret->val[c]);
		putchar('\n');
		mm_session_free(session);
		session = mm_session_new();
		printf(_("restart? (Y/N): "));
		scanf("%c", &c);
	} while (c == 'y' || c == 'Y');
quit:
	mm_session_free(session);
	exit(EXIT_SUCCESS);
}
