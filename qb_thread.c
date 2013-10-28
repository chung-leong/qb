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

#include "qb.h"

long qb_get_cpu_count(void) {
	static long count = 0;
	if(!count) {
#if defined(__linux__)
		count = sysconf( _SC_NPROCESSORS_ONLN );
#elif defined(WIN32)
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		count = sysinfo.dwNumberOfProcessors;
#endif
	}
	return count;
}

void qb_schedule_task(qb_thread_pool *pool, qb_thread_proc proc, void *param1, void *param2, int param3, qb_thread_worker **p_worker) {
	qb_thread_task *next_task;
	if(pool->task_count >= pool->task_buffer_size) {
		pool->task_buffer_size += 16;
		pool->tasks = realloc(pool->tasks, sizeof(qb_thread_task) * pool->task_buffer_size);
	}
	next_task = &pool->tasks[pool->task_count];
	next_task->proc = proc;
	next_task->param1 = param1;
	next_task->param2 = param2;
	next_task->param3 = param3;
	next_task->worker_pointer = p_worker;
	pool->task_count++;
}

void qb_run_in_main_thread(qb_thread_worker *worker, qb_thread_proc proc, void *param1, void *param2, int param3) {
	qb_thread_pool *pool = worker->pool;
	qb_thread_task req;
	req.proc = proc;
	req.param1 = param1;
	req.param2 = param2;
	req.param3 = param3;

#ifndef WIN32
	// acquire mutex--only one worker may submit a request at a time
	pthread_mutex_lock(&pool->main_thread_request_mutex);

	// wake up the main thread to handle the request
	pthread_mutex_lock(&pool->main_thread_resumption_mutex);
	pool->worker_request = &req;
	pool->waiting_worker = worker;
	pthread_cond_signal(&pool->main_thread_resumption_condition);
	pthread_mutex_unlock(&pool->main_thread_resumption_mutex);

	// wait for request to be handled
	pthread_cond_wait(&worker->resumption_condition, &worker->resumption_mutex);

	// release mutex
	pthread_mutex_unlock(&pool->main_thread_request_mutex);
#else
	// acquire mutex--only one worker may submit a request at a time
	WaitForSingleObject(pool->main_thread_request_mutex, INFINITE);
	pool->worker_request = &req;
	pool->waiting_worker = worker;
	SetEvent(pool->main_thread_resumption_event);

	// wait for request to be handled
	WaitForSingleObject(worker->resumption_event, INFINITE);

	// release mutex
	ReleaseMutex(pool->main_thread_request_mutex);
#endif
}

