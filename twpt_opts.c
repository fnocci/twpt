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

#include  "twpt.h"
/*  twpt_get_options
 *  process tw pthreads problem command-line options
 */

const char *twpt_usage = "usage: twpt -h (help)\n\t"
                                       "-T <numThreads>\n\t"
                                       "-X <Xfer amt>\n\t"
                                       "-N <Num accounts>\n\t"
                                       "-M <initial Max balance>\n\t"
                                       "-q <min queue size>\n\t"
                                       "-s <seed for rand>\n\t"
                                       "-v verbose\n";

void twpt_print_usage(void) {
    fprintf (stderr, "%s\n", twpt_usage);
}

void twpt_init_options(twpt_opts_t *opts) {
        opts->T = 10;           /* default 10 Threads  */
        opts->X = 1;            /* Xfer one dollar     */
        opts->N = 100;          /* 100 fake accounts   */
        opts->M = 1000;         /* 1000 dollars        */
        opts->s = time(0);      /* timestamp rand seed */
        opts->q = 5;            /* minimum queue size  */
        opts->v = VERBOSE_OFF;  /* verbose on or off   */
}

int twpt_get_options(int argc, char *argv[], twpt_opts_t *opts) {
  int option = 0;
  long long drop  = 0; 
  char **optend = NULL;

  while ((option = getopt(argc, argv, "hT:X:N:M:q:s:v")) != EOF) {
    switch (option) {
      case 'v':
        opts->v = VERBOSE_ON;
        break;

      case 'T':
        errno = 0;
        drop = strtoll (optarg, optend, 0);
        if (errno != 0 || (drop <= 0 || drop >= ULONG_MAX)) {
           fprintf (stderr, "number of Threads must be positive long int");
           fprintf (stderr, "%s\n", twpt_usage)  ;
           exit (EXIT_FAILURE);
        } else {
           opts->T = (unsigned long) drop;
           if (opts->v == VERBOSE_ON) fprintf (stderr, "T=%lu\n",opts->T);
        }
        break;

      case 'X':
        errno = 0;
        drop = strtoll (optarg, optend, 0);
        if (errno != 0 || (drop <= 0 || drop >= ULONG_MAX)) {
           fprintf (stderr, "Xfer dollar amt must be positive long int");
           fprintf (stderr, "%s\n", twpt_usage)  ;
           exit (EXIT_FAILURE);
        } else {
           opts->X = (unsigned long) drop;
           if (opts->v == VERBOSE_ON) fprintf (stderr, "X=%lu\n", opts->X);
        }
        break;

      case 'N':
        errno = 0;
        drop = strtoll (optarg, optend, 0);
        if (errno != 0 || (drop <= 0 || drop >= ULONG_MAX)) {
           fprintf (stderr, "Number of accounts must be positive long int");
           fprintf (stderr, "%s\n", twpt_usage)  ;
           exit (EXIT_FAILURE);
        } else {
           opts->N = (unsigned long) drop;
           if (opts->v == VERBOSE_ON) fprintf (stderr, "N=%lu\n", opts->N);
        }
        break;

      case 'M':
        errno = 0;
        drop = strtoll (optarg, optend, 0);
        if (errno != 0 || (drop <= 0 || drop >= ULONG_MAX)) {
           fprintf (stderr, "maximum balance must be positive long int");
           fprintf (stderr, "%s\n", twpt_usage)  ;
           exit (EXIT_FAILURE);
        } else {
           opts->M = (unsigned long) drop;
           if (opts->v == VERBOSE_ON) fprintf (stderr, "M=%lu\n", opts->M);
        }
        break;

      case 's':
        errno = 0;
        drop = strtoll (optarg, optend, 0);
        if (errno != 0 || (drop <= 0 || drop >= ULONG_MAX)) {
           fprintf (stderr, "Seed for random number generator must be postive int");
           fprintf (stderr, "%s\n", twpt_usage)  ;
           exit (EXIT_FAILURE);
        } else {
          opts->s = (unsigned long) drop;
          if (opts->v == VERBOSE_ON) fprintf (stderr, "seed=%u\n", opts->s); 
        }
        break;

      case 'q':
        errno = 0;
        drop = strtoll (optarg, optend, 0);
        if (errno != 0 || (drop <= 0 || drop >= USHRT_MAX)) {
           fprintf (stderr, "minimum accounts available queue size must be positive num < USHRT_MAX\n");
           fprintf (stderr, "%s\n", twpt_usage)  ;
           exit (EXIT_FAILURE);
        } else {
           opts->q = (unsigned long) drop;
           if (opts->v == VERBOSE_ON) fprintf (stderr, "minimum queue size=%d\n",opts->q);
        }
        break;

      case 'h':
      default:
        fprintf (stderr, "%s\n", twpt_usage);
        exit(EXIT_FAILURE);
        break;
    }
  }
  return -1;
}

int twpt_bad_options (const twpt_opts_t opts){ 
  if (opts.q+2  >= opts.N) {
     fprintf (stderr, "minimum accounts available buffer + 2 must be smaller than total number of accounts!\n");
     exit (EXIT_FAILURE);
  }
  return 1; 
}
