/*********************************************************************************************
* Fichero:	gestionExcepciones.c
* Autores:	Alejandro Guiu Perez - 680669 y Alvaro Juan Ciriaco - 682531
* Descrip:	gestiona todas las excepciones de la placa
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "44blib.h"
#include "44b.h"
#include "stdio.h"
#include <inttypes.h>

/*--- variables globales ---*/
int NUM_ALERTAS = 20;

/*--- funciones externas ---*/
extern void D8Led_symbol(int value);
extern void push_debug(int ID_evento, int auxData);

/*--- declaracion de funciones ---*/
void avisoExcepcion(int tipo);
void init_excep();
void gestionGlobal(void) __attribute__((interrupt("ABORT")));
void gestionGlobal(void) __attribute__((interrupt("UNDEF")));
void gestionGlobal(void) __attribute__((interrupt("ABORT")));

/*--- codigo de funciones ---*/
/**
 * Funcion encargada de gestionar de forma global
 * las distintas excepciones que ocurran. Ademas
 * se encarga de diferenciar de que tipo es.
 */
void gestionGlobal(void){
	uint32_t direccion;
	uint32_t tipo;
	asm ("mov %0, lr;" : "=r" (direccion));
	asm ("mrs %0, cpsr;" : "=r" (tipo));   //tipo = cpsr
	switch(tipo&0x1F){  //compara con el modo (con los ultimos 5 bits)
		case 0x1B:  //undefined
			push_debug(1,direccion);
			avisoExcepcion(1);
			break;
		case 0x17:  //abort --> en data lr = lr+8, en prefetch lr = lr+4
			push_debug(2,direccion);
			avisoExcepcion(2);
			break;
	}

}

/**
 * Funcion encargada de mostrar por D8Led el numero
 * que corresponda a la excepcion que se ha gestionado.
 * El programa finaliza por completo.
 */
void avisoExcepcion(int tipo){
	int paridad = 0;				//Cuando sea par mostrara el dato, impar se apagara
	while(1){
		if(paridad%2==0){			//Par = muestra codigo de error
			D8Led_symbol(tipo);
		}else{						//Impar = se apaga para parpadear
			D8Led_symbol(16);
		}
		paridad++;
		Delay(5000);
	}
}

void init_excep() {
	pISR_DABORT = (unsigned) gestionGlobal;
	pISR_UNDEF = (unsigned) gestionGlobal;
	pISR_PABORT = (unsigned) gestionGlobal;
}

