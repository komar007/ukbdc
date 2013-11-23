/* This file is part of ukbdc.
 *
 * ukbdc is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ukbdc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ukbdc; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

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
	for (unsigned i = 0; i < list->size; ++i) {
		node_t *next = node->next;
		free(node);
		node = next;
	}
	list_init(list);
}
