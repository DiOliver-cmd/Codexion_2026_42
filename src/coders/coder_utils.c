#include "codexion.h"

static void	acquire_first_dongle(t_coder *coder, t_dongle *first, t_dongle *second)
{
	wait_for_dongle(first, coder);
	log_state(coder->sim, coder->id, STATE_TAKEN_DONGLE);
	wait_for_dongle(second, coder);
	log_state(coder->sim, coder->id, STATE_TAKEN_DONGLE);
}

void	acquire_dongles(t_coder *coder)
{
	if (coder->sim->num_coders == 1)
	{
		wait_for_dongle(coder->left_dongle, coder);
		log_state(coder->sim, coder->id, STATE_TAKEN_DONGLE);
		return ;
	}
	if (coder->left_dongle->id < coder->right_dongle->id)
		acquire_first_dongle(coder, coder->left_dongle, coder->right_dongle);
	else
		acquire_first_dongle(coder, coder->right_dongle, coder->left_dongle);
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

void	wait_for_dongle(t_dongle *dongle, t_coder *coder)
{
	t_request	*req;
	long		deadline;

	pthread_mutex_lock(&dongle->mutex);
	deadline = coder->deadline;
	req = create_request(coder->id, deadline, get_current_time_ms());
	enqueue_request(dongle->heap, req);
	while (1)
	{
		pthread_mutex_lock(&coder->sim->stop_mutex);
		if (coder->sim->stop_simulation)
		{
			pthread_mutex_unlock(&coder->sim->stop_mutex);
			dequeue_request(dongle->heap);
			pthread_mutex_unlock(&dongle->mutex);
			return ;
		}
		pthread_mutex_unlock(&coder->sim->stop_mutex);
		if (try_acquire_dongle(dongle, coder)
			&& is_request_at_front(dongle->heap, coder->id))
			break ;
		if (!dongle->available)
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
		else
			wait_for_cooldown(dongle);
	}
	dequeue_request(dongle->heap);
	pthread_mutex_unlock(&dongle->mutex);
}