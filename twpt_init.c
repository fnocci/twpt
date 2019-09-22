/*
 * twpt_init.c 
 * 
 * initialize and shuffle the accounts table
 * generate a lookup table to find accts later
 * initialize the available accounts buffer
 *
 */
#include "twpt.h"

extern twpt_acct_t  *twpt_account;
extern unsigned long X;
extern unsigned char v;
unsigned long *twpt_idlut; /* id lookup table */

twpt_acct_t  *twpt_init_accounts (const twpt_opts_t opts) {

  twpt_acct_t  *acct_ptr;
  int i; 
  unsigned int swap_value;
  unsigned int swap_index;

  acct_ptr = (twpt_acct_t *) calloc ((opts.N+1) , sizeof (twpt_acct_t));

  if (acct_ptr == NULL) {
    fprintf (stderr,"error allocating memory for accounts buffer, exiting\n");
    exit (EXIT_FAILURE);
    }

  acct_ptr[0].id=0;
  acct_ptr[0].balance=-1; 

  srandom (opts.s);            

  for (i=1; i<opts.N+1; i++) { /* assign pos random balances between 1 and M */
    acct_ptr[i].id=i;
    acct_ptr[i].balance = 1 + (int) (opts.M * (random()/(RAND_MAX + 1.0)));
  }

  if (v == VERBOSE_ON) twpt_print_accounts ("init accts\n", opts.N, acct_ptr); 

  for (i=1; i< opts.N+1; i++) { /* shuffle the account ids */

    swap_index =  1 + (int) ((opts.N) * (random()/(RAND_MAX + 1.0)));
    swap_value = acct_ptr[swap_index].id;

    acct_ptr[swap_index].id = acct_ptr[i].id;
    acct_ptr[i].id = swap_value;
  }

  if (v == VERBOSE_ON) twpt_print_accounts ("shuffled accts\n", opts.N, acct_ptr); 

  twpt_idlut = (unsigned long *) calloc ((opts.N+1), sizeof (unsigned long));

  for (i=1; i<= opts.N; i++) twpt_idlut[acct_ptr[i].id]=i; 

  if (v == VERBOSE_ON) {
    fprintf (stderr, "twpt_idlut\n");
    for (i=1; i< opts.N+1; i++) fprintf (stderr, "(%lu,%d),",twpt_idlut[i],i); 
    fprintf (stderr, "\n");
  }
  return acct_ptr;
}

twpt_acct_t  *twpt_init_avail_accounts (const int T, 
                                        const int N,
                                        const int q, 
                                        int *num_avail_accts) {
  int i, j;
  int twpt_acct_copy_err = 0;
  twpt_acct_t *twpt_avail_accts;

  /* make the buffer size N+1 in (unlikely) case all of the threads return their data */
  /* and again, leave the zeroth indexed account flagged for null data */

  twpt_avail_accts = (twpt_acct_t *) calloc ((N+1) , sizeof (twpt_acct_t));

  if (twpt_avail_accts == NULL) {
    fprintf (stderr,"error allocating memory for accounts available buffer, exiting\n");
    exit (EXIT_FAILURE);
    }

  /* ensure we start out with at least q+2 accounts in the feed buffer   */
  /* so that we can start off with at least one active thread processing */

  if (0 > (N - (q+2))) {
    fprintf (stderr,"initial min avail acct buffer size %d larger than "
                    "initial number of accounts %d, exiting\n", q, N);
    exit (EXIT_FAILURE);
  } 
  if ( 2*T >= N - (q + 2)  ) { 
    *num_avail_accts = q + 2 + (N-(q+2))%2;  
    } else {                      
      *num_avail_accts = (N - 2*T) + (N-2*T)%2 ;
      } 

  twpt_avail_accts[0].id = 0;
  twpt_avail_accts[0].balance = -1;

  for (i=N, j=1; i>N-*num_avail_accts; i--, j++) {

    if (!(twpt_acct_copy_err = twpt_copy_acct (twpt_account[i],&(twpt_avail_accts[j])))) {
      fprintf (stderr, "error initializing available accounts buffer");
      exit (EXIT_FAILURE);
    } 
  }
  return twpt_avail_accts;
}

twpt_acct_pair_t *twpt_init_thread_data (const int T, const int N, const int q) {

  int i;
  int j;
  twpt_acct_pair_t *thread_data;

  thread_data = (twpt_acct_pair_t *) calloc (T, sizeof (twpt_acct_pair_t));
  
  for (i=0, j=1; i < T; i++, j=j+2) {

     pthread_cond_init  (&thread_data [i].ready_cv, NULL);
     pthread_mutex_init (&thread_data [i].mutex, NULL);
 
     if (j < N-(q+2)) {
        twpt_copy_acct (twpt_account[j], &thread_data [i].source);
        twpt_copy_acct (twpt_account[j+1], &thread_data [i].dest);
        
     } else {
        twpt_copy_acct (twpt_account[0], &thread_data [i].source);
        twpt_copy_acct (twpt_account[0], &thread_data [i].dest);
     }
   } 
  return thread_data;
}

void twpt_free_accounts (twpt_acct_t  *accts) {
  free (accts);
}

void twpt_print_accounts (const char *header, unsigned long N, twpt_acct_t *twpt_accts) {
  int i;
  fprintf (stderr, "%s", header);
  for (i=0; i<=N; i++) {
    fprintf (stderr,"%d\t", i);
    fprintf (stderr,"%lu\t", twpt_accts[i].id);
    fprintf (stderr,"%ld\n", twpt_accts[i].balance);
  }
}

void twpt_print_account (const char *header, twpt_acct_t *twpt_accts) {
  fprintf (stderr, "\n%s", header);
  fprintf (stderr,"%lu\t", twpt_accts->id);
  fprintf (stderr,"%ld\t", twpt_accts->balance);
}

void twpt_print_account_pairs (const char *header, 
                               unsigned long T, 
                               twpt_acct_pair_t *twpt_acct_pair) {
  int i;
  fprintf (stderr, "%s", header);
  for (i=0; i<T; i++){
    fprintf (stderr, "%lu\t%ld\t%lu\t%ld\n", 
                      twpt_acct_pair [i].source.id, 
                      twpt_acct_pair [i].source.balance,
                      twpt_acct_pair [i].dest.id,
                      twpt_acct_pair [i].dest.balance);
   }
}

int twpt_copy_acct (const twpt_acct_t from, twpt_acct_t *to) {

  if (to == NULL) return 0;  /* no space allocated */
  else {
    to->id = from.id;
    to->balance = from.balance;
  }
  return 1;  /* in the real world would make these named enums */
}

int twpt_copy_acct_pair (const twpt_acct_pair_t from, twpt_acct_pair_t *to) {

  int err=0;
  if (to == NULL) return 0;
  else {
    if (!(err=twpt_copy_acct (from.source, &to->source))) return 0;
    if (!(err=twpt_copy_acct (from.dest,   &to->dest))) return 0;
  }
  return 1;
}
