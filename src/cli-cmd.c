#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"
#include "core.h"
#include "cli-cmd.h"
#ifndef DISABLE_GETOPT
#include <getopt.h>
#endif
/* \file cli-cmd.c
 * \brief commands unctions handlers
 */

uint8_t mm_cmd_mode = MM_CMD_MODE_OPT;
#ifndef DISABLE_GETOPT
/*! parse and exec command line arguments
 * @param argc	main function argc param
 * @param argv	main function argv param
 * @param cmds	list of commands objects
 * @param len	length of cmds array
 * @return	excuted commands return values
*/
int execArgs(int argc, char *argv[], cmd_t *cmds, size_t len,
	     mm_session *session)
{
	if (argc == 1)
		return MM_CMD_SUCCESS;
	char c, *args;
	struct option *ops;
	unsigned i = 0, j, ret = MM_CMD_SUCCESS;
	cmd_t *cmd;
	mm_cmd_mode = MM_CMD_MODE_OPT;
	args = (char *)malloc(sizeof(char) * (len * 3 + 1));
	ops = (struct option *)malloc(sizeof(struct option) * (len + 1));
	for (cmd = cmds; cmd < cmds + len; cmd++) {
		if (cmd->s == 0)
			continue;
		args[i * 3] = cmd->s;
		args[i * 3 + 1] = ':';
		args[i * 3 + 2] = ':';
		ops[i] = (struct option){.name = cmd->n,
					 .has_arg = optional_argument,
					 .flag = NULL,
					 .val = cmd->s};
		i++;
	}
	ops[i] = (struct option){NULL, 0, NULL, 0};
	args[i * 3] = '\0';
	while ((c = getopt_long(argc, argv, args, ops, NULL)) != -1) {
		cmd = cmds;
		if (c == '?' || c == ':')
			return MM_CMD_ERROR;
		while (cmd < cmds + len && c != cmd->s) {
			cmd++;
		}
		if (cmd == cmds + len)
			return MM_CMD_ERROR;
		i = 1;
		const char **a =
		    (const char **)malloc(sizeof(char) * (cmds->a + 1));
		a[0] = cmd->n;
		if (cmd->a > 0 && optarg)
			a[i++] = optarg;
		j = optind;
		while (i <= cmd->a && j < argc && argv[j][0] != '-')
			a[i++] = argv[j++];
		ret |= cmd->e(i, a, session);
		free(a);
	}

	free(args);
	free(ops);
	return ret;
}
#endif // DISABLE_GETOPT
int cmd_quit(const char argc, const char **argv, mm_session *session)
{
	mm_session_exit(session);
	printf(_("Bye!\n"));
	exit(EXIT_SUCCESS);
	return MM_CMD_SUCCESS | MM_CMD_OPT_EXIT;
}

int cmd_savegame(const char argc, const char **argv, mm_session *session)
{
	printf(_("Saving session\n"));
	mm_session_save(session);
	return MM_CMD_SUCCESS;
}
int cmd_set(const char argc, const char **argv, mm_session *session)
{
	mm_conf_t *conf;
	switch (argc) {
	case 1:
		if (mm_cmd_mode != MM_CMD_MODE_OPT)
			printf(_("Global configs:\n"));
		for (conf = mm_confs; conf < mm_confs + LEN(mm_confs); conf++)
			switch (conf->type) {
			case MM_CONF_INT:
				printf("%s%s = %d\n",
				       mm_cmd_mode == MM_CMD_MODE_OPT ? ""
								      : "\t",
				       conf->nbre.name, conf->nbre.val);
				break;
			case MM_CONF_BOOL:
				printf("%s%s = %u\n",
				       mm_cmd_mode == MM_CMD_MODE_OPT ? ""
								      : "\t",
				       conf->bool.name, conf->bool.val);
				break;
			case MM_CONF_STR:
				printf("%s%s = %s\n",
				       mm_cmd_mode == MM_CMD_MODE_OPT ? ""
								      : "\t",
				       conf->str.name, conf->str.val);
				break;
			}
		if (mm_cmd_mode != MM_CMD_MODE_OPT) {
			printf(_("Session configs:\n"));
			printf("\tguesses = %d\n\tcolors = %d\n\tholes = "
			       "%d\n\tremise "
			       "= %d\n",
			       session->config->guesses,
			       session->config->colors, session->config->holes,
			       session->config->remise);
		}
		break;
	case 2:
		conf = mm_confs;
		while (conf < mm_confs + LEN(mm_confs) &&
		       strcmp(conf->str.name, argv[1]) != 0)
			conf++;
		if (conf < mm_confs + LEN(mm_confs))
			switch (conf->type) {
			case MM_CONF_INT:
				printf("%s = %d\n", conf->nbre.name,
				       conf->nbre.val);
				break;
			case MM_CONF_BOOL:
				printf("%s = %u\n", conf->bool.name,
				       conf->bool.val);
				break;
			case MM_CONF_STR:
				printf("%s = %s\n", conf->str.name,
				       conf->str.val);
				break;
			}
		else
			printf(_("Global config not supported!!\n"));
		break;
	case 3:
		mm_config_set(argv[1], argv[2]);
		break;
	default:
		printf(_("Command format error!\n"
			 "set [config_name [config_value]]\n"));
		return MM_CMD_ERROR | MM_CMD_OPT_EXIT;
	}
	return MM_CMD_SUCCESS | MM_CMD_OPT_EXIT;
}
int cmd_restart(const char argc, const char **argv, mm_session *session)
{
	//  FIXME: find better and standard way to reset session object
	mm_session_free(session);
	return MM_CMD_SUCCESS | MM_CMD_NEW_SESSION | MM_CMD_REDESIGN;
}
int cmd_help(const char argc, const char **argv, mm_session *session)
{
	printf(_("About " PROGRAM_NAME
		 ": \nIs a two players logical game(encoder,decoder)"
		 "encoder chose one combination compose from four to six color"
		 "decoder try to find this combination by trying to find color "
		 "position \n"
		 "RTFM: "
		 "http://en.wikipedia.org/wiki/"
		 "Mastermind_%%28board_game%%29#Gameplay_and_rules\n"));
	return MM_CMD_SUCCESS | MM_CMD_OPT_EXIT;
}
int cmd_score(const char argc, const char **argv, mm_session *session)
{
	unsigned i;
	const mm_scores_t *scores = mm_scores_get();
	if (scores->len == 0)
		printf(_("No score yet!\n"));
	for (i = 0; i < scores->len; i++)
		printf("%-2d) %-15ld %s\n", i, scores->T[i].score,
		       scores->T[i].account);
	return MM_CMD_SUCCESS | MM_CMD_OPT_EXIT;
}
int cmd_account(const char argc, const char **argv, mm_session *session)
{
	if (argc == 1) {
		printf(_("current account: %s\n"), session->account);
		return MM_CMD_SUCCESS | MM_CMD_OPT_EXIT;
	} else if (argc == 2) {
		const char *args[3];
		args[0] = "set";
		args[1] = "account";
		args[2] = argv[1];
		return cmd_set(3, args, session) |
		       cmd_restart(1, (const char * [1]){"restart"}, session);
	}
	return MM_CMD_ERROR | MM_CMD_OPT_EXIT;
}
int cmd_version(const char argc, const char **argv, mm_session *session)
{
	printf("%s - v%s\nSite: %s\n", PROGRAM_NAME, PROGRAM_VERSION,
	       PROGRAM_URL);
	return MM_CMD_SUCCESS | MM_CMD_OPT_EXIT;
}
