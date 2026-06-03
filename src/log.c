/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/* log.c                                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glieuw-a <glieuw-a@student.codam.nl>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 12:00:00 by glieuw-a        #+#    #+#               */
/*   Updated: 2026/05/14 12:00:00 by glieuw-a       ###   ########.fr         */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

# define RESET  "\033[0m"
# define CYAN   "\033[36m"
# define GREEN  "\033[32m"
# define YELLOW "\033[33m"
# define BLUE   "\033[34m"
# define RED    "\033[31;1m"

static const char	*event_color(const char *event)
{
	if (event[0] == 'h')
		return (CYAN);
	if (event[3] == 'c')
		return (GREEN);
	if (event[3] == 'd')
		return (YELLOW);
	if (event[3] == 'r')
		return (BLUE);
	return (RED);
}

void	log_event(t_sim *sim, int id, const char *event)
{
	long	ts;

	ts = time_now_ms() - sim->start_ms;
	pthread_mutex_lock(&sim->log_mutex);
	printf("%s%ld %d %s%s\n", event_color(event), ts, id, event, RESET);
	pthread_mutex_unlock(&sim->log_mutex);
}
