CFLAGS = -Wall -Wno-missing-braces -Wextra -Wpedantic -O2

all: receive

%: %.c shell.c
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^
