#include <err.h>
#include <unistd.h>
#include "shex.h"

int main(int argc, char *argv[]) {
	char chr;
	int c, r, w;

	c = 1;
	while (!terminator(argv[c])) c++;

	if (c > 1) usage("%s", argv[0]);

	while ((r = read(STDIN_FILENO, &chr, 1)) > 0) {
		if (chr == '\n') w = write(STDOUT_FILENO, "\0", 1);
		else w = write(STDOUT_FILENO, &chr, 1);
		if (w == -1) err(1, "write");
	}

	if (r == -1) err(1, "read");

	execat(argc, argv, c + 1);
}
