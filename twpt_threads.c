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

/* algorithm:
 *
 * each thread processes its initial account pair unlocked on the accounts table,
 *    goes right to waiting on the non-zero condition variable if it gets the zeroth accounts
 *    in detail:
 *    using the locked pair passed to it by the main thread from the available accounts table
 *      if not 0 0:
 *         test the 2 new account balances
 *            if transaction will result in one zero balance: 
 *               lock transaction counter, increment it and report result to stdout.
 *               clean up and exit
 *            if transaction it will succeed with 2 nonzero non-negative balances:
 *               update both accounts in the table
 *               lock transaction counter, increment it and unlock immediately.
 *      wait for more data
 */

#include "twpt.h"

extern twpt_acct_t      *twpt_account;
extern twpt_acct_pair_t *twpt_thread_data;
extern twpt_acct_t      *twpt_avail_account;
extern pthread_t        *twpt_threads;

extern unsigned long T;
extern unsigned long X;
extern unsigned char v;

extern unsigned long *twpt_idlut; 
extern unsigned long long Xcount;
extern pthread_mutex_t Xcount_mutex;

void twpt_thread_fun (twpt_acct_pair_t *twpt_acct_pair) {
 
  int true=1;
  twpt_acct_t *source;
  twpt_acct_t *dest;
  
  pthread_mutex_lock (&twpt_acct_pair->mutex); /* could rwlock here to go faster */
                                               /* since thread only reads this   */
                                               /* & main reads it before writing */
  while (true) {

    if (v == VERBOSE_ON) twpt_print_account_pairs ("\nTgiven:\t", 1, twpt_acct_pair);
    if (twpt_acct_pair->source.id > 0 && twpt_acct_pair->dest.id > 0) {

       source = &twpt_account [ twpt_idlut [ twpt_acct_pair->source.id ]];
       dest   = &twpt_account [ twpt_idlut [ twpt_acct_pair->dest.id   ]];

       if (v == VERBOSE_ON) {
         twpt_print_account ("\nsource:\t",  source);
         twpt_print_account ("dest:  \t",  dest);
       }
       if (source->balance - X  == 0 || dest->balance - X == 0) {
          if (v == VERBOSE_ON) fprintf (stderr, "Eureka!\n");

          pthread_mutex_lock (&Xcount_mutex); 
          printf ("The number of transactions is %llu\n", ++Xcount);
          pthread_mutex_unlock (&Xcount_mutex); 

          /* clean up and go home */
          free (twpt_account);
          free (twpt_thread_data);
          free (twpt_threads);
          free (twpt_avail_account);
          exit (EXIT_SUCCESS);

       } else {
          source->balance -= X; 
          dest->balance   += X;

          if (v == VERBOSE_ON) {
            twpt_print_account ("source:\t",  source);
            twpt_print_account ("dest:  \t",  dest);
          }
          pthread_mutex_lock (&Xcount_mutex); 
          Xcount++;
          pthread_mutex_unlock (&Xcount_mutex); 
          if (v == VERBOSE_ON) fprintf (stderr, "Xcount=%llu\n", Xcount);
       }
    } 
    if (v == VERBOSE_ON) fprintf (stderr, "\nwaiting....\n");
    pthread_cond_wait (&twpt_acct_pair->ready_cv, &twpt_acct_pair->mutex);

    if (v == VERBOSE_ON) {
      twpt_print_account_pairs ("message received!", 1, twpt_acct_pair);
     }
  }
  pthread_mutex_unlock (&twpt_acct_pair->mutex);
}
