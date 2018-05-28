#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "mpi.h"

#define SIZESTRING 25
/* Time in seconds from some point in the past*/
double dwalltime();

typedef struct{
  char palabra[SIZESTRING];
  int contador;
}Lista;

void master(int N, int cantProcesos);
void par(int N, int cantProcesos);
void impar(int N, int cantProcesos);
void final(int N, int cantProcesos);
Lista *AddToList(Lista *pLista, int *posListaPalabras,int largoPalabra, char *palabra);


int main(int argc, char **argv){

  int N, i, j, k, id, cantProcesos;
	N= atoi(argv[1]);
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &cantProcesos);

	if(id==0){
		master(N, cantProcesos);
  }
	else{
    if ((id%2 == 0) && id < cantProcesos)
      par(N, cantProcesos);
    else{
      if (id < cantProcesos) {
        impar(N, cantProcesos);
      }else
        final(N, cantProcesos);
    }
  }

  MPI_Finalize();
  return 0;
}

/*****************************************************************/

//Funcion encargada de almacenar la estructura en la lista
Lista *AddToList(Lista *pLista, int *posListaPalabras,int largoPalabra, char *palabra){
  int encontrada=0;

  for(int j = 0; j < *posListaPalabras; j++){
    if(strcmp(pLista[j].palabra, palabra) == 0){
      pLista[j].contador++;
      encontrada=1;
    }
  }
  if(encontrada == 0){
    if(pLista == NULL)
      pLista = malloc(sizeof(Lista));
    else
      pLista = realloc(pLista,sizeof(Lista)*(*posListaPalabras+1));

    //Inicializo el contenido del vector
    memset(pLista[*posListaPalabras].palabra,0,largoPalabra);
    pLista[*posListaPalabras].contador = 1;
    strncpy(pLista[*posListaPalabras].palabra, palabra ,largoPalabra);
    *posListaPalabras += 1;
   }
  return pLista;
}

void master(int N , int cantProcesos) {
  char oracion[100], palabra[SIZESTRING], oracion_aux[100];
  int largoPalabra=0, i, posListaPalabras=0;
  double timetick;

  //Creamos lista para las palabras
  Lista *pListaPalabras = NULL;
  //Inicializo la palabra con ceros
  memset(palabra,0,SIZESTRING);

  printf("Ingrese una oracion: ");
  fgets(oracion, 100, stdin);

  timetick = dwalltime();

  MPI_Scatter(oracion, (strlen(oracion)/cantProcesos), MPI_CHAR, oracion_aux, (strlen(oracion)/cantProcesos), MPI_CHAR, 0, MPI_COMM_WORLD);
  /*for(i = 0; i < strlen(oracion); i++){
  //Verifico si se cambió de palabra o si se está por terminar el texto para
  //elminar el \0
    if(oracion[i] != ' ' && i < strlen(oracion)-1){
      palabra[largoPalabra] = oracion[i];
      largoPalabra++;
    }else{
      pListaPalabras = AddToList(pListaPalabras, &posListaPalabras, largoPalabra,palabra);
      largoPalabra = 0;
  	  memset(palabra,0,SIZESTRING);
    }
  }

  //Solo MASTER
  for(i=0; i < posListaPalabras; i++){
    printf("%s: %d veces\n", pListaPalabras[i].palabra, pListaPalabras[i].contador);
  }*/

  printf("MASTER: %s\n", oracion_aux);
  printf("Tiempo en segundos %f \n", dwalltime() - timetick);

  free(pListaPalabras);
}

void par(int N , int cantProcesos) {
  char oracion_aux[100];
  int cant, id;
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Bcast(oracion_aux, cant, MPI_CHAR,0,MPI_COMM_WORLD);
  printf("PAR %d: %s\n", id, oracion_aux);
}

void impar(int N , int cantProcesos) {
  char oracion_aux[100];
  int cant, id;
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Bcast(oracion_aux, cant, MPI_CHAR,0,MPI_COMM_WORLD);
  printf("IMPAR %d: %s\n", id, oracion_aux);
}

void final(int N , int cantProcesos) {
  char oracion_aux[100];
  int cant, id;
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Bcast(oracion_aux, cant, MPI_CHAR,0,MPI_COMM_WORLD);
  printf("FINAL %d: %s\n", id, oracion_aux);
}

/*****************************************************************/

#include <sys/time.h>

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}
