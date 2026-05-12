#include "codexion.h"

static int	is_uint_str(const char *s)
{
	int	i;

	if (!s || !s[0])
		return (0);
	i = 0;
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	parse_scheduler(const char *s, int *out)
{
	if (strcmp(s, "fifo") == 0)
	{
		*out = FIFO;
		return (1);
	}
	if (strcmp(s, "edf") == 0)
	{
		*out = EDF;
		return (1);
	}
	return (0);
}

static int	validate(t_args *args)
{
	if (args->n_coders < 1 || args->n_compiles < 1)
		return (0);
	if (args->burnout_ms < 0 || args->compile_ms < 0)
		return (0);
	if (args->debug_ms < 0 || args->refactor_ms < 0)
		return (0);
	if (args->cooldown_ms < 0)
		return (0);
	return (1);
}

static int	all_numeric(char **argv)
{
	int	i;

	i = 1;
	while (i <= 7)
	{
		if (!is_uint_str(argv[i]))
			return (0);
		i++;
	}
	return (1);
}

static int	parse_error(const char *msg)
{
	fprintf(stderr, "Error: %s\n", msg);
	return (0);
}

int	parse_args(int argc, char **argv, t_args *args)
{
	if (argc != 9)
	{
		fprintf(stderr, "Usage: ./codexion n_coders burnout_ms compile_ms"
			" debug_ms refactor_ms n_compiles cooldown_ms scheduler\n");
		return (0);
	}
	if (!all_numeric(argv))
		return (parse_error("numeric args must be non-negative integers"));
	args->n_coders = atoi(argv[1]);
	args->burnout_ms = atoi(argv[2]);
	args->compile_ms = atoi(argv[3]);
	args->debug_ms = atoi(argv[4]);
	args->refactor_ms = atoi(argv[5]);
	args->n_compiles = atoi(argv[6]);
	args->cooldown_ms = atoi(argv[7]);
	if (!parse_scheduler(argv[8], &args->scheduler))
		return (parse_error("scheduler must be 'fifo' or 'edf'"));
	if (!validate(args))
		return (parse_error("invalid argument values"));
	return (1);
}
