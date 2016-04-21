/*********************************************************************************************
* Fichero:	button.c
* Autores:	Alejandro Guiu Perez - 680669 y Alvaro Juan Ciriaco - 682531
* Descrip:	Funciones de manejo de los pulsadores (EINT6-7)
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "44blib.h"
#include "44b.h"
#include "def.h"
#include "gestorSudoku.h"
#include "gestorRebotes.h"
#include "pilaDepuracion.h"
#include <inttypes.h>

enum {botonIzquierdo = 0, botonDerecho = 1};
enum {botonIzqMascara = 6, botonDerMascara = 7};
enum {interrupcionBoton = 10};

/*--- declaracion de funciones ---*/
void Eint4567_ISR(void) __attribute__((interrupt("IRQ")));
void Eint4567_init(void);

/*--- codigo de funciones ---*/
// Función que inicializa los pulsadores.
void Eint4567_init(void)
{

	/* Configuracion del controlador de interrupciones. Estos registros están definidos en 44b.h */
	rI_ISPC    = 0x3ffffff;				// Borra INTPND escribiendo 1s en I_ISPC
	rEXTINTPND = 0xf;       			// Borra EXTINTPND escribiendo 1s en el propio registro
	rINTMSK    &= ~(BIT_EINT4567); 		// Enmascara todas las lineas excepto eint4567.

	/* Establece la rutina de servicio para Eint4567 */
	pISR_EINT4567 = (int)Eint4567_ISR;

	/* Configuracion del puerto G */
	rPCONG  = 0xffff;        			// Establece la funcion de los pines (EINT0-7)
	rPUPG   = 0x0;                  	// Habilita el "pull up" del puerto
	rEXTINT = rEXTINT | 0x22222222;     // Configura las lineas de int. como de flanco de bajada

	/* Por precaucion, se vuelven a borrar los bits de INTPND y EXTINTPND */
	rI_ISPC    |= (BIT_EINT4567);
	rEXTINTPND = 0xf;
}

/* Gestión de la interrupción de la pulsación de un botón
 * Si se pulsa el botón izquierdo o el botón derecho, informa
 * al gestor de rebotes y al gestor de sudokus de la acción.
 */
void Eint4567_ISR(void)
{
	int which_int = rEXTINTPND;
	switch (which_int)
	{
		case 4:		// En caso de que sea el botón izquierdo
			gestionarRebotesPulsacion(botonIzqMascara);		//informa al gestor de rebotes de la pulsación
			gestionarSudoku(botonIzquierdo);				//informa al gestor de sudoku de la pulsación
			push_debug(interrupcionBoton, botonIzquierdo);	//Apilamos la interrupción de pulsación con botón izquierdo (4)
			break;
		case 8:		// En caso de que sea el botón derecho
			gestionarRebotesPulsacion(botonDerMascara);		//informa al gestor de rebotes de la pulsación
			gestionarSudoku(botonDerecho);					//informa al gestor de sudoku de la pulsación
			push_debug(interrupcionBoton, botonDerecho);	//Apilamos la interrupción de pulsación con botón derecho (8)
			break;
		default:	// Si es cualquier otro botón, se ignora.
			break;
	}
	rINTMSK  |= (BIT_EINT4567);		//Desactivamos la ISR
	/* Finalizar ISR */
	rEXTINTPND = 0xf;				// Borra los bits en EXTINTPND.
	rI_ISPC   |= BIT_EINT4567;		// Borra el bit pendiente en INTPND.
}

