default: game.out
	./game.out

game.out: main.c
	gcc -Wall -Wextra main.c -o game.out -lSDL3
