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
#include <time.h>

static clock_t start, end;
static pthread_mutex_t clock_lock;
static char clock_started, is_clock_lock_init;


#define INIT_CLOCK_LOCK if (!is_clock_lock_init) {		       \
				pthread_mutex_init(&clock_lock, NULL); \
				is_clock_lock_init = 1;		       \
			}

#define DEST_CLOCK_LOCK if (is_clock_lock_init) {			\
				pthread_mutex_destroy(&clock_lock);	\
				is_clock_lock_init = 0;			\
			}

/* Useful MACROS */
#define START_CLOCK INIT_CLOCK_LOCK			   \
		    if (!pthread_mutex_lock(&clock_lock)) {\
			if (!clock_started) {		   \
				start = clock();	   \
				printf("start = %ld\n", start);\
				clock_started = 1;	   \
			}				   \
		    }					   \
		    pthread_mutex_unlock(&clock_lock);
#define STOP_CLOCK if (!pthread_mutex_lock(&clock_lock)) { \
			if (clock_started) {		   \
				end = clock();		   \
				printf("end = %ld\n", end);	\
				clock_started = 0;	   \
			}				   \
		   }					   \
		   pthread_mutex_unlock(&clock_lock);	   \
		   DEST_CLOCK_LOCK
#define CPU_TIME    end - start
#define WALL_CLOCK_TIME  CPU_TIME / CLOCKS_PER_SEC
