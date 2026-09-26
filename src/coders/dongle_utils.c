/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_utils.c                                    +:+      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dilferre <dilferre@student.42sp.org.br>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 10:00:00 by dilferre  #+#    #+#             */
/*   Updated: 2026/09/25 10:00:00 by dilferre  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	is_request_at_front(t_heap *heap, int coder_id)
{
	if (heap->size == 0)
		return (false);
	return (heap->data[0]->coder_id == coder_id);
}

void	wait_for_cooldown(t_dongle *dongle)
{
	struct timespec	ts;
	long			now;
	long			wait_ms;

	now = get_current_time_ms();
	if (now >= dongle->cooldown_end)
		return ;
	wait_ms = dongle->cooldown_end - now;
	ts.tv_sec = wait_ms / 1000;
	ts.tv_nsec = (wait_ms % 1000) * 1000000;
	pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
}

void	broadcast_all_dongles(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->num_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}
