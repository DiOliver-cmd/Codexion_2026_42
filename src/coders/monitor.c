/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                         +:+      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dilferre <dilferre@student.42sp.org.br>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 10:00:00 by dilferre  #+#    #+#             */
/*   Updated: 2026/09/25 10:00:00 by dilferre  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	is_deadline_passed(t_coder *coder, long current_time)
{
	return (current_time >= coder->deadline);
}

static bool	check_coder_burnout(t_simulation *sim, int i, long current_time)
{
	if (is_deadline_passed(&sim->coders[i], current_time))
	{
		log_state(sim, sim->coders[i].id, STATE_BURNED_OUT);
		pthread_mutex_lock(&sim->stop_mutex);
		sim->stop_simulation = true;
		pthread_cond_broadcast(&sim->stop_cond);
		pthread_mutex_unlock(&sim->stop_mutex);
		return (true);
	}
	return (false);
}

static void	check_all_coders(t_simulation *sim, long current_time)
{
	int	i;

	i = 0;
	while (i < sim->num_coders)
	{
		if (check_coder_burnout(sim, i, current_time))
			return ;
		i++;
	}
}

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;
	long			current_time;

	sim = (t_simulation *)arg;
	while (1)
	{
		pthread_mutex_lock(&sim->stop_mutex);
		if (sim->stop_simulation)
		{
			pthread_mutex_unlock(&sim->stop_mutex);
			break ;
		}
		pthread_mutex_unlock(&sim->stop_mutex);
		current_time = get_timestamp_ms(sim);
		check_all_coders(sim, current_time);
		usleep(1000);
	}
	return (NULL);
}
