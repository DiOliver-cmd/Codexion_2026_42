/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_ops.c                                        +:+      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dilferre <dilferre@student.42sp.org.br>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 10:00:00 by dilferre  #+#    #+#             */
/*   Updated: 2026/09/25 10:00:00 by dilferre  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	compare_requests(t_request *a, t_request *b, t_scheduler type)
{
	if (type == SCHED_TYPE_FIFO)
	{
		if (a->arrival_time < b->arrival_time)
			return (-1);
		if (a->arrival_time > b->arrival_time)
			return (1);
		return (0);
	}
	if (a->deadline < b->deadline)
		return (-1);
	if (a->deadline > b->deadline)
		return (1);
	if (a->arrival_time < b->arrival_time)
		return (-1);
	if (a->arrival_time > b->arrival_time)
		return (1);
	return (0);
}

void	heapify_up(t_heap *heap, int idx)
{
	int			parent;
	t_request	*tmp;

	while (idx > 0)
	{
		parent = (idx - 1) / 2;
		if (compare_requests(heap->data[idx], heap->data[parent],
				heap->type) >= 0)
			break ;
		tmp = heap->data[idx];
		heap->data[idx] = heap->data[parent];
		heap->data[parent] = tmp;
		idx = parent;
	}
}

void	heapify_down(t_heap *heap, int idx)
{
	int			smallest;
	int			left;
	int			right;
	t_request	*tmp;

	while (1)
	{
		smallest = idx;
		left = 2 * idx + 1;
		right = 2 * idx + 2;
		if (left < heap->size && compare_requests(heap->data[left],
				heap->data[smallest], heap->type) < 0)
			smallest = left;
		if (right < heap->size && compare_requests(heap->data[right],
				heap->data[smallest], heap->type) < 0)
			smallest = right;
		if (smallest == idx)
			break ;
		tmp = heap->data[idx];
		heap->data[idx] = heap->data[smallest];
		heap->data[smallest] = tmp;
		idx = smallest;
	}
}
