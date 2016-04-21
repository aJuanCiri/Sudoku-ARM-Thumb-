/*********************************************************************************************
* Fichero:	timer2.c
* Autores:	Alejandro Guiu Perez - 680669 y Alvaro Juan Ciriaco - 682531
* Descrip:	funciones de control del timer2
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "44blib.h"
#include <inttypes.h>
#include "gestorPantalla.h"

/*--- variables globales ---*/
uint32_t timer2_num_int = 0;		// Número de interrupciones generadas por el timer 2.

/*--- declaracion de funciones ---*/
void timer_ISR2(void) __attribute__((interrupt("IRQ")));
void Timer2_Inicializar();
void Timer2_Empezar();
uint32_t Timer2_Leer();

/*--- codigo de las funciones ---*/
void timer_ISR2(void)
{
	timer2_num_int++;		//Aumentamos el número de interrupciones del timer2
	/* borrar bit en I_ISPC para desactivar la solicitud de interrupción*/
	rI_ISPC |= BIT_TIMER2;
}

void Timer2_Inicializar()
{
	/* Configuraion controlador de interrupciones */
	// Enmascara el bit del timer 2.
	rINTMSK &= ~(BIT_TIMER2);

	/* Establece la rutina de servicio para TIMER2 */
	pISR_TIMER2 = (unsigned) timer_ISR2;

	/* Configura el Timer2 */
	rTCFG0 &= 0xFFFF00FF; 	// Preescalado a 0
	rTCFG1 &= 0xFFFFF0FF;	// Entrada del mux. 00 divisor 1/2
	rTCNTB2 = 65535;		// Valor inicial de la cuenta del timer (valor máximo como se pide enunciado)
	rTCMPB2 = 0;			// Valor con el que se comparará la cuenta del timer2
	/* Activamos los bits para iniciar la cuenta (12 y 13) */
	rTCON = (rTCON & 0xFFFF0FFF) | 0x2000;		//Activamos manual update bit.
	rTCON = (rTCON & 0xFFFF0FFF) | 0x9000;		//Activamos start/stop y desactivamos manual update bit a la vez.
}

// Función encargada de reiniciar la cuenta de interrupciones del timer2
void Timer2_Empezar()
{
	timer2_num_int = 0;
}

// Devuelve el tiempo actual del timer 2 (tratando las condiciones de carrera).
uint32_t Timer2_Leer()
{
	uint32_t interrupcionesAntes, interrupcionesDesp, interrupcionesCuenta;
	// Leemos 2 veces las interrupciones totales del timer más una vez la cuenta actual
	interrupcionesAntes = timer2_num_int;
	uint32_t cuentaActual = rTCNTO2;
	interrupcionesDesp = timer2_num_int;
	// Si las interrupciones coinciden, no ha habido problema al leer el tiempo
	if (interrupcionesAntes == interrupcionesDesp) {
		interrupcionesCuenta = interrupcionesDesp;
	} else {
		// Si las interrupciones no coinciden en las 2 mediciones
		interrupcionesCuenta = cuentaActual>=65535/2 ?
					interrupcionesAntes : interrupcionesDesp;
	}
	// Devolvemos el tiempo actual que lleva el timer 2 en microsegundos
	return (interrupcionesCuenta*(65535/32)+((65535-cuentaActual)/32));
}

