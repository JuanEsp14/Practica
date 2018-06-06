#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <ej2_omp_source.h>
#include <time.h>


/* Time in seconds from some point in the past */
double dwalltime();

void master(int N, int cantProcesos);
void procesos(int N, int cantProcesos);


int main(int argc,char*argv[]){

    int i, id, cantProcesos, N;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &cantProcesos);

    //Controla los argumentos al programa
    if (argc < 2){
      printf("\n Falta un argumento:: N que indica el tamaño de la matriz \n");
      return 0;
    }

    N=atoi(argv[1]);

    if(id==0){
      master(N, cantProcesos);
    }
    else{
      proceso(N, cantProcesos);
    }

    MPI_Finalize();
    return 0;
}

void master(int N, int cantProcesos){
  printf("MASTER: Llamo a los procesos de omp\n");
  procesamientoHilo(0);
  printf("MASTER: Finalizo\n");
}

void proceso(int N, int cantProcesos){
  int id;
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  printf("Proceso %d: Llamo a los procesos de omp\n", id);
  procesamientoHilo(id);
  printf("Proceso %d: Finalizo\n", id);
}

/****************************************************************************/

#include <sys/time.h>

double dwalltime()
{
  double sec;
  struct timeval tv;

  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}
