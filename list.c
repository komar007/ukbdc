#include "list.h"

#include <stdlib.h>

void list_init(list_t *list)
{
	list->size = 0;
	list->head = list->tail = NULL;
}

void list_add(list_t *list, void *data)
{
	node_t *new_node = calloc(1, sizeof(*new_node));
	new_node->data = data;
	new_node->next = NULL;
	if (list->size == 0)
		list->head = list->tail = new_node;
	else
		list->tail = list->tail->next = new_node;
	++list->size;
}

void list_clear(list_t *list)
{
	node_t *node = list->head;
	for (int i = 0; i < list->size; ++i) {
		node_t *next = node->next;
		free(node);
		node = next;
	}
	list_init(list);
}
