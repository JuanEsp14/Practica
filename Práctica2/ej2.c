#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
/*
N: tamaño del vector
X: ocurrencia a encontrar

ejecutar:
./ej2 N X
*/


/* Time in seconds from some point in the past */
double dwalltime();
void* multiplicacion(void *id);
int *vector;
int ocurrencias=0;
pthread_mutex_t mutexOcurrencias=PTHREAD_MUTEX_INITIALIZER;
int N, X, T, i;
int tamanoBloque;
double timetick;
void* funcionOcurrencias(void *id);

int main(int argc,char*argv[]){

//Controla los argumentos al programa
if (argc < 3){
	printf("\n Faltan argumentos: N dimension del arreglo, X ocurrencia, T hilos \n");
	return 0;
}

N=atoi(argv[1]);
X=atoi(argv[2]);
T=atoi(argv[3]);
tamanoBloque=N/T;
vector=(int*)malloc(sizeof(int)*N);
pthread_t misHilos[T];
int threads_ids[T];
int vectorCantOcurrencias[T];


//inicialización del vector
vector[0]=1;
vector[1]=3;
for(i=2; i<N; i++){
	vector[i]=2;
}

timetick = dwalltime();

//Creo los hilos
for(int id=0;id<T;id++){
	threads_ids[id] = id;
	pthread_create(&misHilos[id], NULL, &funcionOcurrencias,(void*)&threads_ids[id]);
}


//Espera a que finalicen
for (i=0; i <T; i++){
		pthread_join(misHilos[i],  NULL);
}




printf("Tiempo: %f \n", dwalltime() - timetick);
printf("La cantidad de ocurrencias es: %d\n", ocurrencias);

}


void* funcionOcurrencias(void *id){
	int miID = *(int *) id;
	int inicio=miID*tamanoBloque;
	int fin=inicio + tamanoBloque;
	int i, cant=0;

	printf("Hilo: %d\n", miID);

	for(i=inicio; i<fin; i++){
		if(vector[i]==X)
			cant++;
	}
	pthread_mutex_lock (&mutexOcurrencias);
	ocurrencias+=cant;
	pthread_mutex_unlock (&mutexOcurrencias);
    pthread_exit(NULL);


}

#include <sys/time.h>

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}
