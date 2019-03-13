#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "shell.h"

bool terminator(char *x)
{
	if (x == NULL) return true;
	if (strlen(x) > 1) return false;
	if (x[0] == ';') return true;
	return false;
}

void execat(int argc, char *argv[], int offset)
{
	if (offset >= argc) {
		exit(0);
	} else {
		extern char **environ;
		if (execve(argv[offset], argv + offset, environ) == -1) {
			perror(argv[offset]);
			exit(1);
		}
	}
}
