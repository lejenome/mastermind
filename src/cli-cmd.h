#include "core.h"

#ifndef __MM_CLI_CMD_H
#define __MM_CLI_CMD_H

#define MM_CMD_SUCCESS 0x000      // cmd succeded
#define MM_CMD_ERROR 0x0001       // their was an error
#define MM_CMD_NEW_SESSION 0x0010 // new session needed
#define MM_CMD_REDESIGN 0x0100    // redesign the panel is needed
#define MM_CMD_OPT_EXIT                                                        \
	0x1000 // just exit the program if cmd executed from option
typedef struct {
	char *n;					   // name of command
	int (*e)(const char, const char **, mm_session *); // function to excute
	char s;    // short arg name else 0
	char *l;   // long arg name or NULL
	uint8_t a; // nbre of options arg accpet
} cmd_t;
int cmd_quit(const char, const char **, mm_session *);
int cmd_savegame(const char, const char **, mm_session *);
int cmd_set(const char, const char **, mm_session *);
int cmd_restart(const char, const char **, mm_session *);
int cmd_score(const char, const char **, mm_session *);
int cmd_help(const char, const char **, mm_session *);
int cmd_account(const char, const char **, mm_session *);
int cmd_version(const char, const char **, mm_session *);
#endif
