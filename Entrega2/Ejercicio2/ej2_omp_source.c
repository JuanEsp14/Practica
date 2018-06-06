#include<omp.h>
#include<stdio.h>

void omp_function(int rank){
  #pragma omp parallel
  {
    printf("Proceso de MPI rank %d thread %d\n", rank, omp_get_thread_num());
  }
}
