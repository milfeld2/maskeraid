#include <stdlib.h>
#include <sys/time.h>

double gtod_secbase = 0.0E0;

double gtod_timer()
{
   struct timeval tv;
   void *Tzp=0;
   double sec;

   gettimeofday(&tv, Tzp);

               /*Always remove the LARGE sec value
                 for improved accuracy  */
   if(gtod_secbase == 0.0E0)
      gtod_secbase = (double)tv.tv_sec;
   sec = (double)tv.tv_sec - gtod_secbase;

   return sec + 1.0E-06*(double)tv.tv_usec;
}
double gtod_timer_()
{
   struct timeval tv;
   void *Tzp=0;
   double sec;

   gettimeofday(&tv, Tzp);

               /*Always remove the LARGE sec value
                 for improved accuracy  */
   if(gtod_secbase == 0.0E0)
      gtod_secbase = (double)tv.tv_sec;
   sec = (double)tv.tv_sec - gtod_secbase;

   return sec + 1.0E-06*(double)tv.tv_usec;
}


static __inline unsigned long long tsc(void){
  unsigned long a, d;
  unsigned long long d2;

  __asm__ __volatile__ ("rdtsc" : "=a" (a), "=d" (d));

  d2 = d;
  return (unsigned long long) a | (d2 << 32);
};

unsigned long long   tsc_timer(void){ return  tsc(); }
unsigned long long  tsc_timer_(void){ return  tsc(); }
