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

#ifndef WIN32
#include <signal.h>
#endif

#ifndef WIN32
#define THREAD_PROC_RETURN_TYPE		void *
#define THREAD_PROC_RETURN_VALUE	NULL
#define THREAD_PROC_DECLARATION(n)	void *(* n)(void *)
#else
#define THREAD_PROC_RETURN_TYPE		DWORD WINAPI
#define THREAD_PROC_RETURN_VALUE	0
#endif

static qb_thread_pool _pool;
static qb_thread_pool *pool = NULL;

#if defined(_MSC_VER)
__declspec(thread) qb_thread *tls_current_thread = NULL;
#elif defined(__GNUC__) && defined(__ELF__)
__thread qb_thread *tls_current_thread = NULL;
#else
static pthread_key_t key;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;

static void qb_make_pthread_key() {
    pthread_key_create(&key, NULL);
}
#endif

static void qb_set_current_thread(qb_thread *thread) {
#if defined(_MSC_VER)
	tls_current_thread = thread;
#elif defined(__GNUC__) && defined(__ELF__)
	tls_current_thread = thread;
#else
	pthread_once(&key_once, qb_make_pthread_key);
	pthread_setspecific(key, thread);
#endif
}

qb_thread *qb_get_current_thread(void) {
#if defined(_MSC_VER)
	return tls_current_thread;
#elif defined(__GNUC__) && defined(__ELF__)
	return tls_current_thread;
#else
	return pthread_getspecific(key);
#endif
}

int qb_in_main_thread(void) {
	qb_thread *current_thread = qb_get_current_thread();
	return (current_thread->type == QB_THREAD_MAIN);
}

/*
static long qb_atomic_increment(long *p_number) {
	long new_value;
#ifndef WIN32
	new_value = __sync_add_and_fetch(p_number, 1);
#else
	new_value = InterlockedIncrement(p_number);
#endif
	return new_value;
}

static long qb_atomic_decrement(long *p_number) {
	long new_value;
#ifndef WIN32
	new_value = __sync_sub_and_fetch(p_number, 1);
#else
	new_value = InterlockedDecrement(p_number);
#endif
	return new_value;
}
*/

static void qb_yield_time_slice(void) {
#ifndef WIN32
	sched_yield();
#else
	Sleep(1);
#endif
}

static int qb_initialize_mutex(qb_mutex *mutex) {
#ifndef WIN32
	if(pthread_mutex_init(&mutex->mutex, NULL) != 0) {
		return FALSE;
	}
#else
	mutex->mutex = CreateMutex(NULL, FALSE, NULL);
	if(!mutex->mutex) {
		return FALSE;
	}
#endif
	return TRUE;
}

static int qb_lock_mutex(qb_mutex *mutex, int non_blocking) {
#ifndef WIN32
	int result;
	if(non_blocking) {
		result = pthread_mutex_trylock(&mutex->mutex);
	} else {
		result = pthread_mutex_lock(&mutex->mutex);
	}
	return (result == 0);
#else
	DWORD result;
	if(non_blocking) {
		result = WaitForSingleObject(mutex->mutex, 0);
	} else {
		result = WaitForSingleObject(mutex->mutex, INFINITE);
	}
	return (result == WAIT_OBJECT_0);
#endif
}

static void qb_unlock_mutex(qb_mutex *mutex) {
#ifndef WIN32
	pthread_mutex_unlock(&mutex->mutex);
#else
	ReleaseMutex(mutex->mutex);
#endif
}

static void qb_free_mutex(qb_mutex *mutex) {
#ifndef WIN32
	pthread_mutex_destroy(&mutex->mutex);
#else
	CloseHandle(mutex->mutex);
#endif
}

static int qb_initialize_condition(qb_condition *condition) {
#ifndef WIN32
	if(pthread_cond_init(&condition->condition, NULL) != 0) {
		return FALSE;
	}
#else
	condition->event = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!condition->event) {
		return FALSE;
	}
#endif
	return TRUE;
}

static int qb_wait_for_condition(qb_condition *condition, qb_mutex *mutex) {
#ifndef WIN32
	int result = pthread_cond_wait(&condition->condition, &mutex->mutex);
	return (result == 0);
#else
	DWORD result;
	HANDLE objects[2] = { condition->event, mutex->mutex };
	ReleaseMutex(mutex->mutex);
	result = WaitForMultipleObjects(2, objects, TRUE, INFINITE);
	return (result == WAIT_OBJECT_0);
#endif
}

