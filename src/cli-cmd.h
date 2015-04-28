#ifndef __MM_CLI_CMD_H
#define __MM_CLI_CMD_H
#include "core.h"

/*!
 * \file cli-cmd.h
 * \brief commands functions handlers
 */

/// cmds return values
enum { MM_CMD_SUCCESS = 0,	    ///< cmd succeded
       MM_CMD_ERROR = (1 << 0),       ///< their was an error
       MM_CMD_NEW_SESSION = (1 << 1), ///< new session needed
       MM_CMD_REDESIGN = (1 << 2),    ///< redesign the panel is needed
       MM_CMD_OPT_EXIT =
	   (1 << 3) ///< just exit the program if cmd executed from option
};
/// mode cmds are executed on (to modify the output style)
enum { MM_CMD_MODE_OPT, ///< command executed on option mode
       MM_CMD_MODE_CLI, ///< command executed on interactive cli mode
       MM_CMD_MODE_GUI  ///< command executed on GUI interface mode
};
// set this var to current exec mode (default: MM_CMD_MODE_OPT)
uint8_t mm_cmd_mode;
/// command object containing its name, its function and args
typedef struct {
	char *n; ///< name of command
	int (*e)(const char, const char **,
		 mm_session *); ///< function to excute
	char s;			///< short arg name else 0
	char *l;		///< long arg name or NULL
	char *h;		///< command help message
	uint8_t a;		///< nbre of max options args accpeted
} cmd_t;
cmd_t cmds[];
int cmd_quit(const char, const char **, mm_session *);
int cmd_savegame(const char, const char **, mm_session *);
int cmd_set(const char, const char **, mm_session *);
int cmd_restart(const char, const char **, mm_session *);
int cmd_score(const char, const char **, mm_session *);
int cmd_help(const char, const char **, mm_session *);
int cmd_account(const char, const char **, mm_session *);
int cmd_version(const char, const char **, mm_session *);
#ifdef MM_GETOPT
int execArgs(int argc, char *argv[], mm_session *);
#endif // MM_GETOPT
#endif // __MM_CLI_CMD_H
