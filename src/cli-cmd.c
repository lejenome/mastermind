#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../config.h"
#include "core.h"
#include "cli-cmd.h"

#define LEN(a) (sizeof(a) / sizeof(a[0]))

int cmd_quit(const char argc, const char **argv, mm_session *session)
{
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
		printf("Global configs:\n");
		for (conf = mm_confs; conf < mm_confs + LEN(mm_confs); conf++)
			printf("\t%s = %d\n", conf->nm, conf->val);
		printf("Session configs:\n");
		printf("\tguesses = %d\n\tcolors = %d\n\tholes = %d\n",
		       session->config->guesses, session->config->colors,
		       session->config->holes);
		break;
	case 2:
		conf = mm_confs;
		while (conf < mm_confs + LEN(mm_confs) &&
		       strcmp(conf->nm, argv[1]) != 0)
			conf++;
		if (conf < mm_confs + LEN(mm_confs))
			printf(_("Global config: %s = %d\n"), conf->nm,
			       conf->val);
		else
			printf(_("Global config not supported!!\n"));
		break;
	case 3:
		mm_config_set(argv[1], atoi(argv[2]));
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
	extern mm_session *session;
	mm_session_free(session);
	session = mm_session_new();
	return 1;
}
int cmd_help(const char argc, const char **argv, mm_session *session)
{
	printf("Help me please!!!\n");
	return 0;
}
int cmd_score(const char argc, const char **argv, mm_session *session)
{
	printf("You're the best!\nNo, I'm just kidding!!!\n");
	return 0;
}
int cmd_reset(const char argc, const char **argv, mm_session *session)
{
	printf("sudo rm -rf /\n");
	return 0;
}
int cmd_account(const char argc, const char **argv, mm_session *session)
{
	printf("sudo useradd --home-dir=/dev/null --gid root --shell "
	       "/dev/rondom\n");
	return 0;
}
