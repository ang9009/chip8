CFLAGS=-std=c17 -Wall -Wextra -Werror
all:
	gcc main.c -o main $(CFLAGS) `sdl2-config --cflags --libs`
	