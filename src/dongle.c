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
		return (0);
	if (pthread_cond_init(&d->cond, NULL) != 0)
		return (0);
	return (1);
}

void	dongle_destroy(t_dongle *d)
{
	heap_destroy(&d->queue);
	pthread_mutex_destroy(&d->mutex);
	pthread_cond_destroy(&d->cond);
}

static long	dongle_priority(t_dongle *d, t_coder *coder)
{
	long	seq;

	if (coder->sim->args.scheduler == EDF)
		return (coder->deadline_ms);
	seq = d->fifo_seq;
	d->fifo_seq++;
	return (seq);
}

static int	dongle_ready(t_dongle *d)
{
	return (d->available && time_now_ms() >= d->ready_at);
}

static void	dongle_wait(t_dongle *d)
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

int	dongle_take(t_dongle *d, t_coder *coder)
{
	pthread_mutex_lock(&d->mutex);
	heap_push(&d->queue, coder->id, dongle_priority(d, coder));
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
