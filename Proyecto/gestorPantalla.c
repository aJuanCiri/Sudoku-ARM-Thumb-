/*********************************************************************************************
* Fichero:	gestorPantalla.c
* Autores:	Alejandro Guiu P谷rez - 680669 y 芍lvaro Juan Ciriaco - 682531
* Descrip:	Gestiona lo que se muestra por pantalla para el caso concreto de un sudoku
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "def.h"
#include "44b.h"
#include <stdlib.h>
#include "44blib.h"
#include "lcd.h"
#include "bmp.h"
#include <inttypes.h>
#include "sudoku_2015.h"
#include "itoa.h"
#include "timer2.h"

/**
 * Pantallas posibles:
 * pantallaJuego: Muestra los elementos principales del juego, el sudoku y los tiempos.
 * pantallaConfirmacionFinal: Solicita al usuario confirmaci車n del final de la partida.
 * pantallaVolverAJugar: Muestra la pantalla de juego pausada, esperando para volver a empezarla.
 * pantallaInstrucciones: Muestra las instrucciones de juego y se queda a la espera de empezar.
 */
enum {pantallaJuego = 0, pantallaConfirmacionFinal = 1, pantallaVolverAJugar = 2,
	pantallaInstrucciones = 3};

/*--- variables internas ---*/
const int margenIzquierdoLeyenda = 0;	// Distancia desde el margen izquierdo hasta la leyenda
const int margenSuperiorLeyenda = 5;	// Distancia desde el margen superior hasta la leyenda
const int margenIzquierdoSudoku = 15;	// Distancia desde el margen izquierdo hasta el sudoku
const int margenSuperiorSudoku = 30;	// Distancia desde el margen superior hasta el sudoku
const int sizeOfCell = 22;				// Tamanho de una celda
int idSiguientePantalla = 0;			// Identificador de la siguiente pantalla a mostrar
char* leyenda;							// Texto correspondiente a la leyenda que se va a mostrar
char tiempoCalculoEnPantalla[15];		// Coste del 迆ltimo calculo en formato de texto
char tiempoTotalEnPantalla[15];			// Tiempo desde que se ha iniciado la partida en formato de texto
	
/**
 * Funcion encargada de actualizar el contenido de la pantalla, leyendo
 * el identificador de la pantalla que se debe mostrar y actualizando
 * la leyenda si fuera necesario.
 */
void actualizarPantalla(){
	if(idSiguientePantalla == pantallaJuego) {
		leyenda = "Introduzca Fila A para acabar la partida";
		dibujar_sudoku();
	} else if (idSiguientePantalla == pantallaConfirmacionFinal) {
		mostrarConfirmacionFinal();
	} else if (idSiguientePantalla == pantallaVolverAJugar) {
		leyenda = "Pulse un boton para jugar";
		dibujar_sudoku();
	} else if (idSiguientePantalla == pantallaInstrucciones) {
		mostrarInstrucciones();
	}
}

/**
 * Dibuja por pantalla el sudoku, incluidas las l赤neas, las numeraciones,
 * los datos de las celdas, los distintos colores de cada celda.
 */
