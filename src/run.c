/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/* run.c                                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glieuw-a <glieuw-a@student.codam.nl>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 12:00:00 by glieuw-a        #+#    #+#               */
/*   Updated: 2026/05/14 12:00:00 by glieuw-a       ###   ########.fr         */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	launch_coders(t_sim *sim, t_coder *coders, pthread_t *threads)
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

int	launch_monitor(t_sim *sim, t_coder *coders, pthread_t *monitor)
{
	sim->mon_args[0] = sim;
	sim->mon_args[1] = coders;
	return (pthread_create(monitor, NULL, monitor_routine, sim->mon_args) == 0);
}

void	join_coders(t_sim *sim, pthread_t *threads)
{
	int	i;

	i = 0;
	while (i < sim->args.n_coders)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
}

void	join_threads(t_sim *sim, pthread_t *threads, pthread_t monitor)
{
	pthread_join(monitor, NULL);
	join_coders(sim, threads);
}
