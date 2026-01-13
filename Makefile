game.out: main.c
	gcc -Wall -Wextra main.c -o game.out -lSDL3

run: game.out
	./game.out

clean: game.out
	rm game.out
