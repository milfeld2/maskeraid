/*   mpi_mapper is a driver 
     1.) Get line arguments (optional):  help or number o seconds for load
     2.) Start MPI
          Option to EASILY set affinity here.
     3.) Call report_mask
     4.) Set a work load for each rank
     5.) Stop MPI
                                         Kent Milfeld 12/16/15
*/

#include <stdio.h>
#include <mpi.h>

                                  // basic routes
void cmdln_get_nsec_or_help(int * nsec, int argc, char * argv[]);
void          load_cpu_nsec(int nsec );
int            hpc_affinity(int icore);
void        mpi_report_mask();
int           map_to_cpuid(int cpuid);          // set your own affinity here 


int main(int argc, char * argv[]){

int i,j;                  // General

int rank, nranks;         // MPI variables.

int nsec = 10;            // Default time for load

int cpuid;                // For mapping this process to cpuid in this variabl


// cmdln_get_nsec_or_help( &nsec, argc, argv); //optional, get nsec from cmd line


   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &nranks);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   // cpuid=rank;                     // set rank to cpuid
   // map_to_cpuid( cpuid );          // set your own affinity here 
                                      // (e.g. map this process to cpuid equal to this rank.

   mpi_report_mask();                 // Call mask reporter

   load_cpu_nsec( nsec );             // Load up rank process so user can watch top.

   MPI_Finalize();

}
