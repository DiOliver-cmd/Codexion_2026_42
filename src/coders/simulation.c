/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                      +:+      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dilferre <dilferre@student.42sp.org.br>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 10:00:00 by dilferre  #+#    #+#             */
/*   Updated: 2026/09/25 10:00:00 by dilferre  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	init_simulation(t_simulation *sim)
{
	int	i;

	sim->coders = malloc(sizeof(t_coder) * sim->num_coders);
	if (!sim->coders)
		return ;
	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_mutex_init(&sim->stop_mutex, NULL);
	pthread_cond_init(&sim->stop_cond, NULL);
	sim->stop_simulation = false;
	sim->start_time = get_current_time_ms();
	sim->finished_coders = 0;
	init_dongles(sim);
	assign_dongles(sim);
	i = 0;
	while (i < sim->num_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compile_count = 0;
		sim->coders[i].last_compile_start = 0;
		sim->coders[i].deadline = sim->time_to_burnout;
		sim->coders[i].sim = sim;
		i++;
	}
}

void	cleanup_simulation(t_simulation *sim)
{
	int	i;

	if (sim->coders)
	{
		i = 0;
		while (i < sim->num_coders)
		{
			pthread_join(sim->coders[i].thread, NULL);
			i++;
		}
		free(sim->coders);
	}
	destroy_dongles(sim);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_cond_destroy(&sim->stop_cond);
}

void	start_coders(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->num_coders)
	{
		pthread_create(&sim->coders[i].thread, NULL, coder_routine,
			&sim->coders[i]);
		i++;
	}
}

void	start_monitor(t_simulation *sim, pthread_t *monitor)
{
	pthread_create(monitor, NULL, monitor_routine, sim);
}

void	wait_for_completion(t_simulation *sim, pthread_t monitor)
{
	pthread_join(monitor, NULL);
	broadcast_all_dongles(sim);
}
