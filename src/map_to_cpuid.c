#include <sched.h>
#include <unistd.h>

int map_to_cpuid( int cpuid){
    cpu_set_t        mask;
    CPU_ZERO(       &mask);
    CPU_SET(cpuid,  &mask);
    return( sched_setaffinity( (pid_t) 0 , sizeof(mask), &mask ) );
}
