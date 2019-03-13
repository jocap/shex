CFLAGS = -Wall -Wno-missing-braces -Wextra -Wpedantic -O2

all: receive

receive: receive.c shell.c shell.h
	$(CC) $(CFLAGS) -o receive $(LDFLAGS) receive.c shell.c

let: let.c shell.c shell.h
	$(CC) $(CFLAGS) -o let $(LDFLAGS) let.c shell.c
