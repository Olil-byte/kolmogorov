#ifndef ARG_LIST_H
#define ARG_LIST_H

typedef struct arg_list
{
	int number;
	double value;

	struct arg_list *next;
} arg_list_t;

void arg_list_init(arg_list_t *list);

void arg_list_free(arg_list_t **list);

void arg_list_add(arg_list_t **list, int number, double value);

#endif

