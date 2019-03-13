#include <stdio.h>
#include <unistd.h>

#include "shell.h"

/* `line' adds a newline to standard input if necessary. */

int main(int argc, char *argv[])
{
	int c;
	char chr;

	c = 1;
	while (!terminator(argv[c])) c++;

	if (c != 1) usage("%s", argv[0]);

	while (read(STDIN_FILENO, &chr, 1) > 0) {
		write(STDOUT_FILENO, &chr, 1);
	}

	if (chr != '\n') printf("\n");

	execat(argc, argv, c + 1);
}
