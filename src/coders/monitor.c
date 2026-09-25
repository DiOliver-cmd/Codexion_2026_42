#include "codexion.h"

bool	is_deadline_passed(t_coder *coder, long current_time)
{
	return (current_time >= coder->deadline);
}

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;
	long			current_time;
	int				i;

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
		i = 0;
		while (i < sim->num_coders)
		{
			if (is_deadline_passed(&sim->coders[i], current_time))
			{
				log_state(sim, sim->coders[i].id, STATE_BURNED_OUT);
				pthread_mutex_lock(&sim->stop_mutex);
				sim->stop_simulation = true;
				pthread_cond_broadcast(&sim->stop_cond);
				pthread_mutex_unlock(&sim->stop_mutex);
				return (NULL);
			}
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}

void	check_burnout(t_simulation *sim)
{
	long	current_time;
	int		i;

	current_time = get_timestamp_ms(sim);
	i = 0;
	while (i < sim->num_coders)
	{
		if (is_deadline_passed(&sim->coders[i], current_time))
		{
			log_state(sim, sim->coders[i].id, STATE_BURNED_OUT);
			pthread_mutex_lock(&sim->stop_mutex);
			sim->stop_simulation = true;
			pthread_mutex_unlock(&sim->stop_mutex);
			return ;
		}
		i++;
	}
}