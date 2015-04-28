#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "util.h"

/*!
 * \file lib.c
 * \brief missing or core/iterface independent functions definition
 */

#ifndef POSIX
/* POSIX strndup spec implementation on non POSIX systems */
char *strndup(const char *buf, size_t len)
{
	size_t i = strlen(buf);
	if (i < len)
		len = i;
	char *s = (char *)malloc(sizeof(char) * (len + 1));
	for (i = 0; i < len; i++)
		s[i] = buf[i];
	s[i] = '\0';
	return s;
}
#endif
/*! parse buffer and get arguments from it
 * @param buf	buffer to parse
 * @param argc	poiter to where to store arguments count
 * @return 	arguments array or NULL if buf is invalid
 */
char **parseBuf(char *buf, unsigned *argc)
{
	char *start = NULL;
	char **args, *c;
	unsigned i = 0;
	c = buf;
	// get argc
	while (*c != '\0') {
		if (strchr(IFS, *c)) {
			if (start) {
				i++;
				start = NULL;
			}
		} else if ((*c >= '0' && *c <= '9') ||
			   (*c >= 'a' && *c <= 'z') || *c == '_') {
			if (start == NULL)
				start = c;
		} else {
			printf(_("\nError: illegal charater on the command "
				 "'%c'\n"),
			       *c);
			i = 0;
			start = NULL;
			break;
		}
		c++;
	}
	if (start)
		i++;
	*argc = i;
	// get args
	if (*argc == 0)
		return (char **)NULL;
	args = (char **)malloc(sizeof(char *) * (*argc));
	i = 0;
	start = NULL;
	while (*buf != '\0') {
		if (strchr(IFS, *buf)) {
			if (start) {
				args[i++] = strndup(start, buf - start);
				start = NULL;
			}
		} else {
			if (start == NULL)
				start = buf;
		}
		buf++;
	}
	if (start)
		args[i++] = strndup(start, buf - start);
	return args;
}
