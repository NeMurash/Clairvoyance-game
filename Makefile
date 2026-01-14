warnings    := -Wall -Wextra
additionals := misc-utils.c
libs        := -lSDL3
output      := game

game.out: main.c
	gcc $(warnings) main.c $(additionals) -o $(output) $(libs)

run: $(output)
	./$(output)

clean:
	rm -f $(output)
