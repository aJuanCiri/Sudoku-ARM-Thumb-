/*
 * Fichero: gestorPantalla.h
 * Autores:
 * Alejandro Guiu Pérez - 680669
 * Álvaro Juan Ciriaco - 682531	
 * Descripcion:
 * Colección de funciones que pueden ser empleadas externamente en lo relacionado 
 * a la pantalla, tales como solicitar que se actualice, establecer la siguiente
 * pantalla, o establecer y actualizar los tiempos de calculo y de juego.
 */

#ifndef GESTORPANTALLA_H_
#define GESTORPANTALLA_H_

/**
 * Pantallas posibles:
 * pantallaJuego: Muestra los elementos principales del juego, el sudoku y los tiempos.
 * pantallaConfirmacionFinal: Solicita al usuario confirmación del final de la partida.
 * pantallaVolverAJugar: Muestra la pantalla de juego pausada, esperando para volver a empezarla.
 * pantallaInstrucciones: Muestra las instrucciones de juego y se queda a la espera de empezar.
 */

/**
 * Funcion encargada de actualizar el contenido de la pantalla, leyendo
 * el identificador de la pantalla que se debe mostrar y actualizando
 * la leyenda si fuera necesario.
 */
void actualizarPantalla();

/**
 * Funcion que establece el identificador de la siguiente pantalla a mostrar
 * cuando se actualice.
 */
void setPantalla(int idPantalla);

/**
 * Establece el tiempo de calculo que se debe mostrar al actualizar la pantalla
 */
void setTiempoCalculo(int tiempo);

/**
 * Muestra por pantalla el tiempo de juego que el usuario lleva en un momento concreto.
 */
void actualizarTiempoTotalEnPantalla();

#endif /* GESTORPANTALLA_H_ */
