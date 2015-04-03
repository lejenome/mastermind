#include "core.h"

#ifndef __MM_CLI_CMD_H
#define __MM_CLI_CMD_H
// cmds return values
#define MM_CMD_SUCCESS 0x00     // cmd succeded
#define MM_CMD_ERROR 0x01       // their was an error
#define MM_CMD_NEW_SESSION 0x02 // new session needed
#define MM_CMD_REDESIGN 0x04    // redesign the panel is needed
#define MM_CMD_OPT_EXIT                                                        \
	0x08 // just exit the program if cmd executed from option
// mode cmds are executed on (to modify the output style)
#define MM_CMD_MODE_OPT 0
#define MM_CMD_MODE_CLI 1
#define MM_CMD_MODE_GUI 2
uint8_t
    mm_cmd_mode; // set this var to current exec mode (default: MM_CMD_MODE_OPT)
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
#ifndef DISABLE_GETOPT
int execArgs(int argc, char *argv[], cmd_t *cmds, size_t len, mm_session *);
#endif // DISABLE_GETOPT
#endif // __MM_CLI_CMD_H
