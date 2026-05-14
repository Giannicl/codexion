/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/* coder_state.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glieuw-a <glieuw-a@student.codam.nl>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 12:00:00 by glieuw-a        #+#    #+#               */
/*   Updated: 2026/05/14 12:00:00 by glieuw-a       ###   ########.fr         */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	coder_debug(t_coder *coder)
{
	log_event(coder->sim, coder->id, "is debugging");
	time_sleep_ms(coder->sim->args.debug_ms);
}

void	coder_refactor(t_coder *coder)
{
	log_event(coder->sim, coder->id, "is refactoring");
	time_sleep_ms(coder->sim->args.refactor_ms);
}
