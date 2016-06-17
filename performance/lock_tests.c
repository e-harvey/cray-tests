/*
 * Copyright (c) 2016 Cray Inc. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/******* Tunable defines for testing different lock implementations *******/
#define rlock_acquire  pthread_rwlock_rdlock
#define rwlock_acquire pthread_rwlock_wrlock
#define release        pthread_rwlock_unlock
#define lock_t	       pthread_rwlock_t
#define lock_header    pthread.h
/**************************************************************************/

#include <lock_header>
#include <fi_atom.h>
/* #include <print_header.h> */
#include "common/ct_utils.h"

/* Thread vars */
static pthread_barrier_t barrier;
static pthread_t *writers, *readers;
static lock_t lock;

/*******************************************************************************
 * Begin test functions
 ******************************************************************************/
void *reader(void *arg)
{
	unsigned nreads = *(unsigned *)arg, i;
	pthread_barrier_wait(&barrier);
	START_CLOCK;

	for (i = 0; i < nreads; i++) {
		rlock_acquire(&lock);
		/* read */
		release(&lock);
	}

	return NULL;
}

void *writer(void *arg)
{
	unsigned nwrites = *(unsigned *) arg, i;
	pthread_barrier_wait(&barrier);
	START_CLOCK;

	for (i = 0; i < nwrites; i++) {
		rwlock_acquire(&lock);
		/* write */
		release(&lock);
	}

	return NULL;
}

/* Fire up nreader/writer threads that do nreads_per_reader/writer before exiting */
void rwlock_test(unsigned nreaders, unsigned nreads_per_reader,
		 unsigned nwrites_per_writer, unsigned nwrites)
{
	int i, j;
	void *ret;

	if (nreaders) {
		readers = malloc(sizeof(pthread_t) * nreaders);
		assert(readers);
	}

	if (nwrites_per_writer) {
		writers = malloc(sizeof(pthread_t) * nwrites_per_writer);
		assert(writers);
	}

	/* Ensure readers and writers wait until all threads are up */
	pthread_barrier_init(&barrier, NULL, nreaders + nwrites_per_writer);
	atomic_set(&clock_started, 0);

	/* spawn read threads */
	for (i = 0; i < nreaders; i++) {
		assert(!pthread_create(readers + i, NULL, reader, nreads_per_reader));
	}

	/* spawn write threads */
	for (j = 0; j < nwrites_per_writer; j++) {
		assert(!pthread_create(writers + i, NULL, writer, nwrites));
	}

	/* join readers */
	for (i = 0; i < nreaders; i++) {
		assert(!pthread_join(readers[i], &ret));
		assert(!ret);
	}

	/* join writers */
	for (j = 0; j < nwrites_per_writer; j++) {
		assert(!pthread_join(writers[i], &ret));
		assert(!ret);
	}

	STOP_CLOCK;
}
/*******************************************************************************
 * End test functions
 ******************************************************************************/

/*******************************************************************************
 * Run tests
 ******************************************************************************/
int main(int argc, char **argv)
{
	test_name   = BUILD_HDR(lock_header.h, rwlock_test);
	csv_header  = "Date,runtime(s),cpu_time";

	/* TODO: init_info(test_name, csv_header) */

	/* n readers, n writers */
	test_params = "nreaders = 10, nreads_per_reader = 10, nwriters = 10,"
		" nwrites_per_writer = 10";
	rwlock_test(10, 10, 10, 10);
	/* TODO: print_utils should print the test_name & test_params followed by
	 * the formatted output.
	 * TODO: print_data(test_params, "%time, %ld, %ld", WALL_CLOCK_TIME, CPU_TIME);
	 */
	printf("%s\n%s\nruntime(s) = %ld,cpu_time = %ld", test_name,
	       test_params, WALL_CLOCK_TIME, CPU_TIME);

	/* x readers, 1 writers */
	test_params = "nreaders = 10, nreads_per_reader = 10, nwriters = 1,"
		" nwrites_per_writer = 10";

	rwlock_test(10, 10, 1, 10);

	printf("%s\n%s\nruntime(s) = %ld,cpu_time = %ld", test_name,
	       test_params, WALL_CLOCK_TIME, CPU_TIME);

	/* x readers, no writers */
	test_params = "nreaders = 10, nreads_per_reader = 10, nwriters = 0,"
		" nwrites_per_writer = 10";

	rwlock_test(10, 10, 0, 10);

	printf("%s\n%s\nruntime(s) = %ld,cpu_time = %ld", test_name,
	       test_params, WALL_CLOCK_TIME, CPU_TIME);

	/* TODO: fini_info */
}
