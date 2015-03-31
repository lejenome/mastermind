#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"
#include "core.h"
#include "cli-cmd.h"

#define LEN(a) (sizeof(a) / sizeof(a[0]))

int cmd_quit(const char argc, const char **argv, mm_session *session)
{
	mm_session_exit(session);
	printf(_("Bye!\n"));
	exit(0);
	return 0;
}

int cmd_savegame(const char argc, const char **argv, mm_session *session)
{
	printf(_("Saving session\n"));
	mm_session_save(session);
	return 0;
}
int cmd_set(const char argc, const char **argv, mm_session *session)
{
	mm_conf_t *conf;
	switch (argc) {
	case 1:
		printf(_("Global configs:\n"));
		for (conf = mm_confs; conf < mm_confs + LEN(mm_confs); conf++)
			switch (conf->type) {
			case MM_CONF_INT:
				printf("\t%s = %d\n", conf->nbre.name,
				       conf->nbre.val);
				break;
			case MM_CONF_BOOL:
				printf("\t%s = %u\n", conf->bool.name,
				       conf->bool.val);
				break;
			case MM_CONF_STR:
				printf("\t%s = %s\n", conf->str.name,
				       conf->str.val);
				break;
			}
		printf(_("Session configs:\n"));
		printf("\tguesses = %d\n\tcolors = %d\n\tholes = %d\n\tremise "
		       "= %d\n",
		       session->config->guesses, session->config->colors,
		       session->config->holes, session->config->remise);
		break;
	case 2:
		conf = mm_confs;
		while (conf < mm_confs + LEN(mm_confs) &&
		       strcmp(conf->str.name, argv[1]) != 0)
			conf++;
		if (conf < mm_confs + LEN(mm_confs))
			switch (conf->type) {
			case MM_CONF_INT:
				printf("\t%s = %d\n", conf->nbre.name,
				       conf->nbre.val);
				break;
			case MM_CONF_BOOL:
				printf("\t%s = %u\n", conf->bool.name,
				       conf->bool.val);
				break;
			case MM_CONF_STR:
				printf("\t%s = %s\n", conf->str.name,
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
		return -1;
	}
	return 0;
}
int cmd_restart(const char argc, const char **argv, mm_session *s)
{
	//  FIXME: find better and standard way to reset session object
	extern mm_session *session;
	mm_session_free(session);
	session = mm_session_new(NULL);
	return 1;
}
int cmd_help(const char argc, const char **argv, mm_session *session)
{
	printf(_("About " PACKAGE
		 ": \nIs a two players logical game(encoder,decoder)"
		 "encoder chose one combination compose from four to six color"
		 "decoder try to find this combination by trying to find color "
		 "position \n"
		 "RTFM: "
		 "http://en.wikipedia.org/wiki/"
		 "Mastermind_%%28board_game%%29#Gameplay_and_rules\n"));
	return 0;
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
	return 0;
}
int cmd_account(const char argc, const char **argv, mm_session *session)
{
	if (argc == 1) {
		printf(_("current account: %s\n"), session->account);
		return 0;
	} else if (argc == 2) {
		const char *args[3];
		args[0] = "set";
		args[1] = "account";
		args[2] = argv[1];
		cmd_set(3, args, session);
		cmd_restart(1, (const char * [1]){"restart"}, session);
		return 1;
	}
	return -1;
}
int cmd_version(const char argc, const char **argv, mm_session *session)
{
	printf("%s - v%s\nSite: %s\n", PACKAGE, PROGRAM_VERSION, PROGRAM_URL);
	return 0;
}