static int qb_signal_condition(qb_condition *condition) {
#ifndef WIN32
	int result = pthread_cond_signal(&condition->condition);
	return (result == 0);
#else
	BOOL result = SetEvent(condition->event);
	return result;
#endif
}

static void qb_free_condition(qb_condition *condition) {
#ifndef WIN32
	pthread_cond_destroy(&condition->condition);
#else
	CloseHandle(condition->event);
#endif
}

static int qb_initialize_event_sink(qb_event_sink *sink) {
	sink->message.type = QB_EVENT_NOT_LISTENING;
	sink->message.sender = NULL;
	if(!qb_initialize_condition(&sink->condition)) {
		return FALSE;
	}
	if(!qb_initialize_mutex(&sink->mutex)) {
		qb_free_condition(&sink->condition);
		return FALSE;
	}
	return TRUE;
}

static void qb_free_event_sink(qb_event_sink *sink) {
	qb_free_condition(&sink->condition);
	qb_free_mutex(&sink->mutex);
}

static int qb_lock_event_sink(qb_event_sink *sink) {
	if(!qb_lock_mutex(&sink->mutex, FALSE)) {
		return FALSE;
	}
	sink->message.type = QB_EVENT_LISTENING;
	return TRUE;
}

static void qb_unlock_event_sink(qb_event_sink *sink) {
	sink->message.type = QB_EVENT_NOT_LISTENING;
	qb_unlock_mutex(&sink->mutex);
}

static int qb_check_event_sink(qb_event_sink *sink) {
	return (sink->message.type != QB_EVENT_NOT_LISTENING);
}

static qb_event qb_wait_for_event(qb_event_sink *sink) {
	qb_event message;
	if(qb_wait_for_condition(&sink->condition, &sink->mutex)) {
		message = sink->message;
		sink->message.type = QB_EVENT_LISTENING;
		sink->message.sender = NULL;
	} else {
		message.type = QB_EVENT_NOT_LISTENING;
		message.sender = NULL;
	}
	return message;
}

static int qb_send_event(qb_event_sink *sink, qb_thread *sender, qb_event_type type, int non_blocking) {
	int delivered = FALSE;
	for(;;) {
		if(qb_lock_mutex(&sink->mutex, non_blocking)) {
			if(sink->message.type == QB_EVENT_LISTENING) {
				sink->message.type = type;
				sink->message.sender = sender;
				qb_signal_condition(&sink->condition);
				delivered = TRUE;
			} else if(sink->message.type != QB_EVENT_NOT_LISTENING) {
				// this is not terribly efficient, since there's a good chance
				// that the same thread will get the mutex again
				// assume that contention is infrequent
				if(!non_blocking) {
					qb_unlock_mutex(&sink->mutex);
					qb_yield_time_slice();
					continue;
				}
			}
			qb_unlock_mutex(&sink->mutex);
		}
		break;
	}
	return delivered;
}

#ifndef WIN32

// see zend_set_timeout() in zend_execute_API.c
#ifdef __CYGWIN__
	#define TIME_OUT_SIGNAL 	SIGALRM
#else
	#define TIME_OUT_SIGNAL		SIGPROF
#endif

static void qb_disable_signals(qb_main_thread *thread) {
	sigset_t set;
	sigfillset(&set);
	pthread_sigmask(SIG_SETMASK, &set, &thread->signal_mask);
}

static void qb_enable_signals(qb_main_thread *thread) {
	pthread_sigmask(SIG_SETMASK, &thread->signal_mask, NULL);
}

static void qb_disable_time_out_signal(qb_main_thread *thread) {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, TIME_OUT_SIGNAL);
	pthread_sigmask(SIG_BLOCK, &set, &thread->signal_mask);
}

static void qb_enable_time_out_signal(qb_main_thread *thread) {
	pthread_sigmask(SIG_SETMASK, &thread->signal_mask, NULL);
}

