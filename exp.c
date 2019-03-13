#include <err.h>
#include <stdlib.h>
#include <stdio.h>

#include "shell.h"

int main(int argc, char *argv[])
{
	int c, i;

	c = 1;
	while (!terminator(argv[c])) c++;

	if (c < 1) goto usage;

	for (i = 1; i < c; i++) {
		if (argv[i][0] == '$') {
			char *value;
			if ((value = getenv(argv[i] + 1)) == NULL)
				err(1, "getenv");
			argv[i] = value;
		}
	}

	execat(argc, argv, 1);

usage:
	fprintf(stderr, "usage: %s command [argument ...]", argv[0]);
	return 1;
}
