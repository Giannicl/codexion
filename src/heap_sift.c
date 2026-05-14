/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/* heap_sift.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glieuw-a <glieuw-a@student.codam.nl>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 12:00:00 by glieuw-a        #+#    #+#               */
/*   Updated: 2026/05/14 12:00:00 by glieuw-a       ###   ########.fr         */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	swap_nodes(t_heap *h, int a, int b)
{
	t_node	tmp;

	tmp = h->data[a];
	h->data[a] = h->data[b];
	h->data[b] = tmp;
}

int	node_lt(t_node a, t_node b)
{
	if (a.priority != b.priority)
		return (a.priority < b.priority);
	return (a.id < b.id);
}

void	sift_up(t_heap *h, int i)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!node_lt(h->data[i], h->data[parent]))
			break ;
		swap_nodes(h, i, parent);
		i = parent;
	}
}

void	sift_down(t_heap *h, int i)
{
	int	left;
	int	right;
	int	min;

	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		min = i;
		if (left < h->size && node_lt(h->data[left], h->data[min]))
			min = left;
		if (right < h->size && node_lt(h->data[right], h->data[min]))
			min = right;
		if (min == i)
			break ;
		swap_nodes(h, i, min);
		i = min;
	}
}
