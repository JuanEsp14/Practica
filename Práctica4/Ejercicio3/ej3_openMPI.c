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
Lista *compararListas(Lista *pListaPrin, Lista *pListaSec, int *cantPalabraPrin, int *cantPalabraSec);

int main(int argc, char **argv){

  int i, id, cantProcesos, N;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &cantProcesos);

  //Controla los argumentos al programa
  if (argc < 2){
    printf("\n Falta un argumento:: N que indica el tamaño del texto \n");
    return 0;
  }

  N=atoi(argv[1]);

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

Lista *compararListas(Lista *pListaPrin, Lista *pListaSec, int *cantPalabraPrin, int *cantPalabraSec){
  int encontrada = 0;
  for(int i = 0; i < *cantPalabraSec; i++){
    for(int j = 0; j < *cantPalabraSec && encontrada == 0; j++){
      if(strcmp(pListaPrin[j].palabra, pListaSec[i].palabra) == 0){
        pListaPrin[j].contador++;
        encontrada=1;
      }
    }
    //Palabra nueva
    if (encontrada == 0){
      if(pListaPrin == NULL)
        pListaPrin = malloc(sizeof(Lista));
      else
        pListaPrin = realloc(pListaPrin,sizeof(Lista)*(*cantPalabraPrin+1));

      //Inicializo el contenido del vector
      memset(pListaPrin[*cantPalabraPrin].palabra,0,strlen(pListaSec[i].palabra));
      pListaPrin[*cantPalabraPrin].contador = 1;
      strncpy(pListaPrin[*cantPalabraPrin].palabra, pListaSec[i].palabra, strlen(pListaSec[i].palabra));
      *cantPalabraPrin += 1;
    }
  }
  return pListaPrincipal;
}

void master(int N, int cantProcesos) {
  char oracion[N], palabra[SIZESTRING], oracion_aux[N], enviar[N];
  int largoPalabra=0, i, j, posListaPalabras=0;
  double timetick;
  FILE *archivo;
  MPI_Status status;
  //Creamos lista para las palabras
  Lista *pListaPalabras = NULL;

  //Inicializo la palabra con ceros
  memset(palabra,0,SIZESTRING);
  memset(oracion_aux,0,N);
  memset(enviar,0,N);

  archivo = fopen("ejercicio3.txt","r");

  if (archivo == NULL){
    printf("\nError de apertura del archivo. \n\n");
  }else{
    fgets(oracion, N, archivo);
  }
  fclose(archivo);

  timetick = dwalltime();
  printf("Texto a procesar\n %s\n",oracion);
  MPI_Scatter(oracion, (N/cantProcesos), MPI_CHAR, oracion_aux, (N/cantProcesos), MPI_CHAR, 0, MPI_COMM_WORLD);
  //Envia su última parte
  if(oracion_aux[N/cantProcesos] == ' '){
      oracion_aux[0] = ' ';
      largoPalabra = 0;
  }else{
    j = 0;
    for(i = N/cantProcesos; i > 0 && oracion_aux[i] != ' '; i--){
      enviar[j] = oracion_aux[i];
      j++;
    }
  }

  //Proceso de envio
  MPI_Send(enviar,(N/cantProcesos),MPI_CHAR, 1, 1,MPI_COMM_WORLD, &status);

  for(i = 0; i < strlen(oracion_aux); i++){
    //Verifico si se cambió de palabra o si se está por terminar el texto para
    //elminar el \0
    if(oracion_aux[i] != ' ' && i < strlen(oracion_aux)-1){
      palabra[largoPalabra] = oracion_aux[i];
      largoPalabra++;
    }else{
      pListaPalabras = AddToList(pListaPalabras, &posListaPalabras, largoPalabra,palabra);
      largoPalabra = 0;
      memset(palabra,0,SIZESTRING);
    }
  }

  //Recibir las listas
  for(i = 1; i <= cantProcesos; i+=2){
    //Preguntar cómo enviar
    MPI_Recv(recibir,(N/cantProcesos),MPI_CHAR, id, id,MPI_COMM_WORLD, &status);
    MPI_Recv(recibir,(N/cantProcesos),MPI_CHAR, id, id,MPI_COMM_WORLD, &status);
    pListaPalabras = compararListas(pListaPalabras, Lista *pListaSec, &posListaPalabras, int *cantPalabraSec);
    free(listaSec);
  }

  //Solo MASTER
  for(i=0; i < posListaPalabras; i++){
    printf("%s: %d veces\n", pListaPalabras[i].palabra, pListaPalabras[i].contador);
  }

  printf("Tiempo en segundos %f \n", dwalltime() - timetick);

  free(pListaPalabras);
}