// intercept timeout signal so we won't longjmp() out of a pthread call
THREAD_PROC_RETURN_TYPE qb_signal_thread_proc(void *arg) {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);
	sigaddset(&set, TIME_OUT_SIGNAL);

	// keep waiting for signals (unless signal_thread_state has been incremented already)
	if(__sync_add_and_fetch(&pool->signal_thread_state, 1) == 1) {
		for(;;) {
			int signal;
			sigwait(&set, &signal);
			if(signal == TIME_OUT_SIGNAL) {
				// note: it's unclear how PHP handles timeout when ZTS is on
				qb_main_thread *main_thread = pool->current_main_thread;
				if(main_thread) {
					pool->current_main_thread = NULL;
					qb_send_event(&main_thread->event_sink, NULL, QB_EVENT_TIMEOUT, FALSE);
				} else {
					zend_timeout(0);
				}
			} else if(signal == SIGQUIT) {
				break;
			}
		}
	}
	return THREAD_PROC_RETURN_VALUE;
}
#endif

int qb_initialize_main_thread(qb_main_thread *thread TSRMLS_DC) {
	thread->type = QB_THREAD_MAIN;
	thread->worker_count = 0;
	if(!qb_initialize_event_sink(&thread->event_sink)) {
		return FALSE;
	}
#ifndef WIN32
	thread->thread = pthread_self();
#else
	thread->thread = GetCurrentThread();
#endif

#ifdef ZTS
	thread->tsrm_ls = tsrm_ls;
#endif


	qb_set_current_thread((qb_thread*) thread);

	qb_lock_event_sink(&thread->event_sink);
	return TRUE;
}

THREAD_PROC_RETURN_TYPE qb_worker_thread_proc(void *arg);

static int qb_initialize_worker_thread(qb_worker_thread *thread, qb_thread *creator) {
	thread->type = QB_THREAD_WORKER;
	thread->allow_termination = FALSE;
	thread->terminated = FALSE;
	thread->current_owner = NULL;
	thread->current_task = NULL;
	thread->creator = creator;
	if(!qb_initialize_event_sink(&thread->event_sink)) {
		return FALSE;
	}
#ifndef WIN32
	if(pthread_create(&thread->thread, NULL, qb_worker_thread_proc, thread) != 0) {
		thread->thread = 0;
		return FALSE;
	}
#else
	thread->thread = CreateThread(NULL, 0, qb_worker_thread_proc, thread, 0, NULL);
	if(!thread->thread) {
		return FALSE;
	}
#endif
	return TRUE;
}

static qb_main_thread * qb_get_thread_owner(qb_thread *thread) {
	if(thread->type == QB_THREAD_MAIN) {
		return (qb_main_thread *) thread;
	} else if(thread->type == QB_THREAD_WORKER) {
		qb_worker_thread *worker = (qb_worker_thread *) thread;
		return worker->current_owner;
	}
	return NULL;
}

#ifdef ZTS
void ***qb_get_tsrm_ls() {
	qb_thread *current_thread = qb_get_current_thread();
	if(current_thread) {
		qb_main_thread *main_thread = qb_get_thread_owner(current_thread);
		return main_thread->tsrm_ls;
	} else {
		TSRMLS_FETCH();
		return tsrm_ls;
	}
}
#endif

static void qb_wait_for_worker_termination(qb_worker_thread *thread) {
	if(thread->thread) {
		if(thread->terminated) {
			// the thread's termination is deferred because it was running where
			// it could be holding synchronization objects
			// keep trying to tell it to quit until it does
			for(;;) {
				if(qb_check_event_sink(&thread->event_sink)) {
					if(qb_send_event(&thread->event_sink, NULL, QB_EVENT_TERMINATION, TRUE)) {
						break;
					}
				} else {
					break;
				}
				qb_yield_time_slice();
			}
		} else {
			thread->terminated = TRUE;
		}
#ifndef WIN32
		pthread_join(thread->thread, NULL);
#else
		WaitForSingleObject(thread->thread, INFINITE);
#endif
	}
}

static void qb_exit_worker_thread(qb_worker_thread *thread) {
	// unlock the mutex so it doesn't get destroyed while locked
	qb_unlock_event_sink(&thread->event_sink);
	thread->terminated = TRUE;
#ifndef WIN32
	pthread_exit(0);
#else
	ExitThread(0);
#endif
}

static int qb_terminate_worker_thread(qb_worker_thread *thread) {
	if(thread->allow_termination) {
#ifndef WIN32
		int old_type, old_state;
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &old_type);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old_state);
		if(pthread_cancel(thread->thread) != 0) {
			return FALSE;
		}
