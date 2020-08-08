CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic

hex: hex.c
	$(CC) $(CFLAGS) -o hex hex.c

lint:
	clang-tidy -checks="*" -header-filter=".*" *.c

clean:
	rm hex

.PHONY: clean lint
