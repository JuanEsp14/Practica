#include<omp.h>
#include<stdio.h>

void multYprom(int N, float distribuido, float *promU,
              float *parcialL, double * L, double *U,
              double *A_aux, double * D_aux, double * L_aux,
              double * aux1, double * aux2, double *aux3){
  int i, j, k;
  #pragma omp parallel
  {
    //Multiplica D_aux*U
    #pragma omp for private(i,j,k) collapse(2)
    for(i=0;i<distribuido;i++){
      for(j=0;j<N;j++){
        aux3[i*N+j]=0;
        for(k=0; k<=j; k++){
          aux3[i*N+j]=aux3[i*N+j]+D_aux[i*N+k]*U[k+j*(j+1)/2];
        }
      }
    }

    //Multiplica L_aux*C
    #pragma omp for private(i,j,k) collapse(2)
    for(i=0;i<distribuido;i++){
      for(j=0;j<N;j++){
        aux2[i*N+j]=0;
        for(k=i; k<N; k++){
          aux2[i*N+j]=aux2[i*N+j]+L_aux[i*N+k]*C[k+N*j];
        }

      }
    }

    //Multiplica A_aux*B
    #pragma omp for private(i,j,k) collapse(2)
    for(i=0;i<distribuido;i++){
      for(j=0;j<N;j++){
        aux1[i*N+j]=0;
        for(k=0; k<N; k++){
          aux1[i*N+j]=aux1[i*N+j]+A_aux[i*N+k]*B[k+N*j];
        }

      }
    }

    //Suma el total de la matriz triangular U
    #pragma omp for reduction(+:promU) private(i,j) collapse(2)
    for(i=0;i<N;i++){
      for(j=i;j<N; j++){
        promU = promU + U[i+j*(j+1)/2]; //suma todos
      }
    }

    //Suma el total de la matriz inferior L
    #pragma omp for reduction(+:parcialL) private(i,j) collapse(2)
    for(i=0;i<distribuido;i++){
      for(j=0;j<N; j++){
        parcialL = parcialL + L_aux[i*N+j]; //suma sólo la parte que le toca
      }
    }
  }
}

void suma(int N, float distribuido, float promLU, double * M_aux,
              double * aux1, double * aux2, double *aux3){
  int i, j;
  #pragma omp parallel
  {
    //Sumo los 3 valores
    #pragma omp for private(i,j) collapse(2)
    for(i=0;i<distribuido;i++){
      for(j=0;j<N;j++){
        M_aux[i*N+j]=aux1[i*N+j]+aux2[i*N+j]+aux3[i*N+j];
      }
    }

    //Multiplico por el producto de promedios LU
    #pragma omp for private(i,j) collapse(2)
    for(i=0;i<distribuido;i++){
      for(j=0;j<N;j++){
        M_aux[i*N+j]=M_aux[i*N+j]*promLU;
      }
    }
  }
}
