CC=g++
CFLAGS=-Wall -O3
LFLAGS=-lSDL2 -lSDL2main -lSDL2_image -lstdc++ 

main: main.cpp
	$(CC) $(CFLAGS) $< $(LFLAGS) -o $@

clean:
	rm -rf main