#ifndef WIN32
void *qb_run_task(void *arg) {
	qb_thread_worker *worker = arg;
	qb_thread_pool *pool = worker->pool;
	long completion_count;
	pthread_mutex_lock(&worker->resumption_mutex);
	for(;;) {
		uint32_t task_index = __sync_fetch_and_add(&pool->task_index, 1);
		if(task_index < pool->task_count) {
			qb_thread_task *task = &pool->tasks[task_index];
			if(task->worker_pointer) {
				*task->worker_pointer = worker;
			}
			task->proc(task->param1, task->param2, task->param3);

			completion_count = __sync_add_and_fetch(&pool->task_completion_count, 1);
			if(completion_count == pool->task_count) {
				// every thing is done--wake up the main thread
				pthread_mutex_lock(&pool->main_thread_resumption_mutex);
				pool->task_count = 0;
				pool->task_completion_count = 0;
				pthread_cond_signal(&pool->main_thread_resumption_condition);
				pthread_mutex_unlock(&pool->main_thread_resumption_mutex);
			}
		} else {
			// nothing to do--start waiting again
			pthread_cond_wait(&worker->resumption_condition, &worker->resumption_mutex);
			if(pool->task_count == 0) {
				// time to leave
				break;
			}
		}
	}
	pthread_mutex_unlock(&worker->resumption_mutex);

	completion_count = __sync_add_and_fetch(&pool->task_completion_count, 1);
	if(completion_count == pool->worker_count) {
		// last thread to exit--wake up the main thread
		pthread_mutex_lock(&pool->main_thread_resumption_mutex);
		pthread_cond_signal(&pool->main_thread_resumption_condition);
		pthread_mutex_unlock(&pool->main_thread_resumption_mutex);
	}
	return NULL;
}
#else
DWORD WINAPI qb_run_task(LPVOID lpThreadParameter) {
	qb_thread_worker *worker = lpThreadParameter;
	qb_thread_pool *pool = worker->pool;
	WaitForSingleObject(worker->resumption_event, INFINITE);

	if(pool->task_count > 0) {
		for(;;) {
			long task_index = InterlockedIncrement(&pool->task_index) - 1;
			if(task_index < pool->task_count) {
				qb_thread_task *task = &pool->tasks[task_index];
				if(task->worker_pointer) {
					*task->worker_pointer = worker;
				}
				task->proc(task->param1, task->param2, task->param3);

				if(InterlockedIncrement(&pool->task_completion_count) == pool->task_count) {
					// every thing is done--wake up the main thread
					pool->task_count = 0;
					pool->task_completion_count = 0;
					SetEvent(pool->main_thread_resumption_event);
				}
			} else {
				// nothing to do--start waiting again
				WaitForSingleObject(worker->resumption_event, INFINITE);
				if(pool->task_count == 0) {
					// time to leave
					break;
				}
			}
		}
	}

	if(InterlockedIncrement(&pool->task_completion_count) == pool->worker_count) {
		// last thread to exit--wake up the main thread
		SetEvent(pool->main_thread_resumption_event);
	}
	return 0;
}
#endif

void qb_run_tasks(qb_thread_pool *pool) {
	long i;
	if(pool->worker_count > 0) {
		pool->task_index = 0;

		// wake up the workers
		for(i = 0; i < pool->worker_count; i++) {
			if(i < pool->task_count) {
				qb_thread_worker *worker = &pool->workers[i];
#ifndef WIN32
				pthread_mutex_lock(&worker->resumption_mutex);
				pthread_cond_signal(&worker->resumption_condition);
				pthread_mutex_unlock(&worker->resumption_mutex);
#else
				SetEvent(worker->resumption_event);
#endif
			}
		}

#ifndef WIN32
		pthread_mutex_lock(&pool->main_thread_resumption_mutex);
#endif

		while(pool->task_count) {
#ifndef WIN32
			pthread_cond_wait(&pool->main_thread_resumption_condition, &pool->main_thread_resumption_mutex);
#else
			WaitForSingleObject(pool->main_thread_resumption_event, INFINITE);
#endif
			if(pool->worker_request) {
				// handle request from a worker thread
				qb_thread_task *task = pool->worker_request;
				qb_thread_worker *worker = pool->waiting_worker;
				task->proc(task->param1, task->param2, task->param3);

				pool->worker_request = NULL;
				pool->waiting_worker = NULL;

#ifndef WIN32
				pthread_mutex_lock(&worker->resumption_mutex);
				pthread_cond_signal(&worker->resumption_condition);
				pthread_mutex_unlock(&worker->resumption_mutex);
#else
				SetEvent(worker->resumption_event);
#endif
			}
		}

#ifndef WIN32
		pthread_mutex_unlock(&pool->main_thread_resumption_mutex);
#endif
	} else {
		// just run everything in the main thread
		for(i = 0; i < pool->task_count; i++) {
			qb_thread_task *task = &pool->tasks[i];
			task->proc(task->param1, task->param2, task->param3);
		}
		pool->task_count = 0;
	}
}

