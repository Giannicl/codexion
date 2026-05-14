/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/* monitor.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glieuw-a <glieuw-a@student.codam.nl>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 12:00:00 by glieuw-a        #+#    #+#               */
/*   Updated: 2026/05/14 12:00:00 by glieuw-a       ###   ########.fr         */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	coder_burned_out(t_sim *sim, t_coder *coder)
{
	if (coder->n_compiled >= sim->args.n_compiles)
		return (0);
	return (time_now_ms() - coder->last_compile_ms > sim->args.burnout_ms);
}

static int	check_burnouts(t_sim *sim, t_coder *coders)
{
	int	i;

	i = 0;
	while (i < sim->args.n_coders)
	{
		if (coder_burned_out(sim, &coders[i]))
		{
			log_event(sim, coders[i].id, "burned out");
			return (1);
		}
		i++;
	}
	return (0);
}

static int	all_compiled(t_sim *sim, t_coder *coders)
{
	int	i;

	i = 0;
	while (i < sim->args.n_coders)
	{
		if (coders[i].n_compiled < sim->args.n_compiles)
			return (0);
		i++;
	}
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	t_coder	*coders;

	sim = ((void **)arg)[0];
	coders = ((void **)arg)[1];
	while (!sim_stopped(sim))
	{
		if (check_burnouts(sim, coders) || all_compiled(sim, coders))
		{
			sim_stop(sim);
			return (NULL);
		}
		usleep(500);
	}
	return (NULL);
}
