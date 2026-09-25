#include "codexion.h"

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (1)
	{
		pthread_mutex_lock(&coder->sim->stop_mutex);
		if (coder->sim->stop_simulation)
		{
			pthread_mutex_unlock(&coder->sim->stop_mutex);
			break ;
		}
		pthread_mutex_unlock(&coder->sim->stop_mutex);
		acquire_dongles(coder);
		log_state(coder->sim, coder->id, STATE_COMPILING);
		update_coder_deadline(coder, coder->sim);
		precise_sleep(coder->sim->time_to_compile);
		release_dongles(coder);
		coder->compile_count++;
		log_state(coder->sim, coder->id, STATE_DEBUGGING);
		precise_sleep(coder->sim->time_to_debug);
		log_state(coder->sim, coder->id, STATE_REFACTORING);
		precise_sleep(coder->sim->time_to_refactor);
		pthread_mutex_lock(&coder->sim->stop_mutex);
		if (coder->compile_count >= coder->sim->compiles_required)
		{
			coder->sim->finished_coders++;
			if (coder->sim->finished_coders >= coder->sim->num_coders)
				coder->sim->stop_simulation = true;
			pthread_mutex_unlock(&coder->sim->stop_mutex);
			break ;
		}
		pthread_mutex_unlock(&coder->sim->stop_mutex);
	}
	return (NULL);
}