#else
		if(!TerminateThread(thread->thread, 0)) {
			return FALSE;
		}
#endif
		thread->terminated = TRUE;
		qb_unlock_event_sink(&thread->event_sink);
	} else {
		thread->terminated = TRUE;
	}
	return TRUE;
}

static void qb_disable_termination(qb_worker_thread *thread) {
	thread->allow_termination = FALSE;
}

static void qb_enable_termination(qb_worker_thread *thread) {
	if(thread->terminated) {
		qb_exit_worker_thread(thread);
	} else {
		thread->allow_termination = TRUE;
	}
}

void qb_free_main_thread(qb_main_thread *thread) {
	qb_unlock_event_sink(&thread->event_sink);
	qb_free_event_sink(&thread->event_sink);
}

static void qb_free_worker_thread(qb_worker_thread *thread) {
	if(thread->thread) {
		qb_free_event_sink(&thread->event_sink);
	}
}

qb_task_group * qb_allocate_task_group(long task_count, long extra_bytes) {
	// emalloc() isn't thread-safe, so we're using malloc(), which hopefully is
	long memory_required = sizeof(qb_task_group) + sizeof(qb_task) * task_count + extra_bytes;
	char *memory = malloc(memory_required);
	qb_task_group *group = (qb_task_group *) memory;
	group->tasks = (qb_task *) (memory + sizeof(qb_task_group));
	group->completion_count = 0;
	group->task_count = 0;
	group->task_index = 0;
	group->owner = qb_get_current_thread();
	group->extra_memory = (extra_bytes) ? &group->tasks[task_count] : NULL;
	group->previous_group = NULL;
	group->next_group = NULL;
	return group;
}

void qb_free_task_group(qb_task_group *group) {
	free(group);
}

void qb_add_task(qb_task_group *group, qb_thread_proc proc, void *param1, void *param2, int param3) {
	qb_task *task = &group->tasks[group->task_count++];
	task->group = group;
	task->param1 = param1;
	task->param2 = param2;
	task->param3 = param3;
	task->proc = proc;
}

static void qb_add_task_group(qb_task_group *group) {
	qb_lock_mutex(&pool->task_queue_mutex, FALSE);
	if(group->task_index != group->task_count) {
		if(pool->task_queue_tail) {
			group->previous_group = pool->task_queue_tail;
			pool->task_queue_tail->next_group = group;
		} else {
			pool->task_queue_head = pool->task_queue_tail = group;
		}
	}
	qb_unlock_mutex(&pool->task_queue_mutex);
}

static void qb_remove_task_group_no_lock(qb_task_group *group) {
	if(group->previous_group) {
		group->previous_group->next_group = group->next_group;
		group->previous_group = NULL;
	} else if(pool->task_queue_head == group) {
		pool->task_queue_head = group->next_group;
	}
	if(group->next_group) {
		group->next_group->previous_group = group->previous_group;
		group->next_group = NULL;
	} else if(pool->task_queue_tail == group) {
		pool->task_queue_tail = group->previous_group;
	}
}

static void qb_remove_task_group(qb_task_group *group) {
	qb_lock_mutex(&pool->task_queue_mutex, FALSE);
	qb_remove_task_group_no_lock(group);
	qb_unlock_mutex(&pool->task_queue_mutex);
}

static int qb_assign_task(qb_worker_thread *worker) {
	int task_assigned = FALSE;
	qb_thread *thread_to_notify = NULL;

	qb_lock_mutex(&pool->task_queue_mutex, FALSE);
	if(worker->current_task) {
		qb_main_thread *main_thread = worker->current_owner;
		qb_task_group *group = worker->current_task->group;
		group->completion_count++;
		main_thread->worker_count--;

		if(group->completion_count == group->task_count) {
			thread_to_notify = group->owner;
		}
	}
	if(pool->task_queue_head) {
		qb_task_group *group;
		for(group = pool->task_queue_head; group; group = group->next_group) {
			qb_main_thread *main_thread = qb_get_thread_owner(group->owner);
			if(main_thread->worker_count < pool->per_request_thread_limit) {
				if(group->task_index < group->task_count) {
					worker->current_task = &group->tasks[group->task_index++];
					worker->current_owner = main_thread;
					main_thread->worker_count++;
					task_assigned = TRUE;

					if(group->task_index == group->task_count) {
						// no more task in this group--remove it
						qb_remove_task_group_no_lock(group);
					}
				} else {
					// no sure how this can happen
					qb_remove_task_group_no_lock(group);
				}
				break;
			}
		}
	}
	if(!task_assigned) {
		worker->current_owner = NULL;
		worker->current_task = NULL;
	}
	qb_unlock_mutex(&pool->task_queue_mutex);

	if(thread_to_notify) {
		qb_send_event(&thread_to_notify->event_sink, (qb_thread *) worker, QB_EVENT_TASK_GROUP_PROCESSED, FALSE);
	}
	return task_assigned;
}

