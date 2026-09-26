/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                     +:+      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dilferre <dilferre@student.42sp.org.br>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 10:00:00 by dilferre  #+#    #+#             */
/*   Updated: 2026/09/25 10:00:00 by dilferre  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	acquire_dongles(t_coder *coder)
{
	if (coder->sim->num_coders == 1)
	{
		wait_for_dongle(coder->left_dongle, coder);
		log_state(coder->sim, coder->id, STATE_TAKEN_DONGLE);
		return ;
	}
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		wait_for_dongle(coder->left_dongle, coder);
		log_state(coder->sim, coder->id, STATE_TAKEN_DONGLE);
		wait_for_dongle(coder->right_dongle, coder);
		log_state(coder->sim, coder->id, STATE_TAKEN_DONGLE);
	}
	else
	{
		wait_for_dongle(coder->right_dongle, coder);
		log_state(coder->sim, coder->id, STATE_TAKEN_DONGLE);
		wait_for_dongle(coder->left_dongle, coder);
		log_state(coder->sim, coder->id, STATE_TAKEN_DONGLE);
	}
}

void	release_dongles(t_coder *coder)
{
	long	now;

	now = get_current_time_ms();
	pthread_mutex_lock(&coder->left_dongle->mutex);
	coder->left_dongle->available = true;
	coder->left_dongle->cooldown_end = now + coder->sim->dongle_cooldown;
	if (coder->left_dongle->heap->size > 0)
		pthread_cond_signal(&coder->left_dongle->cond);
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	if (coder->sim->num_coders > 1)
	{
		pthread_mutex_lock(&coder->right_dongle->mutex);
		coder->right_dongle->available = true;
		coder->right_dongle->cooldown_end = now + coder->sim->dongle_cooldown;
		if (coder->right_dongle->heap->size > 0)
			pthread_cond_signal(&coder->right_dongle->cond);
		pthread_mutex_unlock(&coder->right_dongle->mutex);
	}
}

bool	try_acquire_dongle(t_dongle *dongle, t_coder *coder)
{
	long	now;

	(void)coder;
	now = get_current_time_ms();
	if (dongle->available && now >= dongle->cooldown_end)
	{
		dongle->available = false;
		return (true);
	}
	return (false);
}

static void	wait_dongle_signal(t_dongle *dongle)
{
	if (!dongle->available)
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	else
		wait_for_cooldown(dongle);
}

void	wait_for_dongle(t_dongle *dongle, t_coder *coder)
{
	t_request	*req;

	pthread_mutex_lock(&dongle->mutex);
	req = create_request(coder->id, coder->deadline, get_current_time_ms());
	enqueue_request(dongle->heap, req);
	while (1)
	{
		pthread_mutex_lock(&coder->sim->stop_mutex);
		if (coder->sim->stop_simulation)
		{
			pthread_mutex_unlock(&coder->sim->stop_mutex);
			free(dequeue_request(dongle->heap));
			pthread_mutex_unlock(&dongle->mutex);
			return ;
		}
		pthread_mutex_unlock(&coder->sim->stop_mutex);
		if (try_acquire_dongle(dongle, coder)
			&& is_request_at_front(dongle->heap, coder->id))
			break ;
		wait_dongle_signal(dongle);
	}
	free(dequeue_request(dongle->heap));
	pthread_mutex_unlock(&dongle->mutex);
}
