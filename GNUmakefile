CFLAGS = -Wall -Wno-missing-braces -Wextra -Wpedantic -O2

all: receive

%: %.c shex.c
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^
