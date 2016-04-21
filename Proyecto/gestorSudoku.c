/*********************************************************************************************
* Fichero:		gestorSudoku.c
* Autores:		Alejandro Guiu Perez - 680669 y Alvaro Juan Ciriaco - 682531
* Descrip:		gestor encargado del funcionamiento del sudoku
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "8led.h"
#include "sudoku_2015.h"
#include "gestorPantalla.h"
#include "timer2.h"
#include "timer4.h"

enum {sinEmpezar = 0, mostrarF = 1, elegirFila = 2, mostrarC = 3,
	elegirColumna = 4, mostrarD = 5, elegirValorDato = 6, confirmarFinal=7};
enum {botonIzquierdo = 0, botonDerecho = 1};
enum {pantallaJuego = 0, pantallaConfirmacionFinal = 1, pantallaVolverAJugar = 2,
	pantallaInstrucciones = 3};

/*--- variables globales ---*/
int estadoSudoku = sinEmpezar; 		// Variable que marca estado actual del sudoku
int datoMostradoPorPantalla = 0;	// Indica el numero actual que se muestra por el 8Led
int fila, columna;					// Indican fila y columna del dato a introducir o modificar
int pintar = 0;						// Indica si hay que pintar la pantalla


/**
 * Gestiona el juego del sudoku, actualizando los estados
 * y mostrando las salidas correspondientes
 * por la pantalla 8Led.
 */
void gestionarSudoku(int botonPulsado) {
	/* ESTADOSUDOKU = SIN EMPEZAR */
	if (estadoSudoku == sinEmpezar) {
		timer4_reset();					// Inicializamos timer que indica el tiempo de juego total
		timer4_init();
		estadoSudoku = mostrarF;
		datoMostradoPorPantalla = 15;	// Mostramos una F por 8Led
		iniciarArmThumb();				// Iniciamos algoritmo (ARM-THUMB)
		setPantalla(pantallaJuego);		// Indicamos la pantalla a dibujar
		refrescarPantalla(1);			// Refrescamos la pantalla
	}

	/* ESTADOSUDOKU = MOSTRAR F */
	else if (estadoSudoku == mostrarF && botonPulsado == botonIzquierdo) {
		datoMostradoPorPantalla = 1;	// Mostramos en 8Led un 1
		estadoSudoku = elegirFila;		// Avanzamos de estado
	}

	/* ESTADOSUDOKU = ELEGIR FILA */
	else if (estadoSudoku == elegirFila) {
		if (botonPulsado == botonIzquierdo) {
			datoMostradoPorPantalla++;
			if (datoMostradoPorPantalla == 11) {
				datoMostradoPorPantalla = 1;
			}
		} else if (botonPulsado == botonDerecho) {
			fila = datoMostradoPorPantalla;
			// Si seleccionamos una "A" vamos a la pantalla de confirmarFinal
			if (fila == 10) {
				estadoSudoku = confirmarFinal;
				setPantalla(pantallaConfirmacionFinal);
				refrescarPantalla(1);
			} else {
				// Avanzamos siguiente estado (mostrar C)
				estadoSudoku = mostrarC;
				datoMostradoPorPantalla = 12;
			}
		}
	}

	/* ESTADOSUDOKU = MOSTRAR C */
	else if (estadoSudoku == mostrarC && botonPulsado == botonIzquierdo) {
		datoMostradoPorPantalla = 1;	// Mostramos un 1 por el 8Led
		estadoSudoku = elegirColumna;
	}

	/* ESTADOSUDOKU = ELEGIR COLUMNA */
	else if (estadoSudoku == elegirColumna) {
		if (botonPulsado == botonIzquierdo) {
			datoMostradoPorPantalla++;
			if (datoMostradoPorPantalla == 10) {
				datoMostradoPorPantalla = 1;
			}
		} else if (botonPulsado == botonDerecho) {
			// Avanzamos siguiente estado (mostrar D)
			columna = datoMostradoPorPantalla;
			estadoSudoku = mostrarD;
			datoMostradoPorPantalla = 13;
		}
	}

	/* ESTADOSUDOKU = MOSTRAR D */
	else if (estadoSudoku == mostrarD && botonPulsado == botonIzquierdo) {
		datoMostradoPorPantalla = 1;
		estadoSudoku = elegirValorDato;
	}

	/* ESTADOSUDOKU = ELEGIR VALOR DATO */
	else if (estadoSudoku == elegirValorDato) {
		if (botonPulsado == botonIzquierdo) {
			datoMostradoPorPantalla = (datoMostradoPorPantalla+1)%10;
		} else if (botonPulsado == botonDerecho) {
			// Insertamos en el sudoku el valor dado por el usuario
			int celdasVacias = insertValor(fila, columna, datoMostradoPorPantalla);
			if (celdasVacias == 0) {
				// Si ya no hay celdas vacias es que el sudoku se ha realizado con exito
				estadoSudoku = sinEmpezar;
				// Reiniciamos maquina de estados y mostramos por pantalla los resultados
				setPantalla(pantallaVolverAJugar);
				timer4_stop();
			} else {
				// Si el sudoku no ha acabado volvemos a realizar el mismo proceso
				estadoSudoku = mostrarF;
				datoMostradoPorPantalla = 15;
			}
			refrescarPantalla(1);
		}
	}
	/* ESTADOSUDOKU = CONFIRMAR EL FINAL DEL JUEGO */
	else if (estadoSudoku == confirmarFinal){
		if (botonPulsado == botonIzquierdo) {
			// Cancelamos la finalizacion del juego
			estadoSudoku = mostrarF;
			datoMostradoPorPantalla = 15;	// Mostramos una F por pantalla
			setPantalla(pantallaJuego);
		} else if(botonPulsado == botonDerecho) {
			// Terminamos de jugar y se muestra por pantalla el resultado actual
			timer4_stop();
			estadoSudoku = sinEmpezar;
			setPantalla(pantallaVolverAJugar);
		}
		refrescarPantalla(1);
	}
	// Se muestra por el 8Led el valor de la variable
	D8Led_symbol(datoMostradoPorPantalla);
}

/**
 * Devuelve el valor de la varible [pintar]
 */
int hayQueRefrescar(){
	return pintar;
}

/**
 * Asigna el valor de la variable X a [pintar]
 */
void refrescarPantalla(int x){
	pintar = x;
}
