CC     = gcc
CFLAGS = -ansi -pedantic -Wall -Werror

OBJ = main.o \
      parser.o \
      generator.o stack.o \
      bfs.o queue.o

maze_solver: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o maze_solver

main.o: main.c config/parser.h grid.h maze/generator.h solver/bfs.h
	$(CC) $(CFLAGS) -c main.c -o main.o

parser.o: config/parser.c config/parser.h
	$(CC) $(CFLAGS) -c config/parser.c -o parser.o

generator.o: maze/generator.c maze/generator.h maze/stack.h grid.h
	$(CC) $(CFLAGS) -c maze/generator.c -o generator.o

stack.o: maze/stack.c maze/stack.h
	$(CC) $(CFLAGS) -c maze/stack.c -o stack.o

bfs.o: solver/bfs.c solver/bfs.h solver/queue.h grid.h
	$(CC) $(CFLAGS) -c solver/bfs.c -o bfs.o

queue.o: solver/queue.c solver/queue.h
	$(CC) $(CFLAGS) -c solver/queue.c -o queue.o

# bmp.o: render/bmp.c render/bmp.h grid.h
# 	$(CC) $(CFLAGS) -c render/bmp.c -o bmp.o

clean:
	rm -f *.o maze_solver
