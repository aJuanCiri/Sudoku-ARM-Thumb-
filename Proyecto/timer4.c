/*********************************************************************************************
* Fichero:	timer2.c
* Autores:	Alejandro Guiu Perez - 680669 y Alvaro Juan Ciriaco - 682531
* Descrip:	funciones de control del timer4
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "44blib.h"
#include <inttypes.h>
#include "gestorPantalla.h"

/*--- declaracion de funciones ---*/
void timer4_ISR(void) __attribute__ ((interrupt ("IRQ")));
void timer4_init(void);
void timer4_stop(void);

/*--- declaracion de variables ---*/
volatile int actualizarTiempo = 0;	// Indica si hay que actualizar el tiempo de la pantalla
volatile uint32_t tiempoTotal=0;	// Lleva la cuenta de las veces que ha interrumpido

void timer4_ISR(void)
{
	tiempoTotal++;					// Aumentamos el numero de interrupciones del timer4
	actualizarTiempo = 1;			// Marcamos que hay que actualizar el tiempo dibujado en la pantalla
	rI_ISPC |= BIT_TIMER4; 			// limpiar bit de pendiente del timer4
}

/**
 * Funcion encargada de parar la gestion de interrupciones del timer
 * con la finalidad de dejar el timer bloqueado con la varibale tiempoTotal 
 * sin resetear, para asi poder mostrarla de forma indefinida en la pantalla final del sudoku
 */
void timer4_stop(void){
	rINTMSK |= BIT_TIMER4;		 	// Deshabilitamos interrupciones de timer4
	rTCON &= ~(0x100000);			// Paramos el reloj del tiemer
}

/* Funcion encargada de resetear la cuenta del timer 4 */
void timer4_reset(void) {
	tiempoTotal = 0;
}

void timer4_init(void){

	// Configuracion del timer4
	rINTMSK &= ~(BIT_TIMER4);
	pISR_TIMER4=(unsigned)timer4_ISR;

    rTCFG0 |= ~(0x00FFFF); 			// Marcamos el preescalado a 255
    rTCFG1 &= 0xFFF0FFFF; 			// Marcamos el divisor a 1/4
    rTCFG1 |= 0x10000;
    rTCNTB4 = 64516; 				// Valor inicial de cuenta
    rTCMPB4 = 0;					// Valor del dato a comparar
	rTCON |= 0x200000; 				// Activamos manual update bit.

    rTCON |= 0x900000; 				// Activa auto-reload
	rTCON &= ~(0x200000); 			// Desactiva manual update bit.
}

/* Devuelve el valor de la variable [tiempoTotal] */
uint32_t leerTiempoTimer4() {
	return tiempoTotal;
}

/* Devuelve el valor de la variable [actualizarTiempo] */
int hayQueActualizarTiempo() {
	return actualizarTiempo;
}

/* Desactiva (pone a 0) la variable [actualizarTiempo] */
void desactivarActualizacion() {
	actualizarTiempo = 0;
}
