/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/* utils.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glieuw-a <glieuw-a@student.codam.nl>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 12:00:00 by glieuw-a        #+#    #+#               */
/*   Updated: 2026/05/14 12:00:00 by glieuw-a       ###   ########.fr         */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	time_now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

void	time_sleep_ms(long ms)
{
	long	end;

	end = time_now_ms() + ms;
	while (time_now_ms() < end)
		usleep(500);
}

void	dongle_wait(t_dongle *d)
{
	struct timespec	ts;

	if (d->ready_at > time_now_ms())
	{
		ts.tv_sec = d->ready_at / 1000;
		ts.tv_nsec = (d->ready_at % 1000) * 1000000L;
		pthread_cond_timedwait(&d->cond, &d->mutex, &ts);
	}
	else
		pthread_cond_wait(&d->cond, &d->mutex);
}
