CC = gcc
CFLAGS = -ansi -pedantic -Wall -Werror 

OBJ = main.o \
      parser.o
#       generator.o stack.o \
#       bfs.o queue.o \
#       bmp.o

maze_solver: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o maze_solver

main.o: main.c config/parser.h grid.h
	$(CC) $(CFLAGS) -c main.c -o main.o

parser.o: config/parser.c config/parser.h
	$(CC) $(CFLAGS) -c config/parser.c -o parser.o

main.o: main.c config/parser.h grid.h
parser.o: config/parser.c config/parser.h
# generator.o: maze/generator.c grid.h
# stack.o: maze/stack.c
# bfs.o: solver/bfs.c grid.h
# queue.o: solver/queue.c
# bmp.o: render/bmp.c grid.h

clean:
	rm -f *.o maze_solver