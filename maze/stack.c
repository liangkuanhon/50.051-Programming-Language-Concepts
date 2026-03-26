#include <stdlib.h>
#include "stack.h"

Stack *create_stack(int capacity) {
    Stack *s = malloc(sizeof(Stack));
    s->data = malloc(sizeof(Cell) * capacity);
    s->top = -1;
    s->capacity = capacity;
    return s;
}

void push(Stack *s, int x, int y) {
    s->top++;
    s->data[s->top].x = x;
    s->data[s->top].y = y;
}

Cell pop(Stack *s) {
    return s->data[s->top--];
}

Cell peek(Stack *s) {
    return s->data[s->top];
}

int is_empty(Stack *s) {
    return s->top == -1;
}

void free_stack(Stack *s) {
    free(s->data);
    free(s);
}