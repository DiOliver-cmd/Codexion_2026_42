#ifndef TYPES_H
# define TYPES_H

# include <pthread.h>
# include <stdbool.h>
# include <sys/time.h>

typedef enum e_state
{
	STATE_TAKEN_DONGLE,
	STATE_COMPILING,
	STATE_DEBUGGING,
	STATE_REFACTORING,
	STATE_BURNED_OUT
}	t_state;

typedef enum e_scheduler
{
	SCHED_TYPE_FIFO,
	SCHED_TYPE_EDF
}	t_scheduler;

typedef struct s_request	t_request;
typedef struct s_heap		t_heap;

typedef struct s_dongle
{
	pthread_mutex_t		mutex;
	pthread_cond_t		cond;
	bool				available;
	long				cooldown_end;
	int					id;
	t_heap				*heap;
}	t_dongle;

typedef struct s_request
{
	int					coder_id;
	long				deadline;
	long				arrival_time;
	struct s_request	*next;
}	t_request;

typedef struct s_heap
{
	t_request			**data;
	int					size;
	int					capacity;
	t_scheduler			type;
}	t_heap;

typedef struct s_coder
{
	int					id;
	pthread_t			thread;
	int					compile_count;
	long				last_compile_start;
	long				deadline;
	t_dongle			*left_dongle;
	t_dongle			*right_dongle;
	struct s_simulation	*sim;
}	t_coder;

typedef struct s_simulation
{
	int					num_coders;
	long				time_to_burnout;
	long				time_to_compile;
	long				time_to_debug;
	long				time_to_refactor;
	int					compiles_required;
	long				dongle_cooldown;
	t_scheduler			scheduler;
	t_coder				*coders;
	t_dongle			*dongles;
	pthread_mutex_t		log_mutex;
	pthread_mutex_t		stop_mutex;
	pthread_cond_t		stop_cond;
	bool				stop_simulation;
	long				start_time;
	int					finished_coders;
}	t_simulation;

#endif