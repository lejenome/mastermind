#include "core.h"

#ifndef __MM_CLI_CMD_H
#define __MM_CLI_CMD_H
typedef struct
{
	char *n;
	void (*e)(const char, const char **, mm_session *);
} cmd_t;
void cmd_quit(const char, const char **, mm_session*);
void cmd_savegame(const char, const char **, mm_session*);
void cmd_set(const char, const char **, mm_session*);
void cmd_restart(const char, const char **, mm_session*);
void cmd_score(const char, const char **, mm_session*);
void cmd_reset(const char, const char **, mm_session*);
void cmd_help(const char, const char **, mm_session*);
void cmd_account(const char, const char **, mm_session*);
#endif
