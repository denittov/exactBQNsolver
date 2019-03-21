CPP = g++
CC = gcc
CFLAGS = -O3 -g -Wall
LDFLAGS =-lm

all: generator matrix_solver indices append_indices

generator: src/generator.cpp src/state.cpp src/transition.cpp
	$(CPP) $(CFLAGS) src/generator.cpp src/state.cpp src/transition.cpp -o bin/generator

matrix_solver: src/matrix_solver.c include/meschach.a
	$(CC) $(CFLAGS) -m32 src/matrix_solver.c include/meschach.a $(LDFLAGS) -o bin/matrix_solver

indices: src/indices.c include/meschach.a
	$(CC) $(CFLAGS) -m32 src/indices.c include/meschach.a $(LDFLAGS) -o bin/indices
	
append_indices: src/append_indices.c
	$(CC) $(CFLAGS) -m32 src/append_indices.c $(LDFLAGS) -o bin/append_indices

cleancode:
	/bin/rm -f bin/generator
	/bin/rm -f bin/matrix_solver
	/bin/rm -f bin/indices
	/bin/rm -f bin/append_indices
