CC = g++
CCOPTS = -g -Wall
LDLIBS = -L/usr/local/lib -lgomp -lpng -ltrimesh -lglut -lGLU -lGL -lX11 -lm `sdl-config --cflags --libs` -lSDL_mixer

all: tanques

clean:
	rm -f *.o tanques

tanques: tanques.o list.o string_extra.o obj_parser.o objLoader.o
	$(CC) $(CCOPTS) -o tanques tanques.o list.o string_extra.o obj_parser.o objLoader.o $(LDLIBS)

tanques.o: tanques.cpp objLoader.h
	$(CC) $(CCOPTS) -c tanques.cpp -DAUDIO

list.o: list.cpp list.h
	$(CC) $(CCOPTS) -c list.cpp

string_extra.o: string_extra.cpp string_extra.h
	$(CC) $(CCOPTS) -c string_extra.cpp

obj_parser.o: obj_parser.cpp obj_parser.h list.h string_extra.h
	$(CC) $(CCOPTS) -c obj_parser.cpp

objLoader.o: objLoader.cpp objLoader.h obj_parser.h
	$(CC) $(CCOPTS) -c objLoader.cpp

