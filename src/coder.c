#include "codexion.h"

static void	dongle_indices(t_coder *coder, int *first, int *second)
{
	int	left;
	int	right;

	left = coder->id - 1;
	right = coder->id % coder->sim->args.n_coders;
	if (coder->sim->args.n_coders == 1)
	{
		*first = left;
		*second = -1;
		return ;
	}
	if (left < right)
	{
		*first = left;
		*second = right;
	}
	else
	{
		*first = right;
		*second = left;
	}
}

static int	acquire_dongles(t_coder *coder)
{
	t_dongle	*dongles;
	int			first;
	int			second;

	dongle_indices(coder, &first, &second);
	dongles = coder->sim->dongles;
	if (!dongle_take(&dongles[first], coder))
		return (0);
	log_event(coder->sim, coder->id, "has taken a dongle");
	if (second == -1)
		return (1);
	if (!dongle_take(&dongles[second], coder))
	{
		dongle_release(&dongles[first], coder);
		return (0);
	}
	log_event(coder->sim, coder->id, "has taken a dongle");
	return (1);
}

static void	release_dongles(t_coder *coder)
{
	t_dongle	*dongles;
	int			first;
	int			second;

	dongle_indices(coder, &first, &second);
	dongles = coder->sim->dongles;
	dongle_release(&dongles[first], coder);
	if (second != -1)
		dongle_release(&dongles[second], coder);
}

static void	coder_compile(t_coder *coder)
{
	coder->last_compile_ms = time_now_ms();
	coder->deadline_ms = coder->last_compile_ms + coder->sim->args.burnout_ms;
	log_event(coder->sim, coder->id, "is compiling");
	time_sleep_ms(coder->sim->args.compile_ms);
	coder->n_compiled++;
}

static void	coder_debug(t_coder *coder)
{
	log_event(coder->sim, coder->id, "is debugging");
	time_sleep_ms(coder->sim->args.debug_ms);
}

static void	coder_refactor(t_coder *coder)
{
	log_event(coder->sim, coder->id, "is refactoring");
	time_sleep_ms(coder->sim->args.refactor_ms);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!sim_stopped(coder->sim))
	{
		if (!acquire_dongles(coder))
			break ;
		if (sim_stopped(coder->sim))
		{
			release_dongles(coder);
			break ;
		}
		coder_compile(coder);
		release_dongles(coder);
		if (coder->n_compiled >= coder->sim->args.n_compiles)
			break ;
		if (!sim_stopped(coder->sim))
			coder_debug(coder);
		if (!sim_stopped(coder->sim))
			coder_refactor(coder);
	}
	return (NULL);
}
