#include "arg_list.h"

#include <stdlib.h>

void arg_list_init(arg_list_t *list)
{
	list->number = -1;
	list->value = 0.0;

	list->next = 0;
}

void arg_list_free(arg_list_t **list)
{
	arg_list_t **head = list;
	arg_list_t **next;

	if (!list) {
		return;
	}

	while (*list != 0) {
		next = &(*list)->next;

		free(*list);

		list = next;
	}

	*head = 0;
}

void arg_list_add(arg_list_t **list, int number, double value)
{
	if (!list) {
		return;
	}

	while (*list != 0) {
		if ((*list)->number == number) {
			(*list)->value = value;

			return;
		}
		list = &(*list)->next;
	}

	*list = malloc(sizeof(**list));
	(*list)->number = number;
	(*list)->value = value;
	(*list)->next = 0;
}

