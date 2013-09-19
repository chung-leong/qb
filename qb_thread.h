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

#ifndef QB_THREAD_H_
#define QB_THREAD_H_

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

typedef void (*qb_thread_proc)(void *, void *);

typedef struct qb_thread_pool		qb_thread_pool;
typedef struct qb_thread_task		qb_thread_task;
typedef struct qb_task_assignment	qb_task_assignment;

struct qb_thread_task {
	qb_thread_proc proc;
	void *param1;
	void *param2;
	int32_t assigned;
};

struct qb_task_assignment {
	volatile qb_thread_task *task;
#ifdef WIN32
	HANDLE win32_completion_event;
	int32_t thread_suspended;
#else
	volatile int32_t thread_suspended;
	pthread_cond_t *pthread_wake_cond;
	pthread_mutex_t *pthread_wake_mutex;
	pthread_cond_t *pthread_completion_cond;
	pthread_mutex_t *pthread_completion_mutex;
	int32_t *pthread_completion_state;
#endif
};

struct qb_thread_pool {
	qb_thread_task *tasks;
	uint32_t task_count;
	uint32_t task_buffer_size;
	uint32_t task_remaining;
	qb_task_assignment *task_assignments;
	uint32_t thread_count;

#ifdef WIN32
	HANDLE *win32_threads;
	HANDLE win32_completion_event;
#else
	pthread_t *pthreads;
	pthread_cond_t *pthread_wake_conds;
	pthread_mutex_t *pthread_wake_mutexes;
	pthread_cond_t pthread_completion_cond;
	pthread_mutex_t pthread_completion_mutex;
	volatile int32_t pthread_completion_state;
#endif
};

int32_t ZEND_FASTCALL qb_schedule_task(qb_thread_pool *pool, void *proc, void *param1, void *param2);
int ZEND_FASTCALL qb_initialize_thread_pool(qb_thread_pool *pool);
int ZEND_FASTCALL qb_run_tasks(qb_thread_pool *pool);

#endif