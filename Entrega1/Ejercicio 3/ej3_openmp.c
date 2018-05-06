#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/* Time in seconds from some point in the past */
double dwalltime();

int main(int argc,char*argv[]){
  int i, N, cantHilos, pares, *vector;
  double timetick;

  if (argc < 3){
    printf("\n Faltan argumentos:: N dimension del vector y cantidad de hilos \n");
    return 0;
  }
  N=atoi(argv[1]);
  cantHilos=atoi(argv[2]);
  vector = (int*)malloc(sizeof(int)*N);
  omp_set_num_threads(cantHilos);
  pares = 0;

//Inicializo vector valores de 1 a 100
  for(i=0; i < N ; i++){
    vector[i] = (rand() % 100) + 1;
  }

  timetick = dwalltime();

//Verifico cuántos números pares existen
  #pragma omp parallel for
  for(i = 0; i < N; i++){
    if((vector[i]&1) == 0)
      pares++;
  }

  printf("Tiempo en segundos %f \n", dwalltime() - timetick);

  printf("Cantidad de números pares: %d\n", pares);

  free(vector);

 return(0);
}


/*****************************************************************/

#include <sys/time.h>

double dwalltime()
{
  double sec;
  struct timeval tv;

  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}
