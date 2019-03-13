#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shell.h"

int receive(char *);

bool positional = false;

int main(int argc, char *argv[])
{
	int c, offset;

	/* calculate own arg count */
	c = 1;
	while (!terminator(argv[c])) c++;

	/* process options */
	if (c < 2) goto usage;

	if (argv[1][0] == '-') {
		if (argv[1][2] != '\0') goto usage;
		offset = 2;

		switch (argv[1][1]) {
		case 'p':
			positional = true;
			if (c > 2) goto usage;
			break;
		case '-':
			break;
		default:
			goto usage;
		}
	} else {
		offset = 1;
	}

	/* assign variables */
	if (positional) {
		int n = 0;
		char variable[3];
		do {
			snprintf(variable, 3, "%d", ++n);
		} while (receive(variable) != -1 && n < 10);
	} else {
		for (; offset < c; offset++) {
			receive(argv[offset]);
		}
	}

	execat(argc, argv, c + 1);

usage:
	usage("%s [-p] [variable ...]", argv[0]);
}

int receive(char *variable)
{
	char c;
	size_t i, size;
	char *string;

	size = 50 * sizeof (char);
	if ((string = malloc(size)) == NULL)
		err(1, "malloc");

	if (read(STDIN_FILENO, &c, 1) <= 0 || c == 0) return -1;
	else string[0] = c;
	i = 1;

	while (read(STDIN_FILENO, &c, 1) > 0 && c != 0) {
		if (i > size) {
			if ((string = realloc(string, size + 50)) == NULL)
				err(1, "realloc");
			size = size + 50;
		}
		string[i] = c;
		i++;
	}

	if (setenv(variable, string, 0) == -1)
		err(1, "setenv");

	free(string);

	return 0;
}