void dibujar_sudoku(){
	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();

    #ifdef Eng_v
	Lcd_DspAscII8x16(10,0,DARKGRAY,"Embest S3CEV40 ");
	#else
	#endif

	Lcd_DspAscII8x16(margenIzquierdoLeyenda,margenSuperiorLeyenda,BLACK,leyenda);
	int x0 = margenIzquierdoSudoku, x1, y0 = margenSuperiorSudoku, y1=235;
	//posiciones para colocar numeracion del 1 al 9
	int margenIzquierdoNumeros = x0-10;
	int margenSuperiorNumeros = y0-10;
	int i;
	INT8U* numero = "";
	//Coloca los numeros para las columnas y filas
	for (i = 1; i<10; i++) {
		numero[0]=i+'0';
		Lcd_DspAscII8x16(x0+8,margenSuperiorNumeros,BLACK,numero);
		Lcd_DspAscII8x16(margenIzquierdoNumeros,y0+8,BLACK,numero);
		x0 += sizeOfCell;
		y0 += sizeOfCell;
	}
	//Se dibujan las lineas horizontales del sudoku:
	x0=margenIzquierdoSudoku; y0=margenSuperiorSudoku+4; x1=215;
	for(i=0; i<10; i++){
		if(i%3==0){
			Lcd_Draw_HLine(x0,x1,y0,15,3);
		} else {
			Lcd_Draw_HLine(x0,x1,y0,15,1);
		}
		y0 += sizeOfCell;
	}
	//Se dibujan las lineas verticales del sudoku:
	x0=margenIzquierdoSudoku; y0=margenSuperiorSudoku+4;
	y1=232;
	for(i=0; i<10; i++){
		if(i%3==0){
			Lcd_Draw_VLine(y0,y1,x0,15,3);
		} else {
			Lcd_Draw_VLine(y0,y1,x0,15,1);
		}
		x0 += sizeOfCell;
	}
	//Muestra los titulos de los respectivos tiempos
	Lcd_DspAscII8x16(margenIzquierdoSudoku+210,margenSuperiorSudoku+4,BLACK,"T. total:");
	Lcd_DspAscII8x16(margenIzquierdoSudoku+210,margenSuperiorSudoku+50,BLACK,"T. calculo:");
	//Se completan las celdas con los datos correspondientes
	escribirDatos();
	//
	if(idSiguientePantalla == pantallaVolverAJugar){
		actualizarTiempoTotalEnPantalla();
	}
	//Se actualiza la pantalla con la nueva informacion
	Lcd_Dma_Trans();
}

/**
 * Escribe en las celdas la informaci車n que corresponda en ese momento,
 * haciendo que su color varie segun si son pista, errores, o datos
 * que ha puesto el usuario. Tambi谷n actualiza el tiempo de calculo en pantalla.
 */
void escribirDatos(){
	int i,j;
	INT8U* numero = "";
	for(i=0; i<9; i++){
		for(j=0; j<9; j++){
			int valor = obtenerValor(i,j);
			numero[0]=valor+'0';
			int x0 = margenIzquierdoSudoku+(j)*sizeOfCell;
			int y0 = margenSuperiorSudoku+(i)*sizeOfCell;
			if(valor==0){
				//No hay valor, poner candidatos
				colocarCandidatos(i,j);
			} else if(esPista(i,j)){
				//Si es pista, se dibuja de color gris
				if(esError(i,j)) {
					//En caso de ser error y pista, se pinta de negro la celda
					LcdClrRect(x0, y0+4, x0+sizeOfCell, y0+4+sizeOfCell, BLACK);
					Lcd_DspAscII8x16(8+x0,8+y0,DARKGRAY, numero);
				} else {
					Lcd_DspAscII8x16(8+x0,8+y0,DARKGRAY, numero);
				}
			} else if(esError(i,j)){
				//Se pinta de negro la celda
				LcdClrRect(x0, y0+4, x0+sizeOfCell, y0+4+sizeOfCell, BLACK);
				Lcd_DspAscII8x16(8+x0,8+y0,WHITE, numero);
			} else{	//Se dibuja un valor normal
				Lcd_DspAscII8x16(8+x0,8+y0,BLACK, numero);
			}
		}
	}
	//Actualiza el tiempo del ultimo calculo en pantalla
	Lcd_DspAscII8x16(margenIzquierdoSudoku+215,margenSuperiorSudoku+65,BLACK,tiempoCalculoEnPantalla);
}

/**
 * Funci車n que informa por pantalla de los candidatos de una celda concreta
 * cuya posicion viene definida por parametros.
 * Para mostrar la informaci車n, se divide la celda en 9 partes, pintando
 * de negro las que correspondan de manera n迆merica y ascendente a un candidato
 * Ejemplo:		(Siendo N negro, y B blanco)
 * N N B
 * B B B
 * B N B
 * Los candidatos del ejemplo serian: 1, 2 y 8.
 */
