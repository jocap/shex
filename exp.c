#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "shex.h"

/* return output of command described in `args' */
char *command(char *args)
{
	/* create argv from arg string */
	char **ap, **argv;
	int i, vlen;

	vlen = 20;
	if ((argv = reallocarray(NULL, vlen, sizeof (char *))) == NULL)
		err(1, "reallocarray");

	ap = argv;
	i = 0;
	while ((*ap = strsep(&args, " \t")) != NULL)
		if (**ap != '\0') { // unless "empty" field
			ap++;
			i++;
			if (i + 1 >= vlen) {
				if ((argv = reallocarray(argv, vlen + 20, sizeof (char *))) == NULL)
					err(1, "reallocarray");
				vlen += 20;
			}
		}
	*ap = NULL;

	/* fork */
	char *output;
	int pipes[2];
	int osize, r, written;
	pid_t child_pid;

	osize = 512;
	if ((output = malloc(osize)) == NULL) err(1, "malloc");

	if (pipe(pipes) == -1) err(1, "pipe");
	if ((child_pid = fork()) == -1) err(1, "fork");

	if (child_pid == 0) { // in child process
		/* change stdout into write end of pipe */
		if (dup2(pipes[1], STDOUT_FILENO) == -1) err(1, "dup2");
		/* close now unneeded descriptors */
		close(pipes[0]);
		close(pipes[1]);
		/* execute argv */
		extern char **environ;
		if (execvpe(argv[0], argv, environ) == -1) err(1, "%s", argv[0]);
	} else { // in parent
		close(pipes[1]);
		written = 0;
		while ((r = read(pipes[0], output, 512)) > 0) {
			written += r;
			if (written + 512 > osize) {
				if ((output = realloc(output, osize + 512)) == NULL)
					err(1, "realloc");
				osize += 512;
			}
		}
		if (r == -1) err(1, "read");

		if (wait(NULL) == -1) err(1, "wait");
	}

	if (output[strlen(output) - 1] == '\n')
		output[strlen(output) - 1] = '\0';

	return output;
}

/* return expansion of ${variable} or $(command) at index `i' in string `arg' */
char *expand_at(char *arg, size_t *i)
{
	bool escape;
	char *e, *pre;
	enum {VARIABLE, COMMAND} type;
	size_t start;

	*i += 1; // skip $
	if (arg[*i] == '{') type = VARIABLE;
	else if (arg[*i] == '(') type = COMMAND;
	else goto syntaxerror;
	*i += 1; // skip {/(

	escape = false;
	start = *i;
	for (; *i < strlen(arg); *i += 1) {
		switch (arg[*i]) {
		case '}':
			if (type == VARIABLE && !escape) goto done;
			break;
		case ')':
			if (type == COMMAND && !escape) goto done;
			break;
		case '\\':
			if (!escape) {
				escape = true;
				continue;
			}
		}
		escape = false;
	}

	goto enderror;

done:
	if ((pre = malloc(*i - start + 1)) == NULL) err(1, "malloc");
	memcpy(pre, &arg[start], *i - start);
	pre[*i - start + 1] = '\0';

	if (type == VARIABLE) {
		if ((e = getenv(pre)) == NULL) goto novariable;
	} else {
		e = command(pre);
	}

	return e;

syntaxerror:
	fprintf(stderr, "syntax error: no { or ( following $\n");
	exit(1);
enderror:
	fprintf(stderr, "syntax error: $ expansion missing terminating bracket\n");
	exit(1);
novariable:
	fprintf(stderr, "no such variable %s\n", pre);
	exit(1);
}

/* expand all variables and commands in string `arg' and return the new string */
char *expand_all(char *arg)
{
	bool escape;
	char *r, *e;
	size_t i, rsize, resize;

	rsize = strlen(arg) + 1;
	if ((r = malloc(rsize)) == NULL) err(1, "malloc");

	escape = false;
	for (i = 0; i < strlen(arg); i++) {
		if (strlen(r) + 1 > rsize - 1) {
			if ((r = realloc(r, rsize + 50)) == NULL) err(1, "realloc");
			rsize += 50;
		}

		switch (arg[i]) {
		case '$':
			if (escape) goto append;
			else {
				e = expand_at(arg, &i);
				resize = strlen(r) + strlen(e) + 1;
				if (resize > rsize) {
					if ((r = realloc(r, resize)) == NULL)
						err(1, "realloc");
					rsize = resize;
				}
				strlcat(r, e, resize);
			}
			break;
		case '\\': // escape
			if (escape) goto append;
			else {
				escape = true;
				continue;
			}
			break;
		default:
		append:
			r[strlen(r)] = arg[i];
			r[strlen(r)] = '\0';
			escape = false;
		}
	}

	return r;
}

int main(int argc, char *argv[])
{
	int c, i;

	c = 1;
	while (!terminator(argv[c])) c++;

	for (i = 1; i < c; i++) {
		argv[i] = expand_all(argv[i]);
	}

	execat(argc, argv, 1);
}
