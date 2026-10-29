CC := gcc
CFLAGS := -Wall -Wextra

all: producer consumer test-syscalls

producer: producer.c wrappers.h
	$(CC) $(CFLAGS) -o $@ producer.c

consumer: consumer.c wrappers.h
	$(CC) $(CFLAGS) -o $@ consumer.c

test-syscalls: test-syscalls.c wrappers.h
	$(CC) $(CFLAGS) -o $@ test-syscalls.c

clean:
	rm -f producer consumer test-syscalls

.PHONY: all clean
