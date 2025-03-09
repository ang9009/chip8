CFLAGS=-std=c99 -Wall -Wextra -Werror

main:
	gcc main.c init.c -o ./out/main $(CFLAGS) `sdl2-config --cflags --libs`

clean:
	rm main

run:
	./out/main $(ARGS)