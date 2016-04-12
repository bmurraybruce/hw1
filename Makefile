# cs335 lab1
# to compile your project, type make and press enter

CFLAGS = -I ./include
##LIB    = ./libggfonts.so
LFLAGS = $(LIB) -lrt -lX11 -lGL -lGLU -pthread -lm

all: hw1

hw1: hw1.cpp
	g++ $(CFLAGS) hw1.cpp libggfonts.a -Wall $(LFLAGS) -ohw1

clean:
	rm -f hw1
	rm -f *.o

