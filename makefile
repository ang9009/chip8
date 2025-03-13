CFLAGS=-std=c99 -Wall -Wextra -Werror

main:
	gcc main.c init.c render.c cycle.c -o ./out/main -g $(CFLAGS) `sdl2-config --cflags --libs`

clean:
	rm ./out/main

run:
	./out/main $(ARGS)