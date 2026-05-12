#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

# define FIFO 0
# define EDF  1

typedef struct s_args
{
	int		n_coders;
	long	burnout_ms;
	long	compile_ms;
	long	debug_ms;
	long	refactor_ms;
	int		n_compiles;
	long	cooldown_ms;
	int		scheduler;
}	t_args;

typedef struct s_node
{
	int		id;
	long	priority;
}	t_node;

typedef struct s_heap
{
	t_node	*data;
	int		size;
	int		cap;
}	t_heap;

typedef struct s_sim	t_sim;

typedef struct s_dongle
{
	int				id;
	int				available;
	long			ready_at;
	long			fifo_seq;
	t_heap			queue;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
}	t_dongle;

typedef struct s_coder
{
	int		id;
	int		n_compiled;
	long	last_compile_ms;
	long	deadline_ms;
	t_sim	*sim;
}	t_coder;

struct s_sim
{
	t_args			args;
	t_dongle		*dongles;
	int				stop;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	log_mutex;
	long			start_ms;
	void			*mon_args[2];
};

/* utils.c */
long	time_now_ms(void);
void	time_sleep_ms(long ms);

/* heap.c */
int		heap_init(t_heap *h, int cap);
void	heap_destroy(t_heap *h);
void	heap_push(t_heap *h, int id, long priority);
void	heap_remove(t_heap *h, int id);
int		heap_peek_id(t_heap *h);

/* log.c */
void	log_event(t_sim *sim, int id, const char *event);

/* simulation.c */
int		sim_init(t_sim *sim);
void	sim_cleanup(t_sim *sim);
void	sim_stop(t_sim *sim);
int		sim_stopped(t_sim *sim);

/* dongle.c */
int		dongle_init(t_dongle *d, int id, int cap);
void	dongle_destroy(t_dongle *d);
int		dongle_take(t_dongle *d, t_coder *coder);
void	dongle_release(t_dongle *d, t_coder *coder);

/* parse.c */
int		parse_args(int argc, char **argv, t_args *args);

/* coder.c */
void	*coder_routine(void *arg);

/* monitor.c */
void	*monitor_routine(void *arg);

#endif
