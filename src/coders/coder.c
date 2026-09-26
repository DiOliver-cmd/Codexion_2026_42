/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                           +:+      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dilferre <dilferre@student.42sp.org.br>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 10:00:00 by dilferre  #+#    #+#             */
/*   Updated: 2026/09/25 10:00:00 by dilferre  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	check_stop_simulation(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->stop_mutex);
	if (coder->sim->stop_simulation)
	{
		pthread_mutex_unlock(&coder->sim->stop_mutex);
		return (true);
	}
	pthread_mutex_unlock(&coder->sim->stop_mutex);
	return (false);
}

static void	perform_compile_cycle(t_coder *coder)
{
	acquire_dongles(coder);
	log_state(coder->sim, coder->id, STATE_COMPILING);
	update_coder_deadline(coder, coder->sim);
	precise_sleep(coder->sim->time_to_compile);
	release_dongles(coder);
	coder->compile_count++;
}

static void	perform_debug_and_refactor(t_coder *coder)
{
	log_state(coder->sim, coder->id, STATE_DEBUGGING);
	precise_sleep(coder->sim->time_to_debug);
	log_state(coder->sim, coder->id, STATE_REFACTORING);
	precise_sleep(coder->sim->time_to_refactor);
}

static bool	check_completion(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->stop_mutex);
	if (coder->compile_count >= coder->sim->compiles_required)
	{
		coder->sim->finished_coders++;
		if (coder->sim->finished_coders >= coder->sim->num_coders)
			coder->sim->stop_simulation = true;
		pthread_mutex_unlock(&coder->sim->stop_mutex);
		return (true);
	}
	pthread_mutex_unlock(&coder->sim->stop_mutex);
	return (false);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (1)
	{
		if (check_stop_simulation(coder))
			break ;
		perform_compile_cycle(coder);
		perform_debug_and_refactor(coder);
		if (check_completion(coder))
			break ;
	}
	return (NULL);
}
