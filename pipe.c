#include <err.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "shex.h"

#define USAGE "%s command | command [| ...]", argv[0]
#define MAX_COMMANDS 40

int main(int argc, char *argv[])
{
	int c;

	c = 1;
	while (!terminator(argv[c])) c++;

	if (c < 3) usage(USAGE);

	/* locate all commands to be linked in argv */
	int i, k, kc, ks[MAX_COMMANDS];

	k = 1;        // index of command in argv
	kc = 0;       // command count

	for (i = 2; i < c; i++) {
		if (strcmp(argv[i], "\\|") == 0) {
			argv[i] = "|";
		} else if (strcmp(argv[i], "|") == 0) {
add:
			argv[i] = NULL;
			ks[kc++] = k;
			k = i + 1;
		} else if (i + 1 == c) { // last argument
			i++;
			goto add;
		}
	}

	if (i == 0) usage(USAGE);

	/* create pipes and child processes for all commands */
	int corpse_pid, pipes[2][kc - 1], status;
	pid_t child_pids[kc - 1];

	for (i = 0; i < kc; i++)
		if (pipe(pipes[i]) == -1) err(1, "pipe");

	k = 0;

fork:
	if ((child_pids[k] = fork()) == -1) err(1, "fork");

	if (child_pids[k] == 0) {
		if (k != 0) // if not first command
			if (dup2(pipes[k - 1][0], 0) == -1) err(1, "pipe");
		if (k + 1 < kc) // if not last command
			if (dup2(pipes[k][1], 1) == -1) err(1, "pipe");
		for (i = 0; i < kc; i++) {
			close(pipes[i][0]);
			close(pipes[i][1]);
		}
		execat(-1, argv, ks[k]);
		/* or error */
	}

	k++;
	if (k < kc) goto fork;

	for (i = 0; i < kc; i++) {
		close(pipes[i][0]);
		close(pipes[i][1]);
	}

	while ((corpse_pid = wait(&status)) > 0) {
		if (status > 0) return status;
	}

	execat(argc, argv, c + 1);
}

