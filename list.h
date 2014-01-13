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

/*! \file list.h
 * A generic implementation of a singly-linked list */

#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct list_node {
	struct list_node *next;
	void *data;
} node_t;

/*! The type representing a singly-linked list */
typedef struct {
	node_t *head;
	node_t *tail;
	size_t size;
} list_t;

/*! Initializes a list. This function must be called before any other
 * functions on this list */
void list_init(list_t *list);
/*! Adds an element to the list
 * \param list the list to add the element to
 * \param data the element to add to the list
 */
void list_add(list_t *list, void *data);
/*! Removes all elements from the list
 * \param list the list to clear
 */
void list_clear(list_t *list);
