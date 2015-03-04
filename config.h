#ifndef MASTERMIND_CONFIG_H
#define MASTERMIND_CONFIG_H

#define PACKAGE		"mastermind"
#define PROGRAM_NAME	PACKAGE
#define PROGRAM_VERSION "0.1"
#define PROGRAM_URL	"https://github.com/lejenome/mastermind"

// default settings
#define MM_HOLES	4
#define MM_COLORS	6
#define MM_GUESSES	12

#include <locale.h>
#include <libintl.h>

#define _(str) gettext(str)

#ifndef LOCALEDIR
#define LOCALEDIR	"intl"
#endif//LOCALEDIR

#endif//MASTERMIN_CONFIG_H
