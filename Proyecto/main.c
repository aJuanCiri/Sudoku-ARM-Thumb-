/*********************************************************************************************
* Fichero:	main.c
* Autores:	Alejandro Guiu Perez - 680669 y Alvaro Juan Ciriaco - 682531
* Descrip:	punto de entrada de C
* Version:  <P4-ARM.timer-leds>
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "44blib.h"
#include "44b.h"
#include "stdio.h"
#include "gestorSudoku.h"
#include "gestorPantalla.h"
#include <inttypes.h>
#include "timer2.h"
#include "timer4.h"

enum {pantallaJuego = 0, pantallaConfirmacionFinal = 1, pantallaVolverAJugar = 2,
	pantallaInstrucciones = 3};

typedef uint16_t CELDA;

/*--- funciones externas ---*/
extern void Eint4567_init();
extern void D8Led_init();
extern void init_excep();

/*--- declaracion de funciones ---*/
void Main(void);

/*--- codigo de funciones ---*/
void Main(void)
{
	/* Inicializa controladores */
	sys_init();       					// Inicializacion de la placa, interrupciones y puertos.
	Eint4567_init();					// Inicializamos los pulsadores. Cada vez que se pulse se verá reflejado en el 8led.
	D8Led_init(); 						// Inicializamos el 8led.
	init_excep();						// Inicializamos tratamiento de excepciones
	setPantalla(pantallaInstrucciones);	// Mostrar la pantalla de instrucciones
	actualizarPantalla();				// Actualizar la pantalla
	while(1) {
		// Si hay que actualizar el tiempo de juego en la pantalla
		if(hayQueActualizarTiempo()) {
			// Dibujamos el nuevo tiempo de juego
			desactivarActualizacion();
			actualizarTiempoTotalEnPantalla();
		}
		// Si hay que pintar una nueva "pantalla de juego" en la pantalla
		if(hayQueRefrescar()){
			//Si es asi, desactiva la peticion de pintar, y muestra por
			//pantalla el sudoku actualizado.
			refrescarPantalla(0);
			actualizarPantalla();
		}
	}
}
