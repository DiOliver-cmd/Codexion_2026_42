#include "codexion.h"

void	log_state(t_simulation *sim, int coder_id, t_state state)
{
	long	timestamp;
	long	now;

	pthread_mutex_lock(&sim->log_mutex);
	now = get_current_time_ms();
	timestamp = now - sim->start_time;
	if (state == STATE_TAKEN_DONGLE)
		printf("%ld %d has taken a dongle\n", timestamp, coder_id);
	else if (state == STATE_COMPILING)
		printf("%ld %d is compiling\n", timestamp, coder_id);
	else if (state == STATE_DEBUGGING)
		printf("%ld %d is debugging\n", timestamp, coder_id);
	else if (state == STATE_REFACTORING)
		printf("%ld %d is refactoring\n", timestamp, coder_id);
	else if (state == STATE_BURNED_OUT)
		printf("%ld %d burned out\n", timestamp, coder_id);
	pthread_mutex_unlock(&sim->log_mutex);
}

void	log_error(const char *msg)
{
	fprintf(stderr, "Error: %s\n", msg);
}

void	init_log_mutex(pthread_mutex_t *mutex)
{
	pthread_mutex_init(mutex, NULL);
}

void	destroy_log_mutex(pthread_mutex_t *mutex)
{
	pthread_mutex_destroy(mutex);
}