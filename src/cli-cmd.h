#include "core.h"

#ifndef __MM_CLI_CMD_H
#define __MM_CLI_CMD_H
typedef struct
{
	char *n;
	int (*e)(const char, const char **, mm_session *);
} cmd_t;
int cmd_quit(const char, const char **, mm_session *);
int cmd_savegame(const char, const char **, mm_session *);
int cmd_set(const char, const char **, mm_session *);
int cmd_restart(const char, const char **, mm_session *);
int cmd_score(const char, const char **, mm_session *);
int cmd_reset(const char, const char **, mm_session *);
int cmd_help(const char, const char **, mm_session *);
int cmd_account(const char, const char **, mm_session *);
int cmd_version(const char, const char **, mm_session *);
#endif
