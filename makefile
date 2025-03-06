CFLAGS=-std=c17 -Wall -Wextra -Werror
main:
	gcc main.c -o main $(CFLAGS) `sdl2-config --cflags --libs`
clean
	rm main