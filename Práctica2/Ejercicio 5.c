# include<stdio.h>
# include<stdlib.h>
# include<pthread.h>

#define N 10000
int cantHilos;
int arreglo[N];
int SUM = 0;
int PROM = 0;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrera;

void *promedio ( void *arg ) {
  int miID = *(int *) arg;
  int desde, hasta, sum = 0;
  float miProm = 0;
  desde = (N/cantHilos)*miID;
  hasta = desde - 1 + N/cantHilos;
  /* Cambiando el máximo del for se puede apreciar como cada hilo procesa
     su parte del arreglo */
  for(int i = desde; i < hasta; i++)
    sum+=arreglo[i];
  miProm = sum/(hasta-desde);
  printf("Hilo %d. Mi promedio es de %f \n", miID, miProm);	
  pthread_mutex_lock(&mut);
  SUM+=sum;
  pthread_mutex_unlock(&mut);
  pthread_barrier_wait(&barrera);
  if(miID == 0){	
    PROM = SUM/N;
    printf( "\nEl promedio del vector es de %d\n", PROM);
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
    pthread_create(&misHilos[i], NULL, &promedio, (void *)&pthread_Ids[i]);
  }

  for (i = 0; i < cantHilos; i++) {
    pthread_join(misHilos[i],NULL);
  }
  pthread_mutex_destroy(&mut);
  pthread_barrier_destroy(&barrera);

}
