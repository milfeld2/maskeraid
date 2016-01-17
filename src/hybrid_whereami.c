/*   hybrid_whereami.c is a driver 
     1.) Get line arguments (optional):  help or number o seconds for load
     2.) Start MPI
          Option to EASILY set affinity here.
     3.) Store map for each rank
     4.) Start OpenMP parallel region
     5.) Store map for each thread
       ) MPI call to report 
     6.) Set a work load on each thread
     7.) Finish parallel region 
     5.) Stop MPI
                                         Kent Milfeld 12/16/15
*/

#include <stdio.h>
#include <omp.h>
#include <mpi.h>


void load_cpu_nsec(int nsec);
void hybrid_report_mask(void);
int  map_to_cpuid( int icore);


int main(int argc, char **argv){
int rank, nranks;         // MPI variables.
int nthrds, thrd, cpuid;  //Thread info

int nsec = 10;     // Load, default time

int ierr;          // Error number

   cmdln_get_nsec_or_help( &nsec, argc, argv); //optional, get nsec from cmd line

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &nranks);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   hybrid_report_mask();                     // Store MPI masks

   #pragma omp parallel private(thrd,ierr)
   {
      thrd   =   omp_get_thread_num();
      nthrds =   omp_get_num_threads();

      cpuid  =   thrd;                     // set cpuid to thread number (thrd)
  //  ierr   =   map_to_cpuid( cpuid );    // set your own affinity here 

      hybrid_report_mask();        // Call mask reporter
      load_cpu_nsec( nsec );       // Load up rank process so user can watch top.
   }

   MPI_Finalize();

}
