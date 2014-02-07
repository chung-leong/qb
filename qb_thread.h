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

#define QB_GLOBAL_THREAD_COUNT_MULTIPLIER		1

typedef struct qb_condition			qb_condition;
typedef struct qb_mutex				qb_mutex;
typedef struct qb_event				qb_event;
typedef struct qb_event_sink		qb_event_sink;
typedef struct qb_task				qb_task;
typedef struct qb_task_group		qb_task_group;

typedef struct qb_thread			qb_thread;
typedef struct qb_worker_thread		qb_worker_thread;
typedef struct qb_main_thread		qb_main_thread;
typedef struct qb_timeout_thread	qb_timeout_thread;

typedef struct qb_thread_pool		qb_thread_pool;

typedef void (*qb_thread_proc)(void *param1, void *param2, int param3);

typedef enum qb_thread_type			qb_thread_type;
typedef enum qb_event_type			qb_event_type;

enum qb_event_type {
	QB_EVENT_LISTENING,
	QB_EVENT_NOT_LISTENING,
	QB_EVENT_WORKER_ADDED,
	QB_EVENT_TASK_GROUP_ADDED,
	QB_EVENT_TASK_GROUP_PROCESSED,
	QB_EVENT_REQUEST_SENT,
	QB_EVENT_REQUEST_PROCESSED,
	QB_EVENT_TERMINATION,
	QB_EVENT_TIMEOUT,
};

enum qb_thread_type {
	QB_THREAD_UNINITIALIZED,
	QB_THREAD_MAIN,
	QB_THREAD_WORKER,
};

struct qb_event {
	qb_event_type type;
	qb_thread *sender;
};

struct qb_mutex {
#ifndef WIN32
	pthread_mutex_t mutex;
#else
	HANDLE mutex;
#endif
};

struct qb_condition {
#ifndef WIN32
	pthread_cond_t condition;
#else
	HANDLE event;
#endif
};

struct qb_event_sink {
	qb_event message;
	qb_condition condition;
	qb_mutex mutex;
};

struct qb_task {
	qb_thread_proc proc;
	void *param1;
	void *param2;
	int param3;
	qb_task_group *group;
};

struct qb_task_group {
	qb_task *tasks;
	long task_count;
	long task_index;
	long completion_count;
	qb_thread *owner;
	void *extra_memory;
	qb_task_group *previous_group;
	qb_task_group *next_group;
};

struct qb_thread {
	qb_thread_type type;
	qb_event_sink event_sink;
#ifndef WIN32
	pthread_t thread;
#else
	HANDLE thread;
#endif
};

struct qb_worker_thread {
	qb_thread_type type;
	qb_event_sink event_sink;
#ifndef WIN32
	pthread_t thread;
#else
	HANDLE thread;
#endif

	int allow_termination;
	volatile int terminated;
	qb_thread *creator;
	qb_main_thread *current_owner;
	qb_task *current_task;
	qb_task *request;
};

struct qb_main_thread {
	qb_thread_type type;
	qb_event_sink event_sink;
#ifndef WIN32
	pthread_t thread;
#else
	HANDLE thread;
#endif

	long worker_count;
#ifndef WIN32
	sigset_t signal_mask;
#endif

#ifdef ZTS
	void ***tsrm_ls;
#endif
};

struct qb_thread_pool {
	qb_worker_thread *workers;
	long worker_count;

	qb_task_group *task_queue_head;
	qb_task_group *task_queue_tail;
	qb_mutex task_queue_mutex;

	long global_thread_limit;
	long per_request_thread_limit;

#ifndef WIN32
	qb_main_thread *current_main_thread;
	pthread_t signal_thread;
#endif
};

long qb_get_cpu_count(void);

int qb_initialize_main_thread(qb_main_thread *thread TSRMLS_DC);
void qb_free_main_thread(qb_main_thread *thread);

#ifdef ZTS
void ***qb_get_tsrm_ls(void);
#endif

void qb_initialize_task_group(qb_task_group *group, long task_count, long extra_bytes);
void qb_free_task_group(qb_task_group *group);
void qb_add_task(qb_task_group *group, qb_thread_proc proc, void *param1, void *param2, int param3);
void qb_run_task_group(qb_task_group *group, int32_t iterative);
void qb_run_in_main_thread(qb_thread_proc proc, void *param1, void *param2, int param3);
void qb_terminate_associated_workers(qb_main_thread *main_thread);

qb_thread *qb_get_current_thread(void);
int32_t qb_in_main_thread(void);

void qb_initialize_thread_pool(TSRMLS_D);
void qb_add_workers(qb_main_thread *thread);
void qb_free_thread_pool(void);

#endif
