/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/* main.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glieuw-a <glieuw-a@student.codam.nl>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 12:00:00 by glieuw-a        #+#    #+#               */
/*   Updated: 2026/05/14 12:00:00 by glieuw-a       ###   ########.fr         */
/*                                                                            */
/* ************************************************************************** */

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
