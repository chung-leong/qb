/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Chung Leong <cleong@cal.berkeley.edu>                        |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "qb.h"

#ifdef HAVE_PTHREAD

uint32_t qb_get_cpu_count() {
	static uint32_t count = 0;
	if(!count) {
#ifdef WIN32
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		count = sysinfo.dwNumberOfProcessors;
#elif defined(__linux__)
		count = sysconf( _SC_NPROCESSORS_ONLN );
#endif
	}
	return count;
}

int32_t qb_schedule_task(qb_thread_pool *pool, void *proc, void *param1, void *param2) {
	qb_thread_task *next_task;
	if(pool->task_count >= pool->task_buffer_size) {
		pool->task_buffer_size += 16;
		pool->tasks = perealloc(pool->tasks, sizeof(qb_thread_task) * pool->task_buffer_size, TRUE);
	}
	next_task = &pool->tasks[pool->task_count];
	next_task->proc = proc;
	next_task->param1 = param1;
	next_task->param2 = param2;
	next_task->assigned = FALSE;
	pool->task_count++;
	return TRUE;
}

#ifdef WIN32
DWORD WINAPI qb_run_task(LPVOID lpThreadParameter) {
	qb_task_assignment *assignment = lpThreadParameter;
	for(;;) {
		qb_thread_task *task = (qb_thread_task *) assignment->task;
		if(task) {
			task->proc(task->param1, task->param2);
			assignment->task = NULL;

			SetEvent(assignment->win32_completion_event);
		}
	}
	return 0;
}
#else
void *qb_run_task(void *arg) {
	qb_task_assignment *assignment = arg;
	for(;;) {
		qb_thread_task *task = (qb_thread_task *) assignment->task;
		if(task) {
			task->proc(task->param1, task->param2);
			assignment->task = NULL;

			pthread_mutex_lock(assignment->pthread_completion_mutex);
			*assignment->pthread_completion_state = TRUE;
			pthread_mutex_unlock(assignment->pthread_completion_mutex);
			pthread_cond_signal(assignment->pthread_completion_cond);
		} else {
			pthread_mutex_lock(assignment->pthread_wake_mutex);
			if(assignment->thread_suspended) {
				pthread_cond_wait(assignment->pthread_wake_cond, assignment->pthread_wake_mutex);
				pthread_mutex_unlock(assignment->pthread_wake_mutex);
			} else {
				pthread_mutex_unlock(assignment->pthread_wake_mutex);
			}
		}
	}
	return NULL;
}
#endif

int qb_initialize_thread_pool(qb_thread_pool *pool) {
	uint32_t i ;
	pool->task_buffer_size = 16;
	pool->tasks = malloc(sizeof(qb_thread_task) * pool->task_buffer_size);
	pool->task_count = 0;
	pool->thread_count = qb_get_cpu_count();
	pool->task_assignments = malloc(sizeof(qb_task_assignment) * pool->thread_count);

#ifdef WIN32
	pool->win32_completion_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	pool->win32_threads = malloc(sizeof(HANDLE) * pool->thread_count);
	for(i = 0; i < pool->thread_count; i++) {
		pool->win32_threads[i] = CreateThread(NULL, 0, qb_run_task, &pool->task_assignments[i], CREATE_SUSPENDED, NULL);
		pool->task_assignments[i].task = NULL;
		pool->task_assignments[i].thread_suspended = TRUE;
		pool->task_assignments[i].win32_completion_event = pool->win32_completion_event;
	}
#else
	pool->pthreads = malloc(sizeof(pthread_t) * pool->thread_count);
	pool->pthread_wake_conds = malloc(sizeof(pthread_cond_t) * pool->thread_count);
	pool->pthread_wake_mutexes = malloc(sizeof(pthread_mutex_t) * pool->thread_count);
	pthread_cond_init(&pool->pthread_completion_cond, NULL);
	pthread_mutex_init(&pool->pthread_completion_mutex, NULL);
	for(i = 0; i < pool->thread_count; i++) {
		pool->task_assignments[i].task = NULL;
		pool->task_assignments[i].thread_suspended = TRUE;
		pthread_cond_init(&pool->pthread_wake_conds[i], NULL);
		pthread_mutex_init(&pool->pthread_wake_mutexes[i], NULL);
		pool->task_assignments[i].pthread_wake_cond = &pool->pthread_wake_conds[i];
		pool->task_assignments[i].pthread_wake_mutex = &pool->pthread_wake_mutexes[i];
		pool->task_assignments[i].pthread_completion_cond = &pool->pthread_completion_cond;
		pool->task_assignments[i].pthread_completion_mutex = &pool->pthread_completion_mutex;
		pool->task_assignments[i].pthread_completion_state = (int32_t *) &pool->pthread_completion_state;
		pthread_create(&pool->pthreads[i], NULL, qb_run_task, &pool->task_assignments[i]);
	}
#endif
	return TRUE;
}

int qb_run_tasks(qb_thread_pool *pool) {
	uint32_t i, j, start_index = 0, active_thread_count = 0, suspended_thread_count = 0;
	// wake up the threads
	for(i = 0; i < pool->thread_count; i++) {
		if(i < pool->task_count) {
#ifdef WIN32
			pool->task_assignments[i].thread_suspended = FALSE;
			ResumeThread(pool->win32_threads[i]);
#else
			pthread_mutex_lock(&pool->pthread_wake_mutexes[i]);
			pool->task_assignments[i].thread_suspended = FALSE;
			pthread_mutex_unlock(&pool->pthread_wake_mutexes[i]);
			pthread_cond_signal(&pool->pthread_wake_conds[i]);
#endif
			active_thread_count++;
		}
	}

	// assign tasks to threads
	for(;;) {
		for(i = 0; i < active_thread_count; i++) {
			qb_task_assignment *assignment = &pool->task_assignments[i];
			if(!assignment->thread_suspended) {
				if(assignment->task == NULL) {
					int32_t assigned = FALSE;
					for(j = start_index; j < pool->task_count; j++) {
						qb_thread_task *next_task = &pool->tasks[j];
						if(!next_task->assigned) {
							next_task->assigned = TRUE;
							assignment->task = next_task;
							assigned = TRUE;
							break;
						} else {
							start_index++;
						}
					}

					// no more tasks remaining--suspend the thread
					if(!assigned) {
#ifdef WIN32
						assignment->thread_suspended = TRUE;
						SuspendThread(pool->win32_threads[i]);
#else
						pthread_mutex_lock(assignment->pthread_wake_mutex);
						assignment->thread_suspended = TRUE;
						pthread_mutex_unlock(assignment->pthread_wake_mutex);
#endif
						suspended_thread_count++;
					}
				}
			}
		}
		if(suspended_thread_count < active_thread_count) {
			// wait for a thread to complete
#ifdef WIN32
			WaitForSingleObject(pool->win32_completion_event, INFINITE);
#else
			pthread_mutex_lock(&pool->pthread_completion_mutex);
			if(!pool->pthread_completion_state) {
				pthread_cond_wait(&pool->pthread_completion_cond, &pool->pthread_completion_mutex);
				pool->pthread_completion_state = FALSE;
				pthread_mutex_unlock(&pool->pthread_completion_mutex);
			} else {
				pool->pthread_completion_state = FALSE;
				pthread_mutex_unlock(&pool->pthread_completion_mutex);
			}
#endif
		} else {
			// all the worker threads are suspended--time to leave
			break;
		}
	}
	return TRUE;
}

#endif
