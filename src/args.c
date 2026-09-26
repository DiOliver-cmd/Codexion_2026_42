/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.c                                            +:+      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dilferre <dilferre@student.42sp.org.br>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 10:00:00 by dilferre  #+#    #+#             */
/*   Updated: 2026/09/25 10:00:00 by dilferre  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	is_valid_number(char *str)
{
	int	i;

	i = 0;
	if (!str || !str[0])
		return (false);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (false);
		i++;
	}
	return (true);
}

static bool	parse_scheduler(char *str, t_scheduler *sched)
{
	if (strcmp(str, "fifo") == 0)
		*sched = SCHED_TYPE_FIFO;
	else if (strcmp(str, "edf") == 0)
		*sched = SCHED_TYPE_EDF;
	else
		return (false);
	return (true);
}

static void	set_sim_values(t_simulation *sim, char **argv)
{
	sim->num_coders = atoi(argv[1]);
	sim->time_to_burnout = atol(argv[2]);
	sim->time_to_compile = atol(argv[3]);
	sim->time_to_debug = atol(argv[4]);
	sim->time_to_refactor = atol(argv[5]);
	sim->compiles_required = atoi(argv[6]);
	sim->dongle_cooldown = atol(argv[7]);
}

void	parse_args(int argc, char **argv, t_simulation *sim)
{
	if (argc != 9)
	{
		log_error("Usage: ./codexion n_coders t_burnout t_compile "
			"t_debug t_refactor n_compiles cooldown scheduler");
		exit(1);
	}
	if (!is_valid_number(argv[1]) || !is_valid_number(argv[2])
		|| !is_valid_number(argv[3]) || !is_valid_number(argv[4])
		|| !is_valid_number(argv[5]) || !is_valid_number(argv[6])
		|| !is_valid_number(argv[7]))
	{
		log_error("All numeric arguments must be positive integers");
		exit(1);
	}
	set_sim_values(sim, argv);
	if (!parse_scheduler(argv[8], &sim->scheduler))
	{
		log_error("Scheduler must be 'fifo' or 'edf'");
		exit(1);
	}
}

bool	validate_args(t_simulation *sim)
{
	if (sim->num_coders <= 0 || sim->time_to_burnout <= 0
		|| sim->time_to_compile <= 0 || sim->time_to_debug <= 0
		|| sim->time_to_refactor <= 0 || sim->compiles_required <= 0
		|| sim->dongle_cooldown < 0)
		return (false);
	return (true);
}
