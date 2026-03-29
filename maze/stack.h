#ifndef STACK_H
#define STACK_H

typedef struct {
    int x;
    int y;
} Cell;

typedef struct {
    Cell *data;
    int top;
    int capacity;
} Stack;

Stack *create_stack(int capacity);
void push(Stack *s, int x, int y);
Cell pop(Stack *s);
Cell peek(Stack *s);
int is_empty(Stack *s);
void free_stack(Stack *s);

#endif
