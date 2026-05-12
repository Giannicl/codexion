#include "codexion.h"

long	time_now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

void	time_sleep_ms(long ms)
{
	long	end;

	end = time_now_ms() + ms;
	while (time_now_ms() < end)
		usleep(500);
}