void qb_initialize_thread_pool(qb_thread_pool *pool TSRMLS_DC) {
	long i, thread_count;
	if(QB_G(thread_count) > 0) {
		thread_count = QB_G(thread_count);
	} else {
		thread_count = qb_get_cpu_count();
	}
	pool->task_buffer_size = 16;
	pool->task_count = 0;
	pool->task_index = 0;
	pool->task_completion_count = 0;
	pool->tasks = malloc(sizeof(qb_thread_task) * pool->task_buffer_size);
	if(thread_count > 1) {
		pool->worker_count = thread_count;
		pool->workers = malloc(sizeof(qb_thread_worker) * pool->worker_count);
		pool->worker_request = NULL;
		pool->waiting_worker = NULL;

#ifndef WIN32
		pthread_cond_init(&pool->main_thread_resumption_condition, NULL);
		pthread_mutex_init(&pool->main_thread_resumption_mutex, NULL);
		pthread_mutex_init(&pool->main_thread_request_mutex, NULL);
#else
		pool->main_thread_resumption_event = CreateEvent(NULL, FALSE, FALSE, NULL);
		pool->main_thread_request_mutex = CreateMutex(NULL, FALSE, NULL);
#endif
		for(i = 0; i < pool->worker_count; i++) {
			qb_thread_worker *worker = &pool->workers[i];
			worker->pool = pool;

#ifndef WIN32
			pthread_cond_init(&worker->resumption_condition, NULL);
			pthread_mutex_init(&worker->resumption_mutex, NULL);
			if(pthread_create(&worker->thread, NULL, qb_run_task, worker) != 0) {
				pthread_cond_destroy(&worker->resumption_condition);
				pthread_mutex_destroy(&worker->resumption_mutex);
				pool->worker_count = i;
				break;
			}
#else
			worker->resumption_event = CreateEvent(NULL, FALSE, FALSE, NULL);
			worker->thread = CreateThread(NULL, 0, qb_run_task, worker, 0, NULL);
			if(worker->thread == NULL) {
				CloseHandle(worker->resumption_event);
				pool->worker_count = i;
				break;
			}
#endif
		}
	} else {
		pool->worker_count = 0;
		pool->workers = NULL;
		pool->worker_request = NULL;
		pool->waiting_worker = NULL;
	}
}

void qb_free_thread_pool(qb_thread_pool *pool) {
	if(pool->worker_count > 0) {
		long i;
		pool->task_index = 0;
		pool->task_count = 0;
		pool->task_completion_count = 0;

		// wake up the workers so they will exit
		for(i = 0; i < pool->worker_count; i++) {
			qb_thread_worker *worker = &pool->workers[i];
#ifndef WIN32
			pthread_mutex_lock(&worker->resumption_mutex);
			pthread_cond_signal(&worker->resumption_condition);
			pthread_mutex_unlock(&worker->resumption_mutex);
#else
			SetEvent(worker->resumption_event);
#endif
		}

		// wait for them to exit before continuing
#ifndef WIN32
		pthread_mutex_lock(&pool->main_thread_resumption_mutex);
		if(pool->task_completion_count < pool->worker_count) {
			pthread_cond_wait(&pool->main_thread_resumption_condition, &pool->main_thread_resumption_mutex);
		}
		pthread_mutex_unlock(&pool->main_thread_resumption_mutex);
#else
		if(pool->task_completion_count < pool->worker_count) {
			WaitForSingleObject(pool->main_thread_resumption_event, INFINITE);
		}
#endif

		for(i = 0; i < pool->worker_count; i++) {
			qb_thread_worker *worker = &pool->workers[i];
#ifndef WIN32
			pthread_cond_destroy(&worker->resumption_condition);
			pthread_mutex_destroy(&worker->resumption_mutex);
#else
			CloseHandle(worker->resumption_event);
			CloseHandle(worker->thread);
#endif
		}

#ifndef WIN32
		pthread_cond_destroy(&pool->main_thread_resumption_condition);
		pthread_mutex_destroy(&pool->main_thread_resumption_mutex);
#else
		CloseHandle(pool->main_thread_resumption_event);
		CloseHandle(pool->main_thread_request_mutex);
#endif
		free(pool->workers);
	}
	free(pool->tasks);
}
