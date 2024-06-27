Enescu Maria 321CA
# Parallel Graph

## Objectives

- Learn how to design and implement parallel programs
- Gain skills in using synchronization primitives for parallel programs
- Get a better understanding of the POSIX threading and synchronization API
- Gain insight on the differences between serial and parallel programs

## Statement

Implement a generic thread pool, then use it to traverse a graph and compute the sum of the elements contained by the nodes.
You will be provided with a serial implementation of the graph traversal and with most of the data structures needed to implement the thread pool.
Your job is to write the thread pool routines and then use the thread pool to traverse the graph.

## Implementation

### Thread Pool Description

A thread pool contains a given number of active threads that simply wait to be given specific tasks.
The threads are created when the thread pool is created.
Each thread continuously polls the task queue for available tasks.
Once tasks are put in the task queue, the threads poll tasks, and start running them.
A thread pool creates **N** threads upon its creation and does not destroy (join) them throughout its lifetime.
That way, the penalty of creating and destroying threads ad-hoc is avoided.
As such, you must implement the following functions (in `os_threadpool.c`):

- `enqueue_task()`: Enqueue task to the shared task queue.
  Use synchronization.
- `dequeue_task()`: Dequeue task from the shared task queue.
  Use synchronization.
- `wait_for_completion()`: Wait for all worker threads.
  Use synchronization.
- `create_threadpool()`: Create a new thread pool.
- `destroy_threadpool()`: Destroy a thread pool.
  Assume all threads have been joined.

You must also update the `os_threadpool_t` structure in `os_threadpool.h` with the required bits for synchronizing the parallel implementation.

Notice that the thread pool is completely independent of any given application.
Any function can be registered in the task queue.

Since the threads are polling the task queue indefinitely, you need to define a condition for them to stop once the graph has been traversed completely.
That is, the condition used by the `wait_for_completion()` function.
The recommended way is to note when no threads have any more work to do.
Since no thread is doing any work, no other task will be created.

### Graph Traversal

Once you have implemented the thread pool, you need to test it by doing a parallel traversal of all connected nodes in a graph.
A serial implementation for this algorithm is provided in `serial.c`.
To make use of the thread pool, you will need to create tasks that will be put in the task queue.
A task consists of 2 steps:

1. Add the current node value to the overall sum.
1. Create tasks and add them to the task queue for the neighbouring nodes.

Implement this in the `parallel.c`.
You must implement the parallel and synchronized version of the `process_node()` function, also used in the serial implementation.

### Synchronization

For synchronization you can use mutexes, semaphores, spinlocks, condition variables - anything that grinds your gear.
However, you are not allowed to use hacks such as `sleep()`, `printf()` synchronization or adding superfluous computation.

### Input Files

Reading the graphs from the input files is being taken care of the functions implemented in `os_graph.c`.
A graph is represented in input files as follows:

- First line contains 2 integers `N` and `M`: `N` - number of nodes, `M` - numbed or edges
- Second line contains `N` integer numbers - the values of the nodes.
- The next `M` lines contain each 2 integers that represent the source and the destination of an edge.

### Data Structures

#### Graph

A graph is represented internally by the `os_graph_t` structure (see `os_graph.h`).

#### List

A list is represented internally by the `os_queue_t` structure (see `os_list.h`).
You will use this list to implement the task queue.

#### Thread Pool

A thread pool is represented internally by the `os_threadpool_t` structure (see `os_threadpool.h`).
The thread pool contains information about the task queue and the threads.

### Requirements

Your implementation needs to be contained in the `os_threadpool.c`, `os_threadpool.h` and `parallel.c` files.

### Building

To build both the serial and the parallel versions, run `make`.
