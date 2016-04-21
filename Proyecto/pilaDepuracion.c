/*********************************************************************************************
* Fichero:	pilaDepuracion.c
* Autores:	Alejandro Guiu Perez - 680669 y Alvaro Juan Ciriaco - 682531
* Descrip:  implementa la operacion encargada de añadir un nuevo registro de excepcion a la pila
* Version:
*********************************************************************************************/
#include "44blib.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "timer4.h"


/*--- declaracion de funciones ---*/
void push_debug(int ID_evento, int auxData);

// Funcion encargada de apilar un nuevo elemento en la pilaDepuracion
void push_debug(int ID_evento, int auxData){
	uint32_t tiempo = (leerTiempoTimer4());			// Tiempo en el que se ha producido la interrupción
	anadirElemento(ID_evento, auxData, tiempo);
}
