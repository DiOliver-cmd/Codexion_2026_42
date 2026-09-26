/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                        +:+      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dilferre <dilferre@student.42sp.org.br>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 10:00:00 by dilferre  #+#    #+#             */
/*   Updated: 2026/09/25 10:00:00 by dilferre  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include "types.h"
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <unistd.h>

int				main(int argc, char **argv);
void			parse_args(int argc, char **argv, t_simulation *sim);
bool			validate_args(t_simulation *sim);
void			init_simulation(t_simulation *sim);
void			cleanup_simulation(t_simulation *sim);
void			*coder_routine(void *arg);
void			*monitor_routine(void *arg);
void			log_state(t_simulation *sim, int coder_id, t_state state);
long			get_timestamp_ms(t_simulation *sim);
void			acquire_dongles(t_coder *coder);
void			release_dongles(t_coder *coder);
bool			try_acquire_dongle(t_dongle *dongle, t_coder *coder);
void			wait_for_dongle(t_dongle *dongle, t_coder *coder);
void			enqueue_request(t_heap *heap, t_request *req);
t_request		*dequeue_request(t_heap *heap);
void			heapify_up(t_heap *heap, int idx);
void			heapify_down(t_heap *heap, int idx);
int				compare_requests(t_request *a, t_request *b, t_scheduler type);
long			get_current_time_ms(void);
void			precise_sleep(long ms);
void			destroy_heap(t_heap *heap);
t_heap			*create_heap(int capacity, t_scheduler type);
t_request		*create_request(int coder_id, long deadline, long arrival);
bool			is_deadline_passed(t_coder *coder, long current_time);
bool			is_request_at_front(t_heap *heap, int coder_id);
void			wait_for_cooldown(t_dongle *dongle);
long			calculate_deadline(t_simulation *sim, long last_compile);
void			update_coder_deadline(t_coder *coder, t_simulation *sim);
void			log_error(const char *msg);
void			init_log_mutex(pthread_mutex_t *mutex);
void			destroy_log_mutex(pthread_mutex_t *mutex);
void			init_dongle(t_dongle *dongle, int id, t_scheduler type);
void			destroy_dongle(t_dongle *dongle);
void			init_dongles(t_simulation *sim);
void			destroy_dongles(t_simulation *sim);
void			assign_dongles(t_simulation *sim);
void			start_coders(t_simulation *sim);
void			start_monitor(t_simulation *sim, pthread_t *monitor);
void			wait_for_completion(t_simulation *sim, pthread_t monitor);
void			broadcast_all_dongles(t_simulation *sim);

#endif