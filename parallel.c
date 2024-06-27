// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "os_graph.h"
#include "os_threadpool.h"
#include "log/log.h"
#include "utils.h"

#define NUM_THREADS 4

static int sum;
static os_graph_t *graph;
static os_threadpool_t *tp;

pthread_mutex_t sum_mutex;
pthread_mutex_t graph_mutex;

static void process_node(unsigned int idx)
{
	pthread_mutex_lock(&graph_mutex);

	// Check if the current node has not been visited yet.
	if (graph->visited[idx] == NOT_VISITED) {
		graph->visited[idx] = PROCESSING;
		pthread_mutex_unlock(&graph_mutex);

		os_node_t *current_node = graph->nodes[idx];

		// Add the current_node's value to the global sum.
		pthread_mutex_lock(&sum_mutex); // lock the mutex to protect sum update
		sum += current_node->info;
		pthread_mutex_unlock(&sum_mutex);

		// Create tasks for each unvisited neighbour.
		for (unsigned int i = 0; i < current_node->num_neighbours; i++) {
			pthread_mutex_lock(&graph_mutex); // lock the mutex to check neighbour's state
			if (graph->visited[current_node->neighbours[i]] == NOT_VISITED) {
				pthread_mutex_unlock(&graph_mutex);
				os_task_t *task = create_task((void *)process_node, (unsigned int *)current_node->neighbours[i], NULL);

				enqueue_task(tp, task);
			} else {
				pthread_mutex_unlock(&graph_mutex);
			}
		}
		pthread_mutex_lock(&graph_mutex);
		graph->visited[idx] = DONE;
	}
	pthread_mutex_unlock(&graph_mutex);
}

int main(int argc, char *argv[])
{
	FILE *input_file;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	input_file = fopen(argv[1], "r");
	DIE(input_file == NULL, "fopen");

	graph = create_graph_from_file(input_file);

	// Initialize all nodes as not visited.
	for (unsigned int i = 0; i < graph->num_nodes; i++)
		graph->visited[i] = NOT_VISITED;

	// Initialize synchronization mechanisms.
	pthread_mutex_init(&sum_mutex, NULL);
	pthread_mutex_init(&graph_mutex, NULL);

	tp = create_threadpool(NUM_THREADS);

	// Creating a task for process_node(0) and adding the task to the threadpool's queue.
	if (graph->num_nodes > 0) {
		os_task_t *task = create_task((void *)(void *)process_node, 0, NULL);

		enqueue_task(tp, task);
	}

	wait_for_completion(tp);
	destroy_threadpool(tp);

	// Destroy synchronization mechanisms.
	pthread_mutex_destroy(&sum_mutex);
	pthread_mutex_destroy(&graph_mutex);

	printf("%d", sum);

	return 0;
}
