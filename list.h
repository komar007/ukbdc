#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct list_node {
	struct list_node *next;
	void *data;
} node_t;

typedef struct {
	node_t *head;
	node_t *tail;
	size_t size;
} list_t;

void list_init(list_t *list);
void list_add(list_t *list, void *data);
void list_clear(list_t *list);
