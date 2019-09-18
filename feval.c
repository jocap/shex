#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shex.h"

#define MAX_STRINGS 30

char **str;

char *format(char *fmt) {
	char *new;
	int i, size, total;

	size = strlen(fmt);
	if ((new = malloc(size)) == NULL)
		err(1, "malloc");

	for (i = 0; i < (int)strlen(fmt) + 1; i++) {
		/* make room for 1 or 2 more characters */
		if ((int)strlen(new) + 2 > size) {
			if ((new = realloc(new, size + 50)) == NULL)
				err(1, "realloc");
			size += 50;
		}

		if (fmt[i] == '%') {
			i++;
			switch (fmt[i]) {
			case '\0': // end of format string
				new[strlen(new)] = '%';
				new[strlen(new)] = '\0';
				break;
			case 's':
				if (*str == 0) goto too_many; // end of strings
				total = strlen(new) + strlen(*str) + 1;
				if (total > size) {
					if ((new = realloc(new, total)) == NULL)
						err(1, "realloc");
					size = total;
				}
				strlcat(new, *str, total);
				str++; // next string
				break;
			case '%':
				break;
			default:
				new[strlen(new)] = '%';
				new[strlen(new)] = fmt[i];
			}
		} else {
			new[strlen(new)] = fmt[i];
		}
	}

	return new;

too_many:
	fprintf(stderr, "feval: more '%%s' conversions than provided strings\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	int c;

	c = 1;
	while (!terminator(argv[c])) c++;

	if (c < 2) usage("%s argv-format ...", argv[0]);

	/* collect strv from standard input */
	char chr;
	char *strv[MAX_STRINGS];
	int i, len, strc, r, size;

	memset(strv, 0, MAX_STRINGS);
	strc = 0; // number of strings
	i = 0; // current string
	size = 0; // allocated size of current string
	len = 0; // length of current string

	while ((r = read(STDIN_FILENO, &chr, 1)) > 0) {
		if (len + 1 > size) {
			if ((strv[i] = realloc(strv[i], size + 50)) == NULL)
				err(1, "realloc");
			size += 50;
		}

		strv[i][len] = chr;
		len++;

		if (chr == '\0') { // prepare new string
			i++;
			size = 0;
			len = 0;
			strc++;
			if (i >= MAX_STRINGS) break;
		}
	}

	strv[strc] = 0;

	if (r == -1) err(1, "read");

	/* process argv format */

	str = strv;
	for (i = 1; i < c; i++) {
		argv[i] = format(argv[i]);
	}

	execat(argc, argv, 1);
}
