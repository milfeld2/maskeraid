#include <stdio.h>
#include <omp.h>

void load_cpu_nsec(int nsec);
void omp_report_mask();
int  map_to_cpuid( int icore);


int main(){

int nthrds, thrd, cpuid;  //Thread info

int nsec = 10;     // Load, default time

int ierr;          // Error number


   #pragma omp parallel private(thrd,ierr)
   {
      thrd   =   omp_get_thread_num();
      nthrds =   omp_get_num_threads();

      cpuid  =   thrd;                      // set cpuid to thread number (thrd)
      ierr   =   map_to_cpuid( cpuid );     // set your own affinity here 

      omp_report_mask();        // Call mask reporter
      load_cpu_nsec( nsec );    // Load up rank process so user can watch top.
   }

}
