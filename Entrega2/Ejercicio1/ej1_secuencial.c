/* Para compilar:
gcc –o salidaEjecutable archivoFuente.c*/
#include <stdio.h>
#include <stdlib.h>


/* Time in seconds from some point in the past */
double dwalltime();


int main(int argc,char*argv[]){
  double *A,*B,*C,*D, *L, *M, *U, *aux1, *aux2, *aux3, temp;
  int i, j, k, N;
  int check=1;
  float promL, promU, promLU, divide;
  unsigned long totalL;
  double timetick;

 //Controla los argumentos al programa
  if (argc < 2){
    printf("\n Faltan argumentos:: N dimension de la matriz\n");
  return 0;
  }

  N=atoi(argv[1]);
  divide=0;

 //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);
  D=(double*)malloc(sizeof(double)*N*N);
  L=(double*)malloc(sizeof(double)*N*N);
  M=(double*)malloc(sizeof(double)*N*N);
  U=(double*)malloc(sizeof(double)*(N/2)*(N+1)); //se alocal sólo los 1
  aux1=(double*)malloc(sizeof(double)*N*N);
  aux2=(double*)malloc(sizeof(double)*N*N);
  aux3=(double*)malloc(sizeof(double)*N*N);

//Inicializa las matrices A, B,C, D  en 1
//Inicializa la matriz L con unos en el triangulo inferior y ceros en el triangulo superior.
//Inicializa la matriz U con unos en el triangulo superior y ceros en el triangulo inferior.
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      A[i*N+j]=1;
      B[i+j*N]=1;
      C[i+j*N]=1;
      D[i*N+j]=1;
      M[i*N+j]=0;
      if(j>=i)
        U[i+j*(j+1)/2]=1; //no se almacenan los ceros
      if(i>=j){
        L[i*N+j]=1;
      }else{
        L[i*N+j]=0;
       }
    }
  }




//Inicializo promedios de las matrices U y L
  promL = 0;
  promU = 0;

  timetick = dwalltime();

  //Suma el total de la matriz triangular U
  for(i=0;i<N;i++){
    for(j=i;j<N; j++){
      promU = promU + U[i+j*(j+1)/2]; //suma todos
    }
  }
  //Suma el total de la matriz inferior L
  for(i=0;i<N;i++){
    for(j=0;j<N; j++){
      totalL = totalL + L[i*N+j]; //suma sólo la parte que le toca
    }
  }
  //Multiplica D*U
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        aux3[i*N+j]=0;
        for(k=0; k<=j; k++){
            aux3[i*N+j]=aux3[i*N+j]+D[i*N+k]*U[k+j*(j+1)/2];
        }
    }
  }
  //Multiplica L*C
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        aux2[i*N+j]=0;
        for(k=i; k<N; k++){
          aux2[i*N+j]=aux2[i*N+j]+L[i*N+k]*C[k+N*j];
        }

    }
  }
  //Multiplica A*B
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        aux1[i*N+j]=0;
        for(k=0; k<N; k++){
          aux1[i*N+j]=aux1[i*N+j]+A[i*N+k]*B[k+N*j];
        }

    }
  }


//Calculo los promedios
  divide = 1.0/(N*N);

  promL = promL*divide;
  promU = promU*divide;
  promLU = promU*promL;


//Sumo los 3 valores
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      M[i*N+j]=aux1[i*N+j]+aux2[i*N+j]+aux3[i*N+j];
    }
  }
//Multiplico por el promedio calculado
  for(i=0;i<N;i++){
     for(j=0;j<N;j++){
       M[i*N+j]=M[i*N+j]*promLU;
    }
  }

  printf("Tiempo en segundos %f \n", dwalltime() - timetick);



  free(A);
  free(B);
  free(C);
  free(D);
  free(L);
  free(U);
  free(aux1);
  free(aux2);
  free(aux3);

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
