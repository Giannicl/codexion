#include "codexion.h"

static int	init_dongles(t_sim *sim)
{
	int	i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->args.n_coders);
	if (!sim->dongles)
		return (0);
	i = 0;
	while (i < sim->args.n_coders)
	{
		if (!dongle_init(&sim->dongles[i], i, sim->args.n_coders))
			return (0);
		i++;
	}
	return (1);
}

static void	cleanup_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->args.n_coders)
	{
		dongle_destroy(&sim->dongles[i]);
		i++;
	}
	free(sim->dongles);
}

int	sim_init(t_sim *sim)
{
	sim->stop = 0;
	sim->start_ms = time_now_ms();
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (0);
	return (init_dongles(sim));
}

void	sim_cleanup(t_sim *sim)
{
	cleanup_dongles(sim);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
}

int	sim_stopped(t_sim *sim)
{
	int	val;

	pthread_mutex_lock(&sim->stop_mutex);
	val = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (val);
}

static void	wake_all_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->args.n_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}

void	sim_stop(t_sim *sim)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	wake_all_dongles(sim);
}
