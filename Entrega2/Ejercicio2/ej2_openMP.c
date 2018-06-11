#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <time.h>
#include "ej2_omp_source.h"


/* Time in seconds from some point in the past */
double dwalltime();

void master(int N, int cantProcesos);
void procesos(int N, int cantProcesos);


int main(int argc,char*argv[]){

  int i, id, cantProcesos, N, cantHilos;
  double time;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &cantProcesos);

  //Controla los argumentos al programa
  if (argc < 3){
    printf("\n Debe ingresar N y la cantidad de hilos \n");
    return 0;
  }

  N=atoi(argv[1]);
  cantHilos=atoi(argv[2]);
  omp_set_num_threads(cantHilos);

  if(id==0){
    time = dwalltime();
    master(N, cantProcesos);
    printf("Tiempo Master %d: %f \n", id, dwalltime() - time);
  }
  else{
    time = dwalltime();
    procesos(N, cantProcesos);
    printf("Tiempo proceso %d: %f \n", id, dwalltime() - time);
  }

  MPI_Finalize();
  return 0;
}

void master(int N, int cantProcesos){
  double *A,*B,*C,*D, *L, *M, *U, *A_aux, *D_aux, *L_aux, *M_aux, *aux1, *aux2, *aux3;
  int i, j, k;
  int check=1;
  float promL, promU, promLU, divide, distribuido;
  unsigned long parcialL, totalL;
  double timetick ,timeComunic, time;

  //Inicializo promedios de las matrices U y L
  promU = 0;
  parcialL=0;
  divide=0;
  distribuido = N/cantProcesos;

  //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);
  D=(double*)malloc(sizeof(double)*N*N);
  L=(double*)malloc(sizeof(double)*N*N);
  M=(double*)malloc(sizeof(double)*N*N);
  U=(double*)malloc(sizeof(double)*(N/2)*(N+1)); //se alocal sólo los 1
  A_aux=(double*)malloc(sizeof(double)*(distribuido)*N);
  D_aux=(double*)malloc(sizeof(double)*(distribuido)*N);
  L_aux=(double*)malloc(sizeof(double)*(distribuido)*N);
  M_aux=(double*)malloc(sizeof(double)*(distribuido)*N);
  aux1=(double*)malloc(sizeof(double)*(distribuido)*N);
  aux2=(double*)malloc(sizeof(double)*(distribuido)*N);
  aux3=(double*)malloc(sizeof(double)*(distribuido)*N);

  //Inicializa las matrices A, B,C, D  en 1
  //Inicializa la matriz L con unos en el triangulo inferior y ceros en el triangulo superior.
  //Inicializa la matriz U con unos en el triangulo superior y ceros en el triangulo inferior.
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      A[i*N+j]=1;
      B[i+j*N]=1;
      C[i+j*N]=1;
      D[i*N+j]=1;
      M[i*N+j]=0;
      if(j>=i)
      U[i+j*(j+1)/2]=1; //no se almacenan los ceros
      if(i>=j){
        L[i*N+j]=1;
      }else{
        L[i*N+j]=0;
      }
    }
  }

  timetick = dwalltime();
  time = dwalltime();

  MPI_Scatter(A, (distribuido)*N, MPI_DOUBLE, A_aux, (distribuido)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(D, (distribuido)*N, MPI_DOUBLE, D_aux, (distribuido)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(L, (distribuido)*N, MPI_DOUBLE, L_aux, (distribuido)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(M, (distribuido)*N, MPI_DOUBLE, M_aux, (distribuido)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(B,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
  MPI_Bcast(C,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
  MPI_Bcast(U,(N/2)*(N+1), MPI_DOUBLE,0,MPI_COMM_WORLD);

  timeComunic = dwalltime() - time;

  multYprom(N, distribuido, promU,parcialL, L, U, A_aux, D_aux, L_aux, aux1, aux2, aux3);

  //Calculo los promedios
  time = dwalltime();
  MPI_Allreduce(&parcialL, &totalL, 1, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
  timeComunic += (dwalltime() -time);
  divide = 1.0/(N*N);
  promL = totalL*divide;
  promU = promU*divide;
  promLU = promU*promL;

  suma(N, distribuido, promLU, M_aux, aux1, aux2, aux3);

  time = dwalltime();
  //Reuno en M la matriz total
  MPI_Gather(M_aux, (distribuido)*N, MPI_DOUBLE, M, (distribuido)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  timeComunic += (dwalltime() -time);
  printf("Tiempo total en segundos %f \n", dwalltime() - timetick);
  printf("Tiempo de las comunicaciones en segundos %f\n", timeComunic);

  free(A);
  free(B);
  free(C);
  free(D);
  free(L);
  free(U);
  free(M);
  free(M_aux);
  free(A_aux);
  free(D_aux);
  free(L_aux);
  free(aux1);
  free(aux2);
  free(aux3);
}

void procesos(int N, int cantProcesos){
  double *A,*B,*C,*D, *L, *M, *U, *A_aux, *D_aux, *L_aux, *M_aux, *aux1, *aux2, *aux3;
  int i, j, k, id, inicio, fin;
  float promL, promU, promLU, divide, distribuido;
  unsigned long totalL, parcialL;
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  //Inicializo promedios de las matrices U y L
  promU = 0;
  parcialL = 0;
  divide = 0;
  distribuido = N/cantProcesos;

  //Aloca memoria para las matrices
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);
  U=(double*)malloc(sizeof(double)*(N/2)*(N+1)); //se alocal sólo los 1
  A_aux=(double*)malloc(sizeof(double)*(distribuido)*N);
  D_aux=(double*)malloc(sizeof(double)*(distribuido)*N);
  L_aux=(double*)malloc(sizeof(double)*(distribuido)*N);
  M_aux=(double*)malloc(sizeof(double)*(distribuido)*N);
  aux1=(double*)malloc(sizeof(double)*(distribuido)*N);
  aux2=(double*)malloc(sizeof(double)*(distribuido)*N);
  aux3=(double*)malloc(sizeof(double)*(distribuido)*N);

  MPI_Scatter(A, (distribuido)*N, MPI_DOUBLE, A_aux, (distribuido)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(D, (distribuido)*N, MPI_DOUBLE, D_aux, (distribuido)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(L, (distribuido)*N, MPI_DOUBLE, L_aux, (distribuido)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(M, (distribuido)*N, MPI_DOUBLE, M_aux, (distribuido)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(B,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
  MPI_Bcast(C,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
  MPI_Bcast(U,(N/2)*(N+1), MPI_DOUBLE,0,MPI_COMM_WORLD);

  inicio = id * distribuido;
  fin = (id + 1) * distribuido;

  multYprom(N, distribuido, promU,parcialL, L, U, A_aux, D_aux, L_aux, aux1, aux2, aux3);

  //Calculo los promedios
  MPI_Allreduce(&parcialL, &totalL, 1, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
  divide = 1.0/(N*N);
  promL = totalL*divide;
  promU = promU*divide;
  promLU = promU*promL;

  suma(N, distribuido, promLU, M_aux, aux1, aux2, aux3);

  //Reuno en M la matriz total
  MPI_Gather(M_aux, (distribuido)*N, MPI_DOUBLE, M, (distribuido)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);


  free(B);
  free(C);
  free(U);
  free(A_aux);
  free(D_aux);
  free(L_aux);
  free(M_aux);
  free(aux1);
  free(aux2);
  free(aux3);

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