static void qb_perform_task(qb_task *task, qb_thread *thread) {
	task->proc(task->param1, task->param2, task->param3);
}

static void qb_handle_worker_events(qb_worker_thread *worker, qb_event_type expecting) {
	long loop = 0;
	for(;;) {
		if(!worker->terminated) {
			qb_event event = qb_wait_for_event(&worker->event_sink);
			switch(event.type) {
				case QB_EVENT_TASK_GROUP_ADDED: {
					// get a task and run it
					while(qb_assign_task(worker)) {
						qb_enable_termination(worker);
						qb_perform_task(worker->current_task, (qb_thread *) worker);
						qb_disable_termination(worker);
					}
				}	break;
				case QB_EVENT_TERMINATION: {
					// exit the thread immediately
					qb_exit_worker_thread(worker);
				}
				case QB_EVENT_REQUEST_PROCESSED:
				case QB_EVENT_TASK_GROUP_PROCESSED: {
					if(expecting == event.type) {
						// time to leave the loop
						return;
					}
				}	break;
				default: {
				}	break;
			}
		} else{
			qb_exit_worker_thread(worker);
		}
		loop++;
	}
}

static void qb_handle_main_thread_events(qb_main_thread *thread, qb_event_type expecting) {
	for(;;) {
		qb_event event = qb_wait_for_event(&thread->event_sink);
		switch(event.type) {
			case QB_EVENT_REQUEST_SENT: {
				// handle request from a worker thread
				qb_worker_thread *worker = (qb_worker_thread *) event.sender;
				qb_task *task = worker->request;
				qb_perform_task(task, (qb_thread *) thread);

				// wake up the worker again
				qb_send_event(&worker->event_sink, (qb_thread *) thread, QB_EVENT_REQUEST_PROCESSED, FALSE);
			}	break;
			case QB_EVENT_TIMEOUT: {
				zend_timeout(0);
			}
			case QB_EVENT_WORKER_ADDED:
			case QB_EVENT_TASK_GROUP_PROCESSED:
				if(expecting == event.type) {
					// time to leave the loop
					return;
				}
			default: {
			}	break;
		}
	}
}

THREAD_PROC_RETURN_TYPE qb_worker_thread_proc(void *arg) {
	qb_worker_thread *worker = arg;
	qb_lock_event_sink(&worker->event_sink);

	qb_set_current_thread((qb_thread *) worker);

	// event to the creator that the thread is ready
	qb_send_event(&worker->creator->event_sink, (qb_thread *) worker, QB_EVENT_WORKER_ADDED, FALSE);

	qb_handle_worker_events(worker, QB_EVENT_TASK_GROUP_ADDED);

	qb_unlock_event_sink(&worker->event_sink);
	return THREAD_PROC_RETURN_VALUE;
}

void qb_add_workers(qb_main_thread *thread) {
	long i;
#ifndef WIN32
	// disable all signals so threads will inherit none
	qb_disable_signals(thread);
#endif
	for(i = 0; i < pool->global_thread_limit; i++) {
		qb_worker_thread *worker = &pool->workers[pool->worker_count];
		if(qb_initialize_worker_thread(worker, (qb_thread *) thread)) {
			// wait for thread to enter main loop
			qb_handle_main_thread_events(thread, QB_EVENT_WORKER_ADDED);
			pool->worker_count++;
		} else {
			break;
		}
	}
#ifndef WIN32
	qb_enable_signals(thread);
#endif
}

void qb_restart_workers(qb_main_thread *main_thread) {
	long i;
#ifndef WIN32
	qb_disable_signals(main_thread);
#endif
	for(i = 0; i < pool->worker_count; i++) {
		qb_worker_thread *worker = &pool->workers[i];
		if(worker->terminated) {
			if(qb_initialize_worker_thread(worker, (qb_thread *) main_thread)) {
				// wait for thread to enter main loop
				qb_handle_main_thread_events(main_thread, QB_EVENT_WORKER_ADDED);
			}
		}
	}
#ifndef WIN32
	qb_enable_signals(main_thread);
#endif
}

