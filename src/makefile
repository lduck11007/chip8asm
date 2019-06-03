CC = gcc
FLAGS = -Wall -Wpedantic
STD = c11
BINNAME = main

main: main.c
	$(CC) main.c -o $(BINNAME) $(FLAGS) -std=$(STD)

clean: $(BINNAME)
	rm $(BINNAME)