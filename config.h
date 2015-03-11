#ifndef __MASTERMIND_CONFIG_H
#define __MASTERMIND_CONFIG_H

#define PACKAGE "mastermind"
#define PROGRAM_NAME PACKAGE
#define PROGRAM_VERSION "0.1"
#define PROGRAM_URL "https://github.com/lejenome/mastermind"

// default settings
#define MM_HOLES 4
#define MM_COLORS 6
#define MM_GUESSES 10

#define MM_HOLES_MAX 8
#define MM_COLORS_MAX 12
#define MM_GUESSES_MAX 20

#ifndef DISABLE_LOCALE
#include <locale.h>
#include <libintl.h>
#define _(str) gettext(str)

#else
#define _(str) (str)
#endif // DISABLE_LOCALE

#ifndef LOCALEDIR
#define LOCALEDIR "intl"
#endif // LOCALEDIR

#endif //__MASTERMIN_CONFIG_H
