/*
 ============================================================================
 Name        : CDemos.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdio.h>
#include <sys/time.h>

int
main(void)
{
   timebasestruct_t start, finish;
   int val = 3;
   int secs, n_secs;
   struct timeval tvs,tvf;
   /* get the time before the operation begins */
   read_real_time(&start, TIMEBASE_SZ);
   gettimeofday(&tvs,NULL);
   /* begin code to be timed */
   (void) printf("This is a sample line %d \n", val);
   /* end code to be timed   */

   /* get the time after the operation is complete */

   read_real_time(&finish, TIMEBASE_SZ);
   gettimeofday(&tvf,NULL);

   /*
    * Call the conversion routines unconditionally, to ensure
    * that both values are in seconds and nanoseconds regardless
    * of the hardware platform.
    */
   time_base_to_time(&start, TIMEBASE_SZ);
   time_base_to_time(&finish, TIMEBASE_SZ);

   /* subtract the starting time from the ending time */
   secs = finish.tb_high - start.tb_high;
   n_secs = finish.tb_low - start.tb_low;
   long s = tvs.tv_sec*1000*1000 + tvs.tv_usec;
   long f = tvf.tv_sec*1000*1000 + tvf.tv_usec;
   printf("finish.flag=%d,finish.tb_high=%d,finish.tb_low=%d\n",finish.flag,finish.tb_high,finish.tb_low);
   printf("start.flag=%d,start.tb_high=%d,start.tb_low=%d\n",start.flag,start.tb_high,start.tb_low);
   printf("tvs.tv_sec=%d,tvs.tv_usec=%d\n",tvs.tv_sec,tvs.tv_usec);
   printf("tvf.tv_sec=%d,tvf.tv_usec=%d\n",tvf.tv_sec,tvf.tv_usec);

  /*
   * If there was a carry from low-order to high-order during
   * the measurement, we may have to undo it.

   if (n_secs < 0)  {
      secs--;
      n_secs += 1000000000;
      }

   (void) printf("Sample time was %d seconds %d nanoseconds\n",
                secs, n_secs);
*/
   exit(0);
}