void par(int N, int cantProcesos) {
  char oracion_aux[N], *oracion, enviar[N], recibir[N];
  int id, largoPalabra=0, i, j, posListaPalabras=0;
  MPI_Status status;
  //Creamos lista para las palabras
  Lista *pListaPalabras = NULL;

  //Inicializo la palabra con ceros
  memset(palabra,0,SIZESTRING);
  memset(oracion_aux,0,N);
  memset(enviar,0,N);
  memset(recibir,0,N);

  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Scatter(oracion, (N/cantProcesos), MPI_CHAR, oracion_aux, (N/cantProcesos), MPI_CHAR, 0, MPI_COMM_WORLD);

  if(oracion_aux[N/cantProcesos] == ' '){
      oracion_aux[0] = ' ';
      largoPalabra = 0;
  }else{
    j = 0;
    for(i = N/cantProcesos, i > 0 && oracion_aux[i] != ' ', i--){
      enviar[j] = oracion_aux[i];
      j++;
    }
  }

  //Envio al siguiente
  MPI_Send(enviar,(N/cantProcesos),MPI_CHAR, id+1, id+1,MPI_COMM_WORLD, &status);
  //Recibo del anterior
  MPI_Recv(recibir,(N/cantProcesos),MPI_CHAR, id, id,MPI_COMM_WORLD, &status);

  //Proceso lo recibido
  for(i = 0; i < strlen(recibir); i++){
    palabra[strlen(recibir)-i] = oracion_aux[i];
    largoPalabra++;
  }

  for(i = 0; i < strlen(oracion_aux); i++){
    //Verifico si se cambió de palabra o si se está por terminar el texto para
    //elminar el \0
    if(oracion_aux[i] != ' ' && i < strlen(oracion_aux)-1){
      palabra[largoPalabra] = oracion_aux[i];
      largoPalabra++;
    }else{
      pListaPalabras = AddToList(pListaPalabras, &posListaPalabras, largoPalabra,palabra);
      largoPalabra = 0;
      memset(palabra,0,SIZESTRING);
    }
  }

  //Enviar lista
  //Preguntar cómo enviar
  MPI_Send(recibir,(N/cantProcesos),MPI_CHAR, id, id,MPI_COMM_WORLD, &status);
  MPI_Send(recibir,(N/cantProcesos),MPI_CHAR, id, id,MPI_COMM_WORLD, &status);
}

void impar(int N, int cantProcesos) {
  char oracion_aux[N], *oracion, enviar[N], recibir[N];
  int id, largoPalabra=0, i, j, posListaPalabras=0;
  MPI_Status status;
  //Creamos lista para las palabras
  Lista *pListaPalabras = NULL;

  //Inicializo la palabra con ceros
  memset(palabra,0,SIZESTRING);
  memset(oracion_aux,0,N);
  memset(enviar,0,N);
  memset(recibir,0,N);

  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Scatter(oracion, (N/cantProcesos), MPI_CHAR, oracion_aux, (N/cantProcesos), MPI_CHAR, 0, MPI_COMM_WORLD);

  if(oracion_aux[N/cantProcesos] == ' '){
      oracion_aux[0] = ' ';
      largoPalabra = 0;
  }else{
    j = 0;
    for(i = N/cantProcesos, i > 0 && oracion_aux[i] != ' ', i--){
      enviar[j] = oracion_aux[i];
      j++;
    }
  }

  //Recibo del anterior
  MPI_Recv(recibir,(N/cantProcesos),MPI_CHAR, id, id,MPI_COMM_WORLD, &status);
  //Envio al siguiente
  MPI_Send(enviar,(N/cantProcesos),MPI_CHAR, id+1, id+1,MPI_COMM_WORLD, &status);

  //Proceso lo recibido
  for(i = 0; i < strlen(recibir); i++){
    palabra[strlen(recibir)-i] = oracion_aux[i];
    largoPalabra++;
  }

  for(i = 0; i < strlen(oracion_aux); i++){
    //Verifico si se cambió de palabra o si se está por terminar el texto para
    //elminar el \0
    if(oracion_aux[i] != ' ' && i < strlen(oracion_aux)){
      palabra[largoPalabra] = oracion_aux[i];
      largoPalabra++;
    }else{
      pListaPalabras = AddToList(pListaPalabras, &posListaPalabras, largoPalabra,palabra);
      largoPalabra = 0;
      memset(palabra,0,SIZESTRING);
    }
  }

  //Recibir lista
  //Preguntar cómo enviar
  MPI_Recv(recibir,(N/cantProcesos),MPI_CHAR, id, id,MPI_COMM_WORLD, &status);
  MPI_Recv(recibir,(N/cantProcesos),MPI_CHAR, id, id,MPI_COMM_WORLD, &status);
  pListaPalabras = compararListas(pListaPalabras, Lista *pListaSec, &posListaPalabras, int *cantPalabraSec);
  free(listaSec);
  //Enviar lista
  //Preguntar cómo enviar
  MPI_Send(recibir,(N/cantProcesos),MPI_CHAR, id, id,MPI_COMM_WORLD, &status);
  MPI_Send(recibir,(N/cantProcesos),MPI_CHAR, id, id,MPI_COMM_WORLD, &status);
}

void final(int N, int cantProcesos) {
  char oracion_aux[N], *oracion, enviar[N];
  int id, largoPalabra=0, i, j, posListaPalabras=0;
  MPI_Status status;
  //Creamos lista para las palabras
  Lista *pListaPalabras = NULL;

  //Inicializo la palabra con ceros
  memset(palabra,0,SIZESTRING);
  memset(oracion_aux,0,N);
  memset(enviar,0,N);

  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Scatter(oracion, (N/cantProcesos), MPI_CHAR, oracion_aux, (N/cantProcesos), MPI_CHAR, 0, MPI_COMM_WORLD);

  //Recibo del anterior
  MPI_Recv(recibir,(N/cantProcesos),MPI_CHAR, id, id,MPI_COMM_WORLD, &status);

  //Proceso lo recibido
  for(i = 0; i < strlen(recibir); i++){
    palabra[strlen(recibir)-i] = oracion_aux[i];
    largoPalabra++;
  }

  for(i = 0; i < strlen(oracion_aux); i++){
    //Verifico si se cambió de palabra o si se está por terminar el texto para
    //elminar el \0
    if(oracion_aux[i] != ' ' && i < strlen(oracion_aux)-1){
      palabra[largoPalabra] = oracion_aux[i];
      largoPalabra++;
    }else{
      pListaPalabras = AddToList(pListaPalabras, &posListaPalabras, largoPalabra,palabra);
      largoPalabra = 0;
      memset(palabra,0,SIZESTRING);
    }
  }

  //Envio lista
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
