/*   hybid,id report_mask collects and prints the masks from each rank.
     1.) Get mask for each rank (and integer array -- no hex).
            Root gathers masks and prints them
     2.) Get node name from each rank.
            Determine and use maximum length of node name
     3.) Call print_mask.
*/

#include <stdio.h>
#include <mpi.h>
#include <omp.h>
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

                                  // basic routes
void print_mask(int hd_prnt, char* name, int multi_node, int rank, int thrd, int ncpus, int nranks, int nthrds, int *proc_mask);


int hybrid_report_mask(void){

int thrd, nthrds;
int rank, nranks;

static int multi_node = 0;

int ncpus, nel_set;

static int ** omp_proc_mask;
static int  * omp_mask_pac;
char *dummy;


       char   proc_name[MPI_MAX_PROCESSOR_NAME];
static char * all_names;
       int    name_len;
static int    max_name_len;

                          // General
int i,j,ierr;
int id, rid,tid;
int in_mpi, in_omp;

                          // Mask storage
       int ** proc_mask;
static int  * all_masks=0;


   MPI_Initialized(&in_mpi);
   in_omp = omp_in_parallel();

   if(in_mpi != 0 && in_omp == 0){

                     // Get number of cpus (this gives no. of cpu_ids in /proc/cpuinfo)
                     // Get rank number & no of ranks via MPI
     ncpus = (int) sysconf(_SC_NPROCESSORS_ONLN);
     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
     MPI_Comm_size(MPI_COMM_WORLD, &nranks);
  
                     // Create a 2-D array for mask
                     // proc_mask[rank][ncpus] -- for simplicity, size is [ncpus][ncpus]
                     // Thinking ahead for hybrid code.
  
                     // zero out proc_mask[ncpus][ncpus]  
                     // I could have made proc_mask a single array (proc_mask[ncpus]); but didn't
                     // This is a hold-over from the openmp version that holds everything for all threads.
                     // For MPI I made a continguous collection array (all_masks).
     proc_mask =          malloc(sizeof(int*)*ncpus);
     for(i=0;i<ncpus;i++) proc_mask[i] =  malloc(sizeof(int)*ncpus);
     for(i=0;i<ncpus;i++) for(j=0;j<ncpus;j++) proc_mask[i][j] =0;
     all_masks =  (int *) malloc(sizeof(int)*ncpus*ncpus);
  
                               // get map for this processor
     ierr=boundto(&nel_set,proc_mask[rank]);

                                  // Gather information to rank 0
  
     MPI_Gather( proc_mask[rank], ncpus, MPI_INT,
                 all_masks,    ncpus, MPI_INT,
                 0, MPI_COMM_WORLD);
  
  
                                  // Get a list of nodes from all ranks.
     MPI_Get_processor_name(proc_name,&name_len);
     MPI_Allreduce(&name_len, &max_name_len, 1,MPI_INT, MPI_MAX, MPI_COMM_WORLD);
     all_names = malloc(sizeof(int*)*nranks*(max_name_len+1));
     MPI_Gather( proc_name, max_name_len+1 , MPI_CHAR,
                 all_names, max_name_len+1, MPI_CHAR,
                 0, MPI_COMM_WORLD);
  
                                  // If multiple nodes, make muti_node not equal to 0.
     if(rank == 0)
        for(id=0;id<nranks;id++){
           if( strcmp(&all_names[id*(max_name_len+1)],&all_names[0]) ) multi_node++;
        }

   }  // End of Pure MPI part


   if(in_mpi != 0 && in_omp != 0){

 
       if(all_masks == 0) {
          printf("ERROR: ***** You must call hybrid_report_mask() in a Pure MPI region first. ***** \n");
          exit(1);
       }
   
        thrd  =  omp_get_thread_num();
       nthrds =  omp_get_num_threads();
       ncpus  =  (int) sysconf(_SC_NPROCESSORS_ONLN);
    
    
       #pragma omp single
       {
          omp_proc_mask =                           malloc(sizeof(int*)*nthrds);
          for(i=0;i<nthrds;i++) omp_proc_mask[i] =  malloc(sizeof(int)*ncpus  );
          for(i=0;i<nthrds;i++) for(j=0;j<ncpus;j++) omp_proc_mask[i][j] =0;
       }
    
       #pragma omp critical
       ierr = boundto(&nel_set,omp_proc_mask[thrd]);
 
       #pragma omp barrier
       MPI_Comm_size(MPI_COMM_WORLD, &nranks);
       MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 
       #pragma omp master
       {
 
          omp_mask_pac = (int *) malloc(sizeof(int)*nranks*ncpus);  // need packing space for mpi send/recv
 
          if(rank == 0){
 
               print_mask(1,  dummy,  multi_node,     0, 0,   ncpus, nthrds,nranks, omp_proc_mask[0]);  //print header 
               fflush(stdout);
   
               for(tid=0;tid<nthrds;tid++){
                  print_mask(0,  &all_names[tid*(max_name_len+1)], multi_node,  0,tid,   ncpus, nthrds,nranks, omp_proc_mask[tid]);
               }
               fflush(stdout);
 
            for(rid=1;rid<nranks;rid++){
                                            // Receive other rank's packed mask arrays
               MPI_Recv(omp_mask_pac, nthrds*ncpus, MPI_INT, rid, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
 
               for(tid=0;tid<nthrds;tid++){
                  print_mask(0,  &all_names[rid*(max_name_len+1)], multi_node,  rid,tid,   ncpus, nthrds,nranks, &omp_mask_pac[tid*ncpus]);
               }
               fflush(stdout);
 
            } // rank loop
          }   // end root printing
 
          else{  //all other ranks
                                            // All non-root ranks send to root. 
 
            for(rid=1;rid<nranks;rid++){
                                            // Pack up the ranks' mask arrays (Uh, should have made one array from beginning!) 
               for(   tid=0;tid<nthrds;tid++){
                  for( id=0; id<ncpus;  id++)  omp_mask_pac[(tid*ncpus)+id] = omp_proc_mask[tid][id];
               }
                                            // Send to root
               MPI_Send(omp_mask_pac, nthrds*ncpus, MPI_INT, 0, 99, MPI_COMM_WORLD);
                    
            }  //all other ranks
 
 
          } // end non-root printing

          MPI_Barrier(MPI_COMM_WORLD);
 
 
       } // end of Master
       #pragma omp barrier
 
   } // end of OpenMP  part
}
  
