#include "pile.h"

struct elem_s{
	int val;
	struct elem_s* next;
};

struct stack_s{
	Elem head;
	int nbElem;
};

int height(Stack s){
	return s->nbElem;
}

int pop(Stack s){
	if (s->nbElem == 0){
		fprintf(stderr, "Pile vide.\n");
		exit(1);
	}
	--s->nbElem;
	Elem tmp = s->head;
	s->head = tmp->next;
	int i = tmp->val;
	free(tmp);
	return i;
}

int top(Stack s){
	if (s->nbElem)
		return s->head->val;
	else{
		return(-1);
		exit(1);
	}
}

void push(Stack s, int i){
	Elem tmp = malloc(sizeof(elem_t));
	++s->nbElem;
	tmp->next = s->head;
	tmp->val = i;
	s->head = tmp;
}

Stack new_stack(){
	Stack tmp = malloc(sizeof(stack_type));
	tmp->head = NULL;
	tmp->nbElem = 0;
	return tmp;
}

void delete(Stack s){
	Elem tmp = s->head, next;
	while (tmp){
		next = tmp->next;
		free(tmp);
		tmp = next;
	}
	free(s);
}