#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shell.h"

/* ---------------------------------------------------------------------
`exp' execs into the following argv, but first having expanded any
${variables} in it.
--------------------------------------------------------------------- */

/***********************************************************************
`append' appends character `chr' to a string `str' of size `*size',
increasing the size if necessary.
***********************************************************************/

void append(char chr, char *str, size_t *size)
{
	size_t len, inc;

	len = strlen(str);
	inc = 50;

	if (len + 1 > *size)
		if ((str = realloc(str, *size + inc)) == NULL)
			err(1, "realloc");
	size += inc;
	str[len] = chr;
	str[len + 1] = '\0';
}

/***********************************************************************
`variable_at' extracts and returns the variable name beginning at
index `*i' in string `str'.  It changes `*i' to the index of the first
character following the variable name.

str[*i] should be the first character of the variable name, terminated
by a single } character.

`expand' also supports backslash-escaping of } and \.
***********************************************************************/

char *variable_at(char *str, size_t *i)
{
	char *variable;
	size_t size;

	size = 10;
	if ((variable = malloc(size)) == NULL)
		err(1, "malloc");

	/* process variable name */
	for (; *i < strlen(str); (*i)++) {
		switch (str[*i]) {
		/* escape */
		case '\\':
			(*i)++;
			switch (str[*i]) {
			/* special characters */
			case '\\':
			case '}':
				append(str[*i], variable, &size);
				break;
			/* normal characters */
			default:
				append('\\', variable, &size);
				append(str[*i], variable, &size);
			}
			break;
		/* end */
		case '}':
			goto end;
		/* normal character */
		default:
			append(str[*i], variable, &size);
		}
	}

	/* normally, } should take us to the `end' label */
	fprintf(stderr, "(exp) syntax error: missing }\n");
	exit(1);

end:
	return variable;
}

/***********************************************************************
`expand' processes string `old' and expands any ${variables}, returning
the resulting string `new'.

It also supports backslash-escaping of $ and \.
***********************************************************************/

char *expand(char *old)
{
	char *new, *variable, *value;
	size_t i, size;

	size = strlen(old) + 1;
	if ((new = malloc(size)) == NULL)
		err(1, "malloc");

	for (i = 0; i < strlen(old); i++) {
		switch (old[i]) {
		/* escape */
		case '\\':
			i++;
			switch (old[i]) {
			/* end of string */
			case '\0':
				append('\\', new, &size);
				break;
			/* special characters */
			case '\\':
			case '$':
				append(old[i], new, &size);
				break;
			/* non-special characters */
			default:
				append('\\', new, &size);
				append(old[i], new, &size);
			}
			break;
		/* expand variable */
		case '$':
			if (old[i + 1] == '{') {
				i += 2;
				variable = variable_at(old, &i);
				if ((value = getenv(variable)) == NULL) break;
				strlcat(new, value, strlen(new) + strlen(value) + 1);
				free(variable);
				break;
			}
		/* normal character */
		default:
			append(old[i], new, &size);
		}
	};

	return new;
}

int main(int argc, char *argv[])
{
	int c, i;

	c = 1;
	while (!terminator(argv[c])) c++; // expand until terminator (semicolon)

	if (c < 2) usage("%s argv ...", argv[0]);

	/* expand arguments within every argument separately */
	for (i = 1; i < c; i++) {
		argv[i] = expand(argv[i]);
	}

	execat(argc, argv, 1);
}
