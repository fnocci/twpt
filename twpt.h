/*
 * twpt -- the tw pthreads problem
 *
 * Copyright (C) 2010  Cheryl Fillekes
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

/* 
 * twpt.h 
 *
 * header file for tw pthreads problem 
 *
*/

#ifndef PTATM_H_
#define PTATM_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
/*#include <error.h>*/
#include <time.h>
#include <getopt.h>
#include <pthread.h>

enum {VERBOSE_OFF, 
      VERBOSE_ON};

typedef struct patm_opts_s {
    unsigned long  T;  /* number of Threads     */
    unsigned long  X;  /* Xfer amount           */
    unsigned long  N;  /* Number of accounts    */
    unsigned long  M;  /* initial Max balance   */
    unsigned int   s;  /* seed for rand         */
    unsigned short q;  /* minimum queue size    */
    unsigned char  v;  /* verbose on or off     */
    } twpt_opts_t;

int twpt_get_options (int argc, char **argv, twpt_opts_t *opts);
void twpt_init_options(twpt_opts_t *opts); 

typedef struct twpt_acct_s {
    unsigned long id; 
      signed long balance;
    } twpt_acct_t;

typedef struct twpt_acct_pair_s {
    twpt_acct_t     source;
    twpt_acct_t     dest;
    pthread_cond_t  ready_cv;
    pthread_mutex_t mutex;
} twpt_acct_pair_t;

/* initialization functions */

twpt_acct_t *twpt_init_accounts (const twpt_opts_t opts) ;

twpt_acct_t  *twpt_init_avail_accounts (const int T,
                                        const int N,
                                        const int q,
                                        int *num_avail_accts);

int twpt_bad_options (const twpt_opts_t opts);

/* functions called by main */

int twpt_get_acct_pair (const int min_avail_accts,
                              int num_avail_accts, 
                              twpt_acct_t *twpt_avail_acct, 
                              twpt_acct_pair_t *twpt_acct_pair);

/* functions called by thread */
twpt_acct_pair_t *twpt_init_thread_data (const int T, const int N, const int q);
void twpt_thread_fun (twpt_acct_pair_t *twpt_acct_pair); 

/* utilities */
int  twpt_copy_acct (const twpt_acct_t from, twpt_acct_t *to);
int  twpt_copy_acct_pair (const twpt_acct_pair_t from, twpt_acct_pair_t *to);

void twpt_print_accounts (const char *header, const unsigned long N, twpt_acct_t *accts);
void twpt_print_accounts_available (const unsigned long N);
void twpt_print_account_pairs (const char *header, const unsigned long T, twpt_acct_pair_t *twpt_acct_pair); 
void twpt_print_usage(void);
void twpt_print_account (const char *header, twpt_acct_t *twpt_accts);
void twpt_free_accounts (twpt_acct_t *accts);

#endif /* PTATM_H_ */
