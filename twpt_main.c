/* 
 * twpt_main.c 
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
#include "twpt.h"

twpt_acct_t      *twpt_account;
twpt_acct_t      *twpt_avail_account;
twpt_acct_pair_t *twpt_thread_data;
pthread_t        *twpt_threads;

unsigned long T;
unsigned long X;
unsigned char v;
unsigned long long Xcount=0;
pthread_mutex_t Xcount_mutex = PTHREAD_MUTEX_INITIALIZER;

int main (int argc, char **argv) {

  int twpt_opts_scanerr = 0;
  int twpt_opts_bad_err = 0;
  int twpt_lock_unlocked = 0;
  int num_avail_accts=0;
  twpt_acct_pair_t rand_acct_pair;
  twpt_acct_pair_t bot_acct_pair;
  twpt_opts_t opts; 
  int i;
  int randi;
  int randj;
  int true=1;

  unsigned short q;
  unsigned long  N;

  twpt_init_options (&opts);

  /* scan options and check for data type errors */
  if (!(twpt_opts_scanerr = twpt_get_options (argc, argv, &opts))) {
    twpt_print_usage();
    exit (EXIT_FAILURE);
  }

  /* evaluate option values for consistency with each other */
  if (!(twpt_opts_bad_err = twpt_bad_options (opts))) {
    exit (EXIT_FAILURE);
  }

  X = opts.X;  /* amount to be Xfered */
  v = opts.v;  /* share the verbosity */
  T = opts.T;
  q = opts.q;
  N = opts.N;

  /* initialize account ids and balances */
  twpt_account = twpt_init_accounts (opts);
  if (v == VERBOSE_ON) 
    twpt_print_accounts ("accounts table\n", N, twpt_account);

  /* initialize account ids and balances */
  twpt_avail_account = twpt_init_avail_accounts (T, N, q, &num_avail_accts);
  if (v == VERBOSE_ON) 
    twpt_print_accounts ("initial avail accts\n", opts.N, twpt_avail_account);

  /* initialize initial thread data */
  twpt_thread_data = twpt_init_thread_data (T, N, q);
  if (v == VERBOSE_ON) 
    twpt_print_account_pairs ("init thread data:\n", T, twpt_thread_data);

  /* initialize thread array */
  twpt_threads = (pthread_t *) calloc (T, sizeof (pthread_t));

  /* launch threads */
  if (v == VERBOSE_ON) fprintf (stderr, "thread data from launched threads\n");
  for (i=0; i < T; i++) {
     pthread_create (&twpt_threads[i], NULL, 
                    (void *) twpt_thread_fun, (void *) &twpt_thread_data[i]);
  }
  /* manage thread data */

  while (true) {
    for (i=0; i<T; i++) {

      randi = randj = 0;

      while (randi == randj) {
        randi = 1 + (int) ((num_avail_accts) * (random()/(RAND_MAX + 1.0)));
        randj = 1 + (int) ((num_avail_accts) * (random()/(RAND_MAX + 1.0)));
      }

      twpt_copy_acct (twpt_avail_account [randj], &rand_acct_pair.source);
      twpt_copy_acct (twpt_avail_account [randi], &rand_acct_pair.dest);

      twpt_copy_acct (twpt_avail_account [num_avail_accts],   &bot_acct_pair.source);
      twpt_copy_acct (twpt_avail_account [num_avail_accts-1], &bot_acct_pair.dest);

      if (!(twpt_lock_unlocked = pthread_mutex_trylock(&twpt_thread_data[i].mutex))){
        if (num_avail_accts >= q+2) { 
          if (twpt_thread_data[i].source.id > 0 && twpt_thread_data[i].dest.id > 0) {
             
            if (v == VERBOSE_ON) fprintf (stderr, "thread has data and accts avail does too, so swap\n");

            twpt_copy_acct (twpt_thread_data[i].dest,     &twpt_avail_account [randj]);
            twpt_copy_acct (twpt_thread_data[i].source,   &twpt_avail_account [randi]);
 
            twpt_copy_acct_pair (rand_acct_pair, &twpt_thread_data[i]);

            if (v == VERBOSE_ON) {
               fprintf (stderr, "both swap, num_avail_accts=%d\n", num_avail_accts);
               twpt_print_accounts ("curr avail accts\n", N, twpt_avail_account);
            } 
          } else { 
            if (v == VERBOSE_ON) {fprintf (stderr, "thread data emtpy but accts avail has data for it\n");} 
            twpt_copy_acct_pair (bot_acct_pair, &twpt_thread_data[i]);
            num_avail_accts -= 2;
            }
        } else {
          if (twpt_thread_data[i].source.id > 0 && twpt_thread_data[i].dest.id > 0) {
            if (v == VERBOSE_ON) fprintf (stderr, "thread has data but avail accts down to min\n");
            /* add thread data to bottom of avail_account list but then swap it to that randi location */

            twpt_copy_acct (twpt_thread_data[i].source, &twpt_avail_account[++num_avail_accts]);
            twpt_copy_acct (twpt_thread_data[i].dest,   &twpt_avail_account[++num_avail_accts]);
  
            twpt_copy_acct (twpt_avail_account[num_avail_accts],   &twpt_avail_account[randi]);
            twpt_copy_acct (twpt_avail_account[num_avail_accts-1], &twpt_avail_account[randj]);
  
            twpt_copy_acct (rand_acct_pair.dest, &twpt_avail_account[num_avail_accts]);
            twpt_copy_acct (rand_acct_pair.source, &twpt_avail_account[num_avail_accts-1]);
          }
        /* if this thread has no data and accts avail is down to min, spin */
        }
            if (v == VERBOSE_ON) {
               fprintf (stderr, "add t data, num_avail_accts=%d\n", num_avail_accts);
               twpt_print_accounts ("curr avail accts\n", N, twpt_avail_account);
            } 

      pthread_cond_signal  (&twpt_thread_data[i].ready_cv);
      pthread_mutex_unlock(&twpt_thread_data[i].mutex); 
      }
    }
  } 

  /* free account data */
  free (twpt_account);
  free (twpt_avail_account);
  free (twpt_thread_data);
  free (twpt_threads);
  exit (EXIT_SUCCESS);
}

