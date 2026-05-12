#include "codexion.h"

void	log_event(t_sim *sim, int id, const char *event)
{
	long	ts;

	ts = time_now_ms() - sim->start_ms;
	pthread_mutex_lock(&sim->log_mutex);
	printf("%ld %d %s\n", ts, id, event);
	pthread_mutex_unlock(&sim->log_mutex);
}
