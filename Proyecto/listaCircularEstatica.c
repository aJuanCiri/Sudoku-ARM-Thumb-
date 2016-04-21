/*********************************************************************************************
* Fichero:	listaCircularEstatica.c
* Autores:	Alejandro Guiu Perez - 680669 y Alvaro Juan Ciriaco - 682531
* Descrip:	lista circular estatica para la pila
* Version:
*********************************************************************************************/
#include "44blib.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

//Creacion del tipo de dato tipoNodo (representa el elemento a apilar)
typedef struct{
   int ID_evento;				// Identificador del evento.
   int auxData;					// Datos auxiliares
   uint32_t tiempo;				// Momento de el que tuvo lugar la interrupciÃ³n
} tipoNodo;

tipoNodo* lista = (tipoNodo *) 0xc7fedfc;		// Puntero a la posicion inicial de la lista. (260 bytes)

void anadirElemento(int identificador, int dato, int tiempo);


// Indice que marca el ultimo elemento de la pila
static int indice = 0;

// Operacion que se encarga de añadir un nuevo elemento a la lista
void anadirElemento(int identificador, int dato, int tiempo){
	lista[indice].ID_evento = identificador;
	lista[indice].auxData = dato;
	lista[indice].tiempo = tiempo;
	indice++;
	
	//Resetea el indice de la lista si esta llega al tope
	if((int)&lista[indice]>0xc7fef00){
		indice = 0;
	}
}
