cflags      := -Wall -Wextra
additionals := 
headers     := myUtils.h
libs        := -lSDL3
output      := game.out

$(output): main.c $(headers)
	gcc $(cflags) main.c $(additionals) -o $(output) $(libs)

run: $(output)
	./$(output)

clean:
	rm -f $(output)
