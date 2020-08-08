CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic

hex: hex.c
	$(CC) -o hex hex.c $(CLFAGS)

lint:
	clang-tidy -checks="*" -header-filter=".*" *.c

clean:
	rm hex

.PHONY: clean lint
