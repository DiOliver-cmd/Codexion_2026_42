#include "codexion.h"

t_heap	*create_heap(int capacity, t_scheduler type)
{
	t_heap	*heap;

	heap = malloc(sizeof(t_heap));
	if (!heap)
		return (NULL);
	heap->data = malloc(sizeof(t_request *) * capacity);
	if (!heap->data)
	{
		free(heap);
		return (NULL);
	}
	heap->size = 0;
	heap->capacity = capacity;
	heap->type = type;
	return (heap);
}

void	destroy_heap(t_heap *heap)
{
	int	i;

	if (!heap)
		return ;
	i = 0;
	while (i < heap->size)
	{
		free(heap->data[i]);
		i++;
	}
	free(heap->data);
	free(heap);
}

t_request	*create_request(int coder_id, long deadline, long arrival)
{
	t_request	*req;

	req = malloc(sizeof(t_request));
	if (!req)
		return (NULL);
	req->coder_id = coder_id;
	req->deadline = deadline;
	req->arrival_time = arrival;
	req->next = NULL;
	return (req);
}

void	enqueue_request(t_heap *heap, t_request *req)
{
	int	idx;

	if (heap->size >= heap->capacity)
		return ;
	idx = heap->size;
	heap->data[idx] = req;
	heap->size++;
	heapify_up(heap, idx);
}

t_request	*dequeue_request(t_heap *heap)
{
	t_request	*min;

	if (heap->size == 0)
		return (NULL);
	min = heap->data[0];
	heap->data[0] = heap->data[heap->size - 1];
	heap->size--;
	if (heap->size > 0)
		heapify_down(heap, 0);
	return (min);
}