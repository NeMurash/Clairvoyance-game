cflags      := -Wall -Wextra
additionals := misc-utils.c
libs        := -lSDL3
output      := game

game.out: main.c
	gcc $(cflags) main.c $(additionals) -o $(output) $(libs)

run: $(output)
	./$(output)

clean:
	rm -f $(output)
