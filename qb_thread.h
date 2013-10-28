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

typedef struct qb_thread_task		qb_thread_task;
typedef struct qb_thread_worker		qb_thread_worker;
typedef struct qb_thread_pool		qb_thread_pool;

typedef void (*qb_thread_proc)(void *param1, void *param2, int param3);

struct qb_thread_task {
	qb_thread_proc proc;
	void *param1;
	void *param2;
	int param3;
	qb_thread_worker **worker_pointer;
};

struct qb_thread_worker {
	qb_thread_pool *pool;

#ifndef WIN32
	pthread_t thread;
	pthread_cond_t resumption_condition;
	pthread_mutex_t resumption_mutex;
#else
	HANDLE thread;
	HANDLE resumption_event;
#endif
};

struct qb_thread_pool {
	qb_thread_task *tasks;
	volatile long task_count;
	volatile long task_index;
	volatile long task_completion_count;
	long task_buffer_size;

	qb_thread_worker *workers;
	long worker_count;

	qb_thread_task *worker_request;
	qb_thread_worker *waiting_worker;

#ifndef WIN32
	pthread_cond_t main_thread_resumption_condition;
	pthread_mutex_t main_thread_resumption_mutex;
	pthread_mutex_t main_thread_request_mutex;
#else
	HANDLE main_thread_resumption_event;
	HANDLE main_thread_request_mutex;
#endif
};

long qb_get_cpu_count(void);

void qb_schedule_task(qb_thread_pool *pool, qb_thread_proc proc, void *param1, void *param2, int param3, qb_thread_worker **p_worker);
void qb_run_tasks(qb_thread_pool *pool);
void qb_run_in_main_thread(qb_thread_worker *worker, qb_thread_proc proc, void *param1, void *param2, int param3);

void qb_initialize_thread_pool(qb_thread_pool *pool TSRMLS_DC);
void qb_free_thread_pool(qb_thread_pool *pool);


#endif