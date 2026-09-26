/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_utils.c                                      +:+      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dilferre <dilferre@student.42sp.org.br>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 10:00:00 by dilferre  #+#    #+#             */
/*   Updated: 2026/09/25 10:00:00 by dilferre  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_current_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

long	get_timestamp_ms(t_simulation *sim)
{
	struct timeval	tv;
	long			now;

	gettimeofday(&tv, NULL);
	now = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return (now - sim->start_time);
}

void	precise_sleep(long ms)
{
	struct timespec	req;
	struct timespec	rem;

	req.tv_sec = ms / 1000;
	req.tv_nsec = (ms % 1000) * 1000000;
	while (nanosleep(&req, &rem) == -1)
		req = rem;
}

long	calculate_deadline(t_simulation *sim, long last_compile)
{
	return (last_compile + sim->time_to_burnout);
}

void	update_coder_deadline(t_coder *coder, t_simulation *sim)
{
	coder->last_compile_start = get_timestamp_ms(sim);
	coder->deadline = calculate_deadline(sim, coder->last_compile_start);
}
