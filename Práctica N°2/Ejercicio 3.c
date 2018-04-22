# include<stdio.h>
# include<stdlib.h>
# include<pthread.h>

#define N 1000000
int cantHilos;
int arreglo[N];
int MAX = -1;
int MIN = 1500;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void *buscar ( void *arg ) {
  int miID = *(int *) arg;
  int max = -1, min = 1500;
  int desde, hasta;
  desde = (N/cantHilos)*miID;
  hasta = desde - 1 + N/cantHilos;
  /* Cambiando el máximo del for se puede apreciar como cada hilo procesa
     su parte del arreglo */
  for(int i = desde; i < hasta; i++){
    if(arreglo[i] > max)
      max = arreglo[i];
    if (arreglo[i] < min)
      min = arreglo[i];
  }
  printf("Mi máximo %d , mi minímo %d\n",max, min );
  pthread_mutex_lock(&mut);
  printf("Tomo el mutex\n");
  if(MAX < max)
    MAX = max;
  if (MIN > min)
    MIN = min;
  pthread_mutex_unlock(&mut);
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
  for (i = 0; i < cantHilos; i++) {
    pthread_Ids[i] = i;
    pthread_create(&misHilos[i], NULL, &buscar, (void *)&pthread_Ids[i]);
  }

  for (i = 0; i < cantHilos; i++) {
    pthread_join(misHilos[i],NULL);
  }
  pthread_mutex_destroy(&mut);

  printf( "\nEl máximo del vector es %d, mientras que el mínimo es %d\n", MAX, MIN);
}
