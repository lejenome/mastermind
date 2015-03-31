#ifndef __MASTERMIN_LIB_H
#define __MASTERMIN_LIB_H

// Detect POSIX systems:
// http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) ||               \
			 (defined(__APPLE__) && defined(__MACH__)))
#define POSIX
#elif defined(_WIN32)
#define WINDOWS
#else
#error System type unknown, please report
#endif

// get static array size
#define LEN(a) (sizeof(a) / sizeof(a[0]))

#ifndef POSIX
char *strndup(const char *buf, size_t len);
#define srandom(var) srand(var)
#define random() rand()
#endif

// Global defines
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

#endif //__MASTERMIN_LIB_H
