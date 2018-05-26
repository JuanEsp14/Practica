# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
#include <time.h>


int main(int argc, char*argv[]){
  int N = atol(argv[1]);
  int i;
  int cantHilos;
  int *arreglo=(int*) malloc(sizeof(int)*N);
  int *arrAux=(int*) malloc(sizeof(int)*N);



  int aux, k, l, iMin, merge = 0;
  srand(time(NULL));

  /* Inicializo el arreglo con un random */
  for (i = 0; i < N; i++) {
      arreglo[i] = (rand() % 500); //Valor entre 0 y 499
      printf("%d\n", arreglo[i]);
  }

  for(int k = 0; k < N; k++){

    if (merge == 0){
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
      merge=1;
    }else{
      k=0;
      l=N/2;
      for(int i = 0; i < N; i++){
        if(arreglo[k] < arreglo[l]){
          arrAux[i] = arreglo[k];
          k++;
        }else{
          arrAux[i] = arreglo[l];
          l++;
        }
      }
    }
  }
  for (i = 0; i < N; i++) {
    printf("%d, ", arrAux[i]);
  }

  free(arreglo);
  free(arrAux);
  return 0;
}