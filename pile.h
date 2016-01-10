#ifndef pile_h
#define pile_h

#include <stdio.h>
#include <stdlib.h>

typedef struct stack_s stack_type;
typedef stack_type *Stack;

typedef struct elem_s elem_t;
typedef elem_t *Elem;

int pop(Stack s);

int top(Stack s);

void push(Stack s, int i);

int height(Stack s);

Stack new_stack();

void delete(Stack s);


#endif /* pile_h */
