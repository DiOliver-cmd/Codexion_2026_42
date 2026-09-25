#include "codexion.h"

void	init_dongle(t_dongle *dongle, int id, t_scheduler type)
{
	pthread_mutex_init(&dongle->mutex, NULL);
	pthread_cond_init(&dongle->cond, NULL);
	dongle->available = true;
	dongle->cooldown_end = 0;
	dongle->id = id;
	dongle->heap = create_heap(100, type);
}

void	destroy_dongle(t_dongle *dongle)
{
	destroy_heap(dongle->heap);
	pthread_mutex_destroy(&dongle->mutex);
	pthread_cond_destroy(&dongle->cond);
}

void	init_dongles(t_simulation *sim)
{
	int	i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->num_coders);
	if (!sim->dongles)
		return ;
	i = 0;
	while (i < sim->num_coders)
	{
		init_dongle(&sim->dongles[i], i, sim->scheduler);
		i++;
	}
}

void	destroy_dongles(t_simulation *sim)
{
	int	i;

	if (!sim->dongles)
		return ;
	i = 0;
	while (i < sim->num_coders)
	{
		destroy_dongle(&sim->dongles[i]);
		i++;
	}
	free(sim->dongles);
}

void	assign_dongles(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->num_coders)
	{
		sim->coders[i].left_dongle = &sim->dongles[i];
		if (sim->num_coders == 1)
			sim->coders[i].right_dongle = &sim->dongles[i];
		else
			sim->coders[i].right_dongle = &sim->dongles[(i + 1) % sim->num_coders];
		i++;
	}
}