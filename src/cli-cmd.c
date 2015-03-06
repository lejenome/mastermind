#include <stdio.h>
#include <stdlib.h>
#include "../config.h"
#include "core.h"
#include "cli-cmd.h"

void cmd_quit(const char argc, const char **argv, mm_session* session)
{
	printf(_("Bye!\n"));
	exit(0);
}

void cmd_savegame(const char argc, const char **argv, mm_session *session)
{
	printf(_("Saving session\n"));
	mm_session_save(session);
}
void cmd_set(const char argc, const char **argv, mm_session *session) {}
void cmd_restart(const char argc, const char **argv, mm_session *session) {}
void cmd_score(const char argc, const char **argv, mm_session *session) {}
void cmd_reset(const char argc, const char **argv, mm_session *session) {}
void cmd_help(const char argc, const char **argv, mm_session *session) {}
void cmd_account(const char argc, const char **argv, mm_session *session) {}
