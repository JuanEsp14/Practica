# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
#include <time.h>

double dwalltime();

int main(int argc, char*argv[]){
  int N = atol(argv[1]);
  int i, aux, k, l, iMin, merge = 0;
  int cantHilos;
  int *arreglo=(int*) malloc(sizeof(int)*N);
  int *arrAux=(int*) malloc(sizeof(int)*N);

  /* Inicializo el arreglo con un random */
  for (i = 0; i < N; i++) {
      arreglo[i] = (rand() % 500); //Valor entre 0 y 499
  }

  for(int k = 0; k < N; k++){
      for(int i = 0; i < N; i++){
        iMin = i;
        for (int j = i+1; j < N; j++){
          if(arreglo[j] < arreglo[iMin])
            iMin=j;
        }
        aux = arreglo[i];
        arreglo[i] = arreglo[iMin];
        arreglo[iMin] = aux;
      }
    }

  for (i = 0; i < N; i++) {
    printf("%d, ", arreglo[i]);
  }

  printf("\n");

  free(arreglo);
  free(arrAux);
  return 0;
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
