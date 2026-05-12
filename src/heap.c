#include "codexion.h"

static void	swap_nodes(t_heap *h, int a, int b)
{
	t_node	tmp;

	tmp = h->data[a];
	h->data[a] = h->data[b];
	h->data[b] = tmp;
}

static int	node_lt(t_node a, t_node b)
{
	if (a.priority != b.priority)
		return (a.priority < b.priority);
	return (a.id < b.id);
}

static void	sift_up(t_heap *h, int i)
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

static void	sift_down(t_heap *h, int i)
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