void colocarCandidatos(int x, int y) {
	int i, j;
	int candidato = 1;
	for(i=0; i<3; i++) {
		for(j=0; j<3; j++) {
			/* Tomando como punto de referencia la esquina superior izquierda del sudoku
			 * se desplaza hacia abajo y hacia la derecha tantas veces como la posici車n
			 * x e y tengan, avanzando en cada caso el tama?o de una celda.
			 * Despu谷s hace lo mismo dentro de la celda, moviendose por las 9 partes que tiene.
			 */
			int x0 = 1+margenIzquierdoSudoku+(y)*sizeOfCell+j*sizeOfCell/3;
			int y0 = 1+margenSuperiorSudoku+(x)*sizeOfCell+i*sizeOfCell/3;
			/* En caso de que el posible candidato que se este revisando, sea finalmente candidato,
			 * se pinta un cuadrado negro en su posici車n correspondiente y calculada anteriormente.
			 */
			if(esCandidato(x, y, candidato)) {
				LcdClrRect(x0, y0+4, x0+(sizeOfCell/3), y0+4+(sizeOfCell/3), BLACK);
			}
			candidato++;	// Se revisa el siguiente posible candidato
		}
	}
}

/**
 * Muestra por pantalla un mensaje preguntando al usuario si desea finalizar,
 * y lo que debe pulsar segun su respuesta.
 */
void mostrarConfirmacionFinal(){
	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();

    #ifdef Eng_v
	Lcd_DspAscII8x16(10,0,DARKGRAY,"Embest S3CEV40 ");
	#else
	#endif
	Lcd_DspAscII8x16(0,45,BLACK,"Esta seguro de que desea finalizar?");
	Lcd_DspAscII8x16(30,75,BLACK,"Boton izquierdo - Cancelar");
	Lcd_DspAscII8x16(30,95,BLACK,"Boton derecho - Aceptar");
	//Se actualiza la pantalla con la nueva informacion
	Lcd_Dma_Trans();
}

/**
 * Muestra la pantalla de instrucciones con lo basico que hay que saber
 */
void mostrarInstrucciones(){
	/* initial LCD controller */
	Lcd_Init();
	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();

    #ifdef Eng_v
	Lcd_DspAscII8x16(10,0,DARKGRAY,"Embest S3CEV40 ");
	#else
	#endif
	Lcd_DspAscII8x16(0,25,BLACK,"Instrucciones para jugar:");
	Lcd_DspAscII8x16(0,55,BLACK,"Utilice el boton izquierdo para");
	Lcd_DspAscII8x16(0,70,BLACK,"seleccionar filas, columnas y valores");
	Lcd_DspAscII8x16(0,105,BLACK,"Utilice el boton derecho para");
	Lcd_DspAscII8x16(0,120,BLACK,"confirmar los datos introducidos");
	Lcd_Draw_HLine(0,320,140,15,3);
	Lcd_DspAscII8x16(0,175,BLACK,"Pulse un boton para jugar");
	//Se actualiza la pantalla con la nueva informacion
	Lcd_Dma_Trans();
}

/**
 * Funcion que establece el identificador de la siguiente pantalla a mostrar
 * cuando se actualice.
 */
void setPantalla(int idPantalla){
	idSiguientePantalla = idPantalla;
}

/**
 * Establece el tiempo de calculo que se debe mostrar al actualizar la pantalla
 */
void setTiempoCalculo(uint32_t tiempo){
	itoa(tiempo, tiempoCalculoEnPantalla);
}

/**
 * Muestra por pantalla el tiempo de juego que el usuario lleva en un momento concreto.
 */
void actualizarTiempoTotalEnPantalla() {
	/* Se comprueba si la pantalla que hay en este momento no es la de confirmacion,
	 * ya que en ese caso el tiempo de juego sigue avanzando, pero no debe mostrarse.
	 */
	if (idSiguientePantalla != pantallaConfirmacionFinal) {
		// Borra el contenido del area en la que se encuentra el tiempo de juego
		LcdClrRect(margenIzquierdoSudoku+215, margenSuperiorSudoku+18, LCD_XSIZE, margenSuperiorSudoku+40, WHITE);
		// Lee el tiempo que el usuario lleva, y lo guarda en formato de texto
		itoa(leerTiempoTimer4(),tiempoTotalEnPantalla);
		// Muestra el tiempo que acaba de leer.
		Lcd_DspAscII8x16(margenIzquierdoSudoku+215,margenSuperiorSudoku+20,BLACK,tiempoTotalEnPantalla);
		//Se actualiza la pantalla con la nueva informacion
		Lcd_Dma_Trans();
	}
}
