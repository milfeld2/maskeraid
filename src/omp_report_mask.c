#include <stdio.h>
#include <omp.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

void omp_report_mask(){

int nthrds, thrd;  //Thread info

int ncpus, nel_set;
static int ** proc_mask;
int i,j, ierr;
char *  dummy;

   thrd   =  omp_get_thread_num();
   nthrds =  omp_get_num_threads();
   ncpus  =  (int) sysconf(_SC_NPROCESSORS_ONLN);
   

   if(omp_get_num_procs() != ncpus){
         printf("ERROR: ncpus_by_omp=%d, ncpus_sched=%d\n",omp_get_num_procs(),ncpus);
         exit(1);
   }


   #pragma omp single
   {
      proc_mask =                           malloc(sizeof(int*)*nthrds);
      for(i=0;i<nthrds;i++) proc_mask[i] =  malloc(sizeof(int)*ncpus  );
      for(i=0;i<nthrds;i++) for(j=0;j<ncpus;j++) proc_mask[i][j] =0;
   }

   ierr = boundto(&nel_set,proc_mask[thrd]);

   #pragma omp barrier
   #pragma omp single
   {
          print_mask(1,  dummy,  0,     0,0,   ncpus, nthrds,1, proc_mask[thrd]);  //print header
      for(thrd=0;thrd<nthrds;thrd++){
         print_mask(0,   dummy,  0,  thrd,0,   ncpus, nthrds,1, proc_mask[thrd]);
      }
   }
   
}
