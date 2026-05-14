/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/* dongle.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glieuw-a <glieuw-a@student.codam.nl>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 12:00:00 by glieuw-a        #+#    #+#               */
/*   Updated: 2026/05/14 12:00:00 by glieuw-a       ###   ########.fr         */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	dongle_init(t_dongle *d, int id, int cap)
{
	d->id = id;
	d->available = 1;
	d->ready_at = 0;
	d->fifo_seq = 0;
	if (!heap_init(&d->queue, cap))
		return (0);
	if (pthread_mutex_init(&d->mutex, NULL) != 0)
	{
		heap_destroy(&d->queue);
		return (0);
	}
	if (pthread_cond_init(&d->cond, NULL) != 0)
	{
		heap_destroy(&d->queue);
		pthread_mutex_destroy(&d->mutex);
		return (0);
	}
	return (1);
}

void	dongle_destroy(t_dongle *d)
{
	heap_destroy(&d->queue);
	pthread_mutex_destroy(&d->mutex);
	pthread_cond_destroy(&d->cond);
}

static int	dongle_ready(t_dongle *d)
{
	return (d->available && time_now_ms() >= d->ready_at);
}

int	dongle_take(t_dongle *d, t_coder *coder)
{
	long	priority;

	pthread_mutex_lock(&d->mutex);
	if (coder->sim->args.scheduler == EDF)
		priority = coder->deadline_ms;
	else
		priority = d->fifo_seq++;
	heap_push(&d->queue, coder->id, priority);
	while (1)
	{
		if (sim_stopped(coder->sim))
		{
			heap_remove(&d->queue, coder->id);
			pthread_mutex_unlock(&d->mutex);
			return (0);
		}
		if (dongle_ready(d) && heap_peek_id(&d->queue) == coder->id)
		{
			d->available = 0;
			heap_remove(&d->queue, coder->id);
			pthread_mutex_unlock(&d->mutex);
			return (1);
		}
		dongle_wait(d);
	}
}

void	dongle_release(t_dongle *d, t_coder *coder)
{
	pthread_mutex_lock(&d->mutex);
	d->available = 1;
	d->ready_at = time_now_ms() + coder->sim->args.cooldown_ms;
	pthread_cond_broadcast(&d->cond);
	pthread_mutex_unlock(&d->mutex);
}
