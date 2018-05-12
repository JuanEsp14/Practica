/*Para compilar:
gcc -pthread –o salidaEjecutable archivoFuente*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* Time in seconds from some point in the past */
double dwalltime();
void* multiplicacion(void *id);
int tamanoBloque, N, T;
 double *A,*At,*resultado;
 double temp;


int main(int argc,char*argv[]){

 //Controla los argumentos al programa
  if (argc < 2){
   printf("\n Falta un argumento: N dimension de la matriz \n");
   return 0;
  }

  N=atoi(argv[1]);
  T=atoi(argv[2]);
  int i,j,k;
  int check=1;
  double timetick;
  pthread_t misHilos[T];
  int threads_ids[T];

  //tamaño que le corresponde a cada hilo
  tamanoBloque=N/T;

   A=(double*)malloc(sizeof(double)*N*N);
   At=(double*)malloc(sizeof(double)*N*N);
   resultado=(double*)malloc(sizeof(double)*N*N);

   //Inicializo matriz A
  for(i=0;i<N;i++){
  	for(j=0;j<N;j++){
  		A[i*N+j]=1;
  	}
  }

//traspuesta
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
		temp = A[i*N+j];
		At[i*N+j]= A[j*N+i];
		At[j*N+i]= temp;
   }
  }

  timetick = dwalltime();

  //Creo los hilos
  for(int id=0;id<T;id++){
  	threads_ids[id] = id;
  	pthread_create(&misHilos[id], NULL, &multiplicacion,(void*)&threads_ids[id]);
  }


  //Espera a que finalicen
  for (i=0; i <T; i++)
  		pthread_join(misHilos[i], NULL);



  printf("Tiempo: %f \n", dwalltime() - timetick);


   //Verifica el resultado
    for(i=0;i<N;i++){
     for(j=0;j<N;j++){
  	    check=check&&(resultado[i*N+j]==N);
     }
    }

    if(check){
     printf("Multiplicacion de matrices resultado correcto\n");
    }else{
     printf("Multiplicacion de matrices resultado erroneo\n");
    }

   free(A);
   free(resultado);
   free(At);

   return(0);
}

void* multiplicacion(void *id){
	int miID = *(int *) id;
	int i, j, k;
	int inicio=miID*tamanoBloque;
	int fin=inicio + tamanoBloque;
    for(i = inicio; i < fin; i++){
        for(j= 0; j < N; j++){
            resultado[i*N+j]=0;
            for(k= 0; k < N; k++){
                resultado[i*N+j]+= A[i*N+k]*At[k+N*j];
            }
        }
    }
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
