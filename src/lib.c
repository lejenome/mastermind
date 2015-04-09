#include <stdlib.h>
#include <string.h>
#include "lib.h"

#ifndef POSIX
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
