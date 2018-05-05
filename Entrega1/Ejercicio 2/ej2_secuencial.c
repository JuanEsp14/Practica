#include<stdio.h>
#include<stdlib.h>


/* Time in seconds from some point in the past */
double dwalltime();


int main(int argc,char*argv[]){
  double *A,*B,*C,*D,*E, *F, *L, *M, *U, *aux1, *aux2, *aux3;
  int i, j, k, N;
  int check=1;
  float promB, promL, promU, promLU, divide;
  double timetick;

 //Controla los argumentos al programa
  if (argc < 3){
    printf("\n Faltan argumentos:: N dimension de la matriz, T cantidad de threads \n");
  return 0;
  }

  N=atoi(argv[1]);
  int numThreads = atoi(argv[2]);
  //omp_set_num_threads(numThreads);
  divide=0;

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

//Inicializo promedios de las matrices B, U y L
  promB = 0;
  promL = 0;
  promU = 0;

//Comienzo la multiplicación aux1 = AA, aux2=BE y aux3=DF
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
    promB = promB + B[i*N+j];
    promL = promL + L[i*N+j];
    promU = promU + U[i*N+j];
    aux1[i*N+j]=0;
    aux2[i*N+j]=0;
    aux2[i*N+j]=0;
    for(k= 0; k < N; k++){
      aux1[i*N+j]=aux1[i*N+j]+A[i*N+k]*A[k+N*j];
      aux2[i*N+j]=aux2[i*N+j]+B[i*N+k]*E[k+N*j];
      aux3[i*N+j]=aux3[i*N+j]+D[i*N+k]*F[k+N*j];
    }
   }
  }

//Multiplico la primer parte de las no triangulares A=aux1C
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
     A[i*N+j]=0;
    for(k= 0; k < N; k++)
      A[i*N+j]=A[i*N+j]+aux1[i*N+k]*C[k+N*j];
   }
  }



//Multiplico las matrices triangulares B=aux2L D=aux3U
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
     B[i*N+j]=0;
     D[i*N+j]=0;
     for(k = 0; k < N; k++){
       B[i*N+j]=B[i*N+j]+aux2[i*N+k]*L[k+N*j];
       D[i*N+j]=D[i*N+j]+aux3[i*N+k]*U[k+N*j];

     }
   }
  }

  divide = 1/(N*N);
  promB = promB*divide;
  promL = promL*divide;
  promU = promU*divide;
  promLU = promU*promL;
  printf("%f\n", divide );

  printf("%f\n", promB );
  printf("%f\n", promL );
  printf("%f\n", promU );
  printf("%f\n", promLU );

  //Verifica el resultado
  for(i=0;i<N;i++){
   for(j=0;j<N;j++)
      printf("%f",D[i+j*N]);
   printf("\n" );
  }

//Multiplicos los promedios a cada valor
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      aux1[i*N+j]=aux1[i*N+j]*promLU;
      aux2[i*N+j]=aux2[i*N+j]*promB;
      aux3[i*N+j]=aux3[i*N+j]*promB;
    }
  }

//Sumo los 3 valores
  for(i=0;i<N;i++){
    for(j=0;j<N;j++)
      M[i*N+j]=aux1[i*N+j]+aux2[i*N+j]+aux3[i*N+j];

  }

 //Verifica el resultado
 for(i=0;i<N;i++){
  for(j=0;j<N;j++){
   check = check && (M[i*N+j]==N);
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

return(0);

}
