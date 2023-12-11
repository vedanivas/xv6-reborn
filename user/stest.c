// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user/user.h"
// #include "kernel/fcntl.h"

// #define NFORK 10
// #define IO 5

// int main() {
//   int n, pid;
//   int wtime, rtime;
//   int twtime=0, trtime=0;
//   for(n=0; n < NFORK;n++) {
//       pid = fork();
//       if (pid < 0)
//           break;
//       if (pid == 0) {
// #ifndef FCFS
//           if (n < IO) {
//             sleep(200); // IO bound processes
//           } else {
// #endif
//             for (volatile int i = 0; i < 1000000000; i++) {} // CPU bound process 
// #ifndef FCFS
//           }
// #endif
//           printf("Process %d finished\n", n);
//           exit(0);
//       } else {
// #ifdef PBS
//         set_priority(80, pid); // Will only matter for PBS, set lower priority for IO bound processes 
// #endif
//       }
//   }
//   for(;n > 0; n--) {
//       if(waitx(0,&wtime,&rtime) >= 0) {
//           trtime += rtime;
//           twtime += wtime;
//       } 
//   }
//   printf("Average rtime %d,  wtime %d\n", trtime / NFORK, twtime / NFORK);
//   exit(0);
// }


/*
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"


#define NFORK 10
#define IO 5

int main() {
  int n, pid;
  int wtime, rtime;
  int twtime=0, trtime=0;
  for (n=0; n < NFORK;n++) {
      pid = fork();
      if (pid < 0)
          break;
      if (pid == 0) {
          if (n < IO) {
            sleep(200); // IO bound processes
          } else {
            for (int i = 0; i < 10000000; i++) {}; // CPU bound process
          }
          // printf("Process %d finished", n);
          exit(0);
      } else {
#ifdef PBS
        set_priority(60-IO+n, pid); // Will only matter for PBS, set lower priority for IO bound processes 
#endif
      }
  }
  for(;n > 0; n--) {
      if(waitx(0,&wtime,&rtime) >= 0) {
          trtime += rtime;
          twtime += wtime;
      } 
  }
  printf("Average rtime %d,  wtime %d\n", trtime / NFORK, twtime / NFORK);
  exit(0);
}
*/

#define N 624
#define M 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */   
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

#define RAND_MAX 0x7fffffff

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* initializing the array with a NONZERO seed */
void
sgenrand(unsigned long seed)
{
    /* setting initial seeds to mt[N] using         */
    /* the generator Line 25 of Table 1 in          */
    /* [KNUTH 1981, The Art of Computer Programming */
    /*    Vol. 2 (2nd Ed.), pp102]                  */
    mt[0]= seed & 0xffffffff;
    for (mti=1; mti<N; mti++)
        mt[mti] = (69069 * mt[mti-1]) & 0xffffffff;
}

long /* for integer generation */
genrand()
{
    unsigned long y;
    static unsigned long mag01[2]={0x0, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if sgenrand() has not been called, */
            sgenrand(4357); /* a default initial seed is used   */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];

        mti = 0;
    }
  
    y = mt[mti++];
    y ^= TEMPERING_SHIFT_U(y);
    y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
    y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
    y ^= TEMPERING_SHIFT_L(y);

    // Strip off uppermost bit because we want a long,
    // not an unsigned long
    return y & RAND_MAX;
}

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define NFORK 10
#define IO 5

int main() {
  int n, pid;
  int wtime, rtime;
  int twtime=0, trtime=0;
  for(n=0; n < NFORK;n++) {
      pid = fork();
      if (pid < 0)
          break;
      if (pid == 0) {
#ifndef FCFS
          if (n < IO) {
            sleep(200); // IO bound processes
          } else {
#endif
            for (volatile int i = 0; i < 1000000000; i++) {} // CPU bound process 
#ifndef FCFS
          }
#endif
          printf("Process %d finished\n", n);
          exit(0);
      } else {
#ifdef PBS
        int n = ((genrand() % 1000) + 1) / 10;
        if (n == 0) n = 15;
        set_priority(n, pid); // Will only matter for PBS, set lower priority for IO bound processes 
#endif

// #ifdef LBS
    // int n = (genrand() % 100) + 1;
    // settickets(10);
// #endif
      }
  }
  for(;n > 0; n--) {
      if(waitx(0,&wtime,&rtime) >= 0) {
          trtime += rtime;
          twtime += wtime;
      } 
  }
  printf("Average Run Time %d\nAverage Wait Time %d\n", trtime / NFORK, twtime / NFORK);
  exit(0);
}
