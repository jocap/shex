#include <stdio.h>
#include <string.h>

#include "shell.h"

int ter = -1; // terminator

int main(int argc, char *argv[])
{
	int c, i;

	c = 1;
	while (!terminator(argv[c])) c++;

	/* parse options */
	if (c > 1) {
		if (argv[1][0] == '-') {
			if (strcmp(argv[1], "-t") == 0) {
				if (c < 4) goto usage;
				ter = argv[2][0];
				i = 3;
			} else if (strcmp(argv[1], "--") == 0) {
				if (c < 3) goto usage;
				i = 2;
			} else {
				goto usage;
			}
		} else {
			i = 1;
		}
	} else {
		goto usage;
	}

	for (; i < c; i++) {
		printf("%s", argv[i]);
		if (ter == -1) putchar('\0'); // default
		else if (ter != 0) putchar(ter); // user-provided
	}

	execat(argc, argv, c + 1);

usage:
	fprintf(stderr, "usage: %s [-t ter] string\n", argv[0]);
	return 1;
}
