#ifndef KSTACK_H
#define KSTACK_H

#include <stdio.h>

struct stack {
	int top;
	void ** data;
	size_t  size;
};

typedef struct stack kstack_t;

kstack_t * stack_create(int size);
void stack_destroy(kstack_t *);
int stack_isempty(kstack_t *);
int stack_isfull(kstack_t *);
int stack_len(kstack_t *);
void * stack_peek(kstack_t *);
void * stack_pop(kstack_t *);
void stack_push(kstack_t *, void * data);

#endif