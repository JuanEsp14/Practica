# include<stdio.h>
# include<stdlib.h>
# include<pthread.h>

#define N 10000
int cantHilos;
int arreglo[N];
int arrAux[N];
pthread_barrier_t barrera;

void *ordenar ( void *arg ) {
  int miID = *(int *) arg;
  int aux, k, l, iMin, desde, hasta, merge = 0;
  /* Cambiando el máximo del for se puede apreciar como cada hilo procesa
     su parte del arreglo */
  for(int k = 0; miID < cantHilos; k++){
    desde = (N/cantHilos)*miID;
    hasta = desde + N/cantHilos;
    if (merge == 0){
      for(int i = desde; i < hasta; i++){
        iMin = i;
        for (int j = i+1; j < hasta; j++){
          if(arreglo[j] < arreglo[iMin])
            iMin=j;
        }
        aux = arreglo[i];
        arreglo[i] = arreglo[iMin];
        arreglo[iMin] = aux;
      }
      merge=1;
    }else{
      k = desde;
      l = hasta/2;
      for(int i = desde; i < hasta; i++){
        if(arreglo[k] < arreglo[l]){
          arrAux[i] = arreglo[k];
          k++;
        }else{
          arrAux[i] = arreglo[l];
          l++;
        }
      }
    }
    if(miID == 0)
      cantHilos = cantHilos/2;
    pthread_barrier_wait(&barrera);
    if(miID == 0)
      pthread_barrier_init(&barrera, NULL, cantHilos);
  }
  pthread_exit(NULL);
}


int main(int argc, char*argv[]){
  cantHilos = atol(argv[1]);
  int i;

  /* Inicializo el arreglo con un random */
  for (i = 0; i < N; i++) {
      arreglo[i] = (rand() % 500); //Valor entre 0 y 499
  }

  /* Creo los hilos */
  pthread_t misHilos[cantHilos];
  int pthread_Ids[cantHilos];
  pthread_barrier_init(&barrera, NULL, cantHilos);
  for (i = 0; i < cantHilos; i++) {
    pthread_Ids[i] = i;
    pthread_create(&misHilos[i], NULL, &ordenar, (void *)&pthread_Ids[i]);
  }

  for (i = 0; i < cantHilos; i++) {
    pthread_join(misHilos[i],NULL);
  }


  pthread_barrier_destroy(&barrera);

  for (i = 0; i < N; i++) {
    printf("%d, ", arrAux[i]);
  }
}
