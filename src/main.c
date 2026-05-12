#include "codexion.h"

static t_coder	*init_coders(t_sim *sim)
{
	t_coder	*coders;
	int		i;

	coders = malloc(sizeof(t_coder) * sim->args.n_coders);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < sim->args.n_coders)
	{
		coders[i].id = i + 1;
		coders[i].n_compiled = 0;
		coders[i].last_compile_ms = sim->start_ms;
		coders[i].deadline_ms = sim->start_ms + sim->args.burnout_ms;
		coders[i].sim = sim;
		i++;
	}
	return (coders);
}

static int	alloc_resources(t_sim *sim, t_coder **coders, pthread_t **threads)
{
	*coders = init_coders(sim);
	if (!*coders)
		return (0);
	*threads = malloc(sizeof(pthread_t) * sim->args.n_coders);
	if (!*threads)
	{
		free(*coders);
		return (0);
	}
	return (1);
}

static void	free_resources(t_coder *coders, pthread_t *threads, t_sim *sim)
{
	free(threads);
	free(coders);
	sim_cleanup(sim);
}

static int	launch_coders(t_sim *sim, t_coder *coders, pthread_t *threads)
{
	int	i;

	i = 0;
	while (i < sim->args.n_coders)
	{
		if (pthread_create(&threads[i], NULL, coder_routine, &coders[i]) != 0)
			return (0);
		i++;
	}
	return (1);
}

static int	launch_monitor(t_sim *sim, t_coder *coders, pthread_t *monitor)
{
	sim->mon_args[0] = sim;
	sim->mon_args[1] = coders;
	return (pthread_create(monitor, NULL, monitor_routine, sim->mon_args) == 0);
}

static void	join_coders(t_sim *sim, pthread_t *threads)
{
	int	i;

	i = 0;
	while (i < sim->args.n_coders)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
}

static void	join_threads(t_sim *sim, pthread_t *threads, pthread_t monitor)
{
	pthread_join(monitor, NULL);
	join_coders(sim, threads);
}

static int	run_sim(t_sim *sim, t_coder *coders, pthread_t *threads)
{
	pthread_t	monitor;

	if (!launch_coders(sim, coders, threads))
		return (0);
	if (!launch_monitor(sim, coders, &monitor))
	{
		sim_stop(sim);
		join_coders(sim, threads);
		return (0);
	}
	join_threads(sim, threads, monitor);
	return (1);
}

int	main(int argc, char **argv)
{
	t_sim		sim;
	t_coder		*coders;
	pthread_t	*threads;

	if (!parse_args(argc, argv, &sim.args) || !sim_init(&sim))
		return (1);
	if (!alloc_resources(&sim, &coders, &threads))
	{
		sim_cleanup(&sim);
		return (1);
	}
	if (!run_sim(&sim, coders, threads))
	{
		free_resources(coders, threads, &sim);
		return (1);
	}
	free_resources(coders, threads, &sim);
	return (0);
}