int qb_wake_workers(qb_thread *thread, long count) {
	long i, awaken = 0;
	for(i = 0; i < pool->worker_count && count > 0; i++) {
		qb_worker_thread *worker = &pool->workers[i];
		if(!worker->current_owner && !worker->terminated) {
			// worker appears to be idle
			if(qb_check_event_sink(&worker->event_sink)) {
				if(qb_send_event(&worker->event_sink, thread, QB_EVENT_TASK_GROUP_ADDED, TRUE)) {
					count--;
					awaken++;
				}
			}
		}
	}
	return (awaken > 0);
}

void qb_run_task_group(qb_task_group *group, int iterative) {
	qb_thread *thread = group->owner;
	qb_main_thread *main_thread = qb_get_thread_owner(thread);
	int workers_available = FALSE;

	if(main_thread->worker_count + 1 < pool->per_request_thread_limit || iterative) {
		long count = pool->per_request_thread_limit - main_thread->worker_count;
		if(count > group->task_count) {
			count = group->task_count;
		}

#ifndef WIN32
		if(thread->type == QB_THREAD_MAIN) {
			pool->current_main_thread = main_thread;
			qb_disable_time_out_signal(main_thread);
		}
#endif

		qb_add_task_group(group);

		if(qb_wake_workers(thread, count)) {
			workers_available = TRUE;
		} else {
			// take the group off the queue
			qb_remove_task_group(group);
			if(group->task_index != 0) {
				// okay, a worker has just become available and took one (or more) of the tasks
				// put it back into the queue
				qb_add_task_group(group);
				workers_available = TRUE;
			}
		}
		if(workers_available) {
			do {
				if(thread->type == QB_THREAD_MAIN) {
					qb_handle_main_thread_events((qb_main_thread *) thread, QB_EVENT_TASK_GROUP_PROCESSED);
				} else {
					qb_handle_worker_events((qb_worker_thread*) thread, QB_EVENT_TASK_GROUP_PROCESSED);
				}
			} while(group->completion_count < group->task_count);
		}

#ifndef WIN32
		if(thread->type == QB_THREAD_MAIN) {
			qb_enable_time_out_signal(main_thread);
			pool->current_main_thread = NULL;
		}
#endif
	}
	if(!workers_available) {
		// just run everything in the current thread
		while(group->task_index < group->task_count) {
			qb_task *task = &group->tasks[group->task_index++];
			qb_perform_task(task, thread);
			group->completion_count++;
		}
	}
}

void qb_run_in_main_thread(qb_thread_proc proc, void *param1, void *param2, int param3) {
	qb_thread *current_thread = qb_get_current_thread();
	if(current_thread && current_thread->type != QB_THREAD_MAIN) {
		qb_main_thread *main_thread = qb_get_thread_owner(current_thread);
		qb_worker_thread *worker = (qb_worker_thread *) current_thread;
		qb_task task;
		task.proc = proc;
		task.param1 = param1;
		task.param2 = param2;
		task.param3 = param3;
		task.group = NULL;
		worker->request = &task;

		// disable termination so threads aren't kill in the middle of pthread functions
		qb_disable_termination(worker);

		// wake up the main thread to handle the request
		qb_send_event(&main_thread->event_sink, (qb_thread *) worker, QB_EVENT_REQUEST_SENT, FALSE);

		// wait for request to be handled
		qb_handle_worker_events(worker, QB_EVENT_REQUEST_PROCESSED);

		qb_enable_termination(worker);
	} else {
		// just run it
		proc(param1, param2, param3);
	}
}

