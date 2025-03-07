CFLAGS=-std=c99 -Wall -Wextra -Werror
main:
	gcc main.c -o ./out/main $(CFLAGS) `sdl2-config --cflags --libs`
clean:
	rm main