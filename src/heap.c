/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/* heap.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glieuw-a <glieuw-a@student.codam.nl>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 12:00:00 by glieuw-a        #+#    #+#               */
/*   Updated: 2026/05/14 12:00:00 by glieuw-a       ###   ########.fr         */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_init(t_heap *h, int cap)
{
	h->data = malloc(sizeof(t_node) * cap);
	if (!h->data)
		return (0);
	h->size = 0;
	h->cap = cap;
	return (1);
}

void	heap_destroy(t_heap *h)
{
	free(h->data);
	h->data = NULL;
	h->size = 0;
}

void	heap_push(t_heap *h, int id, long priority)
{
	h->data[h->size].id = id;
	h->data[h->size].priority = priority;
	sift_up(h, h->size);
	h->size++;
}

void	heap_remove(t_heap *h, int id)
{
	int	i;

	i = 0;
	while (i < h->size && h->data[i].id != id)
		i++;
	if (i == h->size)
		return ;
	h->data[i] = h->data[--h->size];
	sift_up(h, i);
	sift_down(h, i);
}

int	heap_peek_id(t_heap *h)
{
	if (h->size == 0)
		return (-1);
	return (h->data[0].id);
}
