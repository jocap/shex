#include <err.h>
#include <stdlib.h>
#include <stdio.h>

#include "shell.h"

int main(int argc, char *argv[])
{
	int c;

	c = 1;
	while (!terminator(argv[c])) c++;

	if (c != 3) goto usage;

	if (setenv(argv[1], argv[2], 1) == -1)
		err(1, "setenv");

	execat(argc, argv, c + 1);

usage:
	fprintf(stderr, "usage: %s variable content\n", argv[0]);
	return 1;
}
