/*Para compilar:
gcc -pthread –o salidaEjecutable archivoFuente*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

double *A,*B,*C,*D,*E, *F, *L, *M, *U, *At, *aux1, *aux2, *aux3, temp;
float promB, promL, promU, promLU, divide;
int tamBloque, cantHilos, N;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrera1;
pthread_barrier_t barrera2;
pthread_barrier_t barrera3;

/* Time in seconds from some point in the past */
double dwalltime();

void* multiplicacion(void *id);


int main(int argc,char*argv[]){
  int i, j;
  int check=1;
  double timetick;

 //Controla los argumentos al programa
  if (argc < 3){
    printf("\n Faltan argumentos:: N dimension de la matriz, T cantidad de threads \n");
  return 0;
  }

  N=atoi(argv[1]);
  cantHilos = atoi(argv[2]);
  pthread_t misHilos[cantHilos];
  int hilos_ID[cantHilos];

 //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);
  D=(double*)malloc(sizeof(double)*N*N);
  E=(double*)malloc(sizeof(double)*N*N);
  F=(double*)malloc(sizeof(double)*N*N);
  L=(double*)malloc(sizeof(double)*N*N);
  M=(double*)malloc(sizeof(double)*N*N);
  U=(double*)malloc(sizeof(double)*N*N);
  At=(double*)malloc(sizeof(double)*N*N);
  aux1=(double*)malloc(sizeof(double)*N*N);
  aux2=(double*)malloc(sizeof(double)*N*N);
  aux3=(double*)malloc(sizeof(double)*N*N);

//Inicializa las matrices A, B,C, D, E y F en 1
//Inicializa la matriz L con unos en el triangulo inferior y ceros en el triangulo superior.
//Inicializa la matriz U con unos en el triangulo superior y ceros en el triangulo inferior.
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
	     A[i*N+j]=1;
	     B[i+j*N]=1;
	     C[i*N+j]=1;
       D[i*N+j]=1;
       E[i*N+j]=1;
       F[i*N+j]=1;
       M[i*N+j]=0;
       if(i>=j){
        U[i+N*j]=1;
       }else{
        U[i+N*j]=0;
       }
       if(i<=j){
        L[i+N*j]=1;
       }else{
        L[i+N*j]=0;
       }
    }
  }

//Calculo transpuesta
 for(i=0;i<N;i++){
   for(j=0;j<N;j++){
		temp = A[i*N+j];
		At[i*N+j]= A[j*N+i];
		At[j*N+i]= temp;
   }
  }

//Inicializo promedios de las matrices B, U y L
  promB = 0;
  promL = 0;
  promU = 0;
  divide = 1.0/(N*N);

  tamBloque = N/cantHilos;

  timetick = dwalltime();

//Inicializo la barrera
  pthread_barrier_init(&barrera1, NULL, cantHilos);
  pthread_barrier_init(&barrera2, NULL, cantHilos);
  pthread_barrier_init(&barrera3, NULL, cantHilos);

//Creo los hilos
  for(i = 0; i < cantHilos; i++){
    hilos_ID[i] = i;
    pthread_create(&misHilos[i], NULL, &multiplicacion, (void *)&hilos_ID[i]);
  }

//Espero a  que todos terminen
  for(i = 0; i < cantHilos; i++){
    pthread_join(misHilos[i], NULL);
  }

  printf("Tiempo en segundos %f \n", dwalltime() - timetick);

  double ver = M[0];
 //Verifica el resultado
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      check = check && (M[i*N+j]==ver);
    }
  }

  if(check){
    printf("Multiplicacion de matriz correcta\n");
  }else{
     printf("Multiplicacion de matriz erroneo\n");
  }

  free(A);
  free(B);
  free(C);
  free(D);
  free(E);
  free(F);
  free(L);
  free(U);
  free(At);
  free(aux1);
  free(aux2);
  free(aux3);
  pthread_mutex_destroy(&mut);
  pthread_mutex_destroy(&mut);
  pthread_barrier_destroy(&barrera1);
  pthread_barrier_destroy(&barrera2);
  pthread_barrier_destroy(&barrera3);

return(0);

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

void *multiplicacion(void *id){

  int miID = *(int *) id;
  int i, j, k;
  int desde, hasta;
  double auxB, auxL, auxU;
  desde = tamBloque*miID;
  hasta = desde + tamBloque;

  auxB = 0;
  auxL = 0;
  auxU = 0;

  //Comienzo la multiplicación aux1 = AA, aux2=BE y aux3=DF
  for(i=desde;i<hasta;i++){
   for(j=0;j<N;j++){
    auxB = auxB + B[i*N+j];
    /*---ANALIZAR SI LOS IF PARA MATRICES TRIANGULARES MEJORAN---*/
    if(i >= j)
      auxL = auxL + L[i*N+j];
    if(j >= i)
      auxU = auxU + U[i*N+j];
    aux1[i*N+j]=0;
    aux2[i*N+j]=0;
    aux3[i*N+j]=0;
    for(k= 0; k < N; k++){
      aux1[i*N+j]=aux1[i*N+j]+A[i*N+k]*At[k+N*j];
      aux2[i*N+j]=aux2[i*N+j]+B[i*N+k]*E[k+N*j];
      aux3[i*N+j]=aux3[i*N+j]+D[i*N+k]*F[k+N*j];
    }
   }
  }
  pthread_mutex_lock (&mut);
  promB = promB + auxB;
  promU = promU + auxU;
  promL = promL + auxL;
  pthread_mutex_unlock (&mut);
  pthread_barrier_wait(&barrera1);

  //Multiplico la primer parte de las no triangulares A=aux1C
  for(i=desde;i<hasta;i++){
   for(j=0;j<N;j++){
     A[i*N+j]=0;
    for(k= 0; k < N; k++)
      A[i*N+j]=A[i*N+j]+aux1[i*N+k]*C[k+N*j];
   }
  }

  //Multiplico las matrices triangulares B=aux2L D=aux3U
  for(i=desde;i<hasta;i++){
   for(j=0;j<N;j++){
     B[i*N+j]=0;
     D[i*N+j]=0;
     for(k = 0; k <= j; k++)
       B[i*N+j]=B[i*N+j]+aux2[i*N+k]*L[k+N*j];
     for(k = j; k < N; k++)
       D[i*N+j]=D[i*N+j]+aux3[i*N+k]*U[k+N*j];
   }
  }

  //Calculo los promedios
  if (miID == 0){
    promB = promB*divide;
    promL = promL*divide;
    promU = promU*divide;
    promLU = promU*promL;
  }
  pthread_barrier_wait(&barrera2);

  //Multiplicos los promedios a cada valor
  //aux1=A.promLU
  //aux2=B.promB
  //aux3=D.promB
  for(i=desde;i<hasta;i++){
    for(j=0;j<N;j++){
      aux1[i*N+j]=A[i*N+j]*promLU;
      aux2[i*N+j]=B[i*N+j]*promB;
      aux3[i*N+j]=D[i*N+j]*promB;
    }
  }
  pthread_barrier_wait(&barrera3);

  //Sumo los 3 valores
  for(i=desde;i<hasta;i++){
    for(j=0;j<N;j++)
      M[i*N+j]=aux1[i*N+j]+aux2[i*N+j]+aux3[i*N+j];
  }
  pthread_exit(NULL);
}