void qb_terminate_associated_workers(qb_main_thread *main_thread) {
	qb_task_group *group, *queue_head = NULL, *queue_tail = NULL, *next_group;
	long i;
	long workers_terminated = 0;

	// remove outstanding task groups belonging to the main thread
	qb_lock_mutex(&pool->task_queue_mutex, FALSE);
	for(group = pool->task_queue_head; group; group = group->next_group) {
		qb_main_thread *owner_main_thread = qb_get_thread_owner(group->owner);
		if(owner_main_thread == main_thread) {
			qb_remove_task_group_no_lock(group);

			// stick the removed groups into a list so we can free them later
			// after the threads have been terminated
			if(queue_tail) {
				queue_tail->next_group = group;
				group->previous_group = queue_tail;
				queue_tail = group;
			} else {
				queue_head = queue_tail = group;
			}
		}
	}
	qb_unlock_mutex(&pool->task_queue_mutex);

	// terminate and restart threads currently associated with this main thread
	if(main_thread->worker_count > 0) {
		// unlock the event sink so workers aren't blocked because they're trying to
		// communicate with the main thread
		qb_unlock_event_sink(&main_thread->event_sink);

		for(i = 0; i < pool->worker_count; i++) {
			qb_worker_thread *worker = &pool->workers[i];
			if(worker->current_owner == main_thread) {
				// ask the thread to terminate itself first, then try to kill it
				int terminated;
				if(qb_send_event(&worker->event_sink, (qb_thread *) main_thread, QB_EVENT_TERMINATION, TRUE)) {
					terminated = TRUE;
				} else if(qb_terminate_worker_thread(worker)) {
					terminated = TRUE;
				}
				if(terminated) {
					// wait for it to exit
					qb_wait_for_worker_termination(worker);
					workers_terminated++;

					// free the resources
					qb_free_worker_thread(worker);
				}
			}
		}
		main_thread->worker_count = 0;
		qb_lock_event_sink(&main_thread->event_sink);
	}

#ifndef WIN32
	qb_enable_time_out_signal(main_thread);
#endif

	if(workers_terminated > 0) {
		// restart threads that were terminated
		qb_restart_workers(main_thread);
		fflush(stdout);
	}

	// free the group
	for(group = queue_head; group; group = next_group) {
		next_group = group->next_group;
		qb_free_task_group(group);
	}
}

int qb_initialize_thread_pool(TSRMLS_D) {
	if(!pool) {
		pool = &_pool;
		pool->per_request_thread_limit = QB_G(thread_count);
		if(pool->per_request_thread_limit > 1) {
			pool->global_thread_limit = pool->per_request_thread_limit * QB_GLOBAL_THREAD_COUNT_MULTIPLIER;
		} else {
			pool->global_thread_limit = 0;
		}

		pool->workers = malloc(sizeof(qb_worker_thread) * pool->global_thread_limit);
		pool->worker_count = 0;
		pool->task_queue_head = NULL;
		pool->task_queue_tail = NULL;

		qb_initialize_mutex(&pool->task_queue_mutex);

#ifndef WIN32
		pool->current_main_thread = NULL;
		pool->signal_thread = 0;
		pool->signal_thread_state = 0;
		if(pool->global_thread_limit > 0) {
			pthread_create(&pool->signal_thread, NULL, qb_signal_thread_proc, pool);
		}
#endif
		return TRUE;
	} else {
		return FALSE;
	}
}

void qb_free_thread_pool(void) {
	if(pool) {
		long i;
#ifndef WIN32
		if(pool->signal_thread) {
			if(__sync_add_and_fetch(&pool->signal_thread_state, 1) == 2) {
				sigset_t set, old_set;
				sigfillset(&set);
				sigprocmask(SIG_SETMASK, &set, &old_set);
				pthread_kill(pool->signal_thread, SIGQUIT);
				sigprocmask(SIG_SETMASK, &old_set, NULL);
			}
			pthread_join(pool->signal_thread, NULL);
		}
#endif

		for(i = 0; i < pool->worker_count; i++) {
			qb_worker_thread *worker = &pool->workers[i];
			int terminated = FALSE;
			if(qb_send_event(&worker->event_sink, NULL, QB_EVENT_TERMINATION, FALSE)) {
				terminated = TRUE;
			} else if(qb_terminate_worker_thread(worker)) {
				terminated = TRUE;
			}
			if(terminated) {
				qb_wait_for_worker_termination(worker);
				qb_free_worker_thread(worker);
			}
		}
		qb_free_mutex(&pool->task_queue_mutex);
	}
}

long qb_get_cpu_count(void) {
	static long count = 0;
	if(!count) {
#if defined(__unix__)
		count = sysconf( _SC_NPROCESSORS_ONLN );
#elif defined(WIN32)
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		count = sysinfo.dwNumberOfProcessors;
#endif
	}
	return count;
}
