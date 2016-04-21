/************************************************************************************************/
/* Fichero: sudoku_2015.c																		*/
/* Alejandro Guiu Pérez - 680669 																*/
/* Álvaro Juan Ciriaco - 682531											    				    */
/************************************************************************************************/

#include <inttypes.h>
#include "timer2.h"

// Tamaños de la cuadricula
// Se utilizan 16 columnas para facilitar la visualización
enum {NUM_FILAS = 9, NUM_COLUMNAS = 16};

enum {FALSE = 0, TRUE = 1};

typedef uint16_t CELDA; 
// La información de cada celda está agrupada en 16 bits con el siguiente formato (empezando en el bit más significativo):
// 4 MSB VALOR				0001 0000 0010 0000	= 0x1020	    Valor actual: 1
// 1 bit PISTA													Pista: 0 (te marca si ya esta puesto en el sudoku y no puedes cambiarlo)
// 1 bit ERROR													Error: 0
// 1 bit no usado												Bit no usado: 0
// 9 LSB CANDIDATOS												Candidatos: el 4

int primeraEjecucion = TRUE;	//Es la primera ejecucion?
uint32_t tiempoCalculo = 0;

CELDA cuadricula [NUM_FILAS][NUM_COLUMNAS] __attribute__((aligned(32)))= {
			{0x9800,0x6800,0x0000,0x0000,0x0000,0x0000,0x7800,0x0000,0x8800,0,0,0,0,0,0,0},
			{0x8800,0x0000,0x0000,0x0000,0x0000,0x4800,0x3800,0x0000,0x0000,0,0,0,0,0,0,0},
			{0x1800,0x0000,0x0000,0x5800,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
			{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1800,0x7800,0x6800,0,0,0,0,0,0,0},
			{0x2800,0x0000,0x0000,0x0000,0x9800,0x3800,0x0000,0x0000,0x5800,0,0,0,0,0,0,0},
			{0x7800,0x0000,0x8800,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
			{0x0000,0x0000,0x7800,0x0000,0x3800,0x2800,0x0000,0x4800,0x0000,0,0,0,0,0,0,0},
			{0x3800,0x8800,0x2800,0x1800,0x0000,0x5800,0x6800,0x0000,0x0000,0,0,0,0,0,0,0},
			{0x0000,0x4800,0x1800,0x0000,0x0000,0x9800,0x5800,0x2800,0x0000,0,0,0,0,0,0,0}
	};
CELDA cAux[NUM_FILAS][NUM_COLUMNAS]__attribute__((aligned(32)));
CELDA cOriginal[NUM_FILAS][NUM_COLUMNAS];

inline void celda_poner_valor(CELDA *celdaptr, uint8_t val){	//Guarda los 12 primeros bits tal cual,
	*celdaptr = (*celdaptr & 0x0FFF) | ((val & 0x000F) << 12);  //pone el valor en los ultimos 4 bits y los desplaza 12 a la izq.
}
inline uint8_t celda_leer_valor(CELDA celda){					//Te saca los bits 16-13
	return celda >> 12;
}

int obtenerValor(int i, int j){
	return celda_leer_valor(cuadricula[i][j]);
}

int esPista(int i, int j){
	return cuadricula[i][j] & 0x800;
}

int esError(int i, int j){
	return cuadricula[i][j] & 0x400;
}

int esCandidato(int i, int j, int candidato) {
	return !((cuadricula[i][j] & 1<<(candidato-1))==0);
}

extern int sudoku_recalcular_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);     //funcion a implementar en ARM
extern int sudoku_candidatos_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS], int i, int j);     //funcion a implementar en ARM
extern int sudoku_candidatos_thumb(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS], int i, int j);   //funcion a implementar en Thumb
extern int sudoku_recalcular_arm_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);
extern int sudoku_recalcular_arm_thumb(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dada una determinada celda encuentra los posibles valores candidatos y guarda el mapa de bits en la celda
// retorna false si la celda esta vacia, true si contiene un valor
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int sudoku_candidatos_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS], uint8_t fila, uint8_t columna) {
	//iniciar candidatos
	cuadricula[fila][columna] |= 0x01FF;
	//recorrer fila recalculando candidatos
	uint8_t filaX = 0;
	uint8_t columnaX = 0;
	while(columnaX<9){
		if (((cuadricula[fila][columnaX] & 0x0800) != 0) ||
				((cuadricula[fila][columnaX] & 0xF000) != 0)) { 			//Si es pista lo añadimos o no es pista pero es !=0
			uint8_t valor = celda_leer_valor(cuadricula[fila][columnaX]);	//Sacamos el valor
			cuadricula[fila][columna] &= ~(1 << (valor-1));					//Ponemos el valor a 0 (no se puede poner)
		}
		columnaX++;
	}
	//recorer columna recalculando candidatos
	while(filaX<9){
		if (((cuadricula[filaX][columna] & 0x0800) != 0) ||
				((cuadricula[filaX][columna] & 0xF000) != 0)) { 			//Si es pista lo añadimos o no es pista pero es !=0
			uint8_t valor = celda_leer_valor(cuadricula[filaX][columna]);	//Sacamos el valor
			cuadricula[fila][columna] &= ~(1 << (valor-1));					//Ponemos el valor a 0 (no se puede poner)
		}
		filaX++;
	}
	//recorrer region recalculando candidatos
	uint8_t fInicio = fila - (fila%3);       //Se posiciona en la primera posicion del cuadrante y se usa para iterar dentro
	uint8_t cInicio = columna;
	cInicio = cInicio - (columna%3);
	filaX=fInicio; columnaX=cInicio;
	int i = 0;
	while(i<9){
		if(filaX!=fila && columnaX!=columna){
			if (((cuadricula[filaX][columnaX] & 0x0800) != 0) ||
					((cuadricula[filaX][columnaX] & 0xF000) != 0)) { 			//Si es pista lo añadimos o no es pista pero es !=0
				uint8_t valor = celda_leer_valor(cuadricula[filaX][columnaX]);	//Sacamos el valor
				cuadricula[fila][columna] &= ~(1 << (valor-1));					//Ponemos el valor a 0 (no se puede poner)
			}
		}
		if(filaX==fInicio+2){	//Avanzamos por el sudoku
			filaX=fInicio;
			columnaX++;
		}else{
			filaX++;
		}
		i++;
	}
	
	//retornar indicando si la celda tiene un valor o esta vacia
	if ((cuadricula[fila][columna] & 0xF000) != 0) return TRUE;
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// recalcula todo el tablero (9x9) buscando candidatos con C
// retorna el numero de celdas vacias
////////////////////////////////////////////////////////////////////////////////
int sudoku_recalcular_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]) {
	int celdasVacias = 0;
	int i;
	int j;
	for(i=0;i<NUM_FILAS;i++){ 	//para cada fila
		for(j=0;j<NUM_COLUMNAS-7;j++){ //para cada columna
			if(((cuadricula[i][j] & 0x0800) == 0) && !sudoku_candidatos_c(cuadricula,i,j)){ //determinar candidatos si no es pista
				celdasVacias++; //actualizar contador de celdas vacias
			}
		}
	}
	return celdasVacias; //retornar el numero de celdas vacias
}
////////////////////////////////////////////////////////////////////////////////
// recalcula todo el tablero (9x9) buscando candidatos con ARM
// retorna el numero de celdas vacias
////////////////////////////////////////////////////////////////////////////////
int sudoku_recalcular_c_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]) {
	int celdasVacias = 0;
	int i;
	int j;
	for(i=0;i<NUM_FILAS;i++){ 	//para cada fila
		for(j=0;j<NUM_COLUMNAS-7;j++){ //para cada columna
			if(((cuadricula[i][j] & 0x0800) == 0) && !sudoku_candidatos_arm(cuadricula,i,j)){ //determinar candidatos si no es pista
				celdasVacias++; //actualizar contador de celdas vacias
			}
		}
	}
	return celdasVacias; //retornar el numero de celdas vacias
}

////////////////////////////////////////////////////////////////////////////////
// recalcula todo el tablero (9x9) buscando candidatos con THUMB
// retorna el numero de celdas vacias
////////////////////////////////////////////////////////////////////////////////
int sudoku_recalcular_c_thumb(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]) {
	int celdasVacias = 0;
	int i;
	int j;
	for(i=0;i<NUM_FILAS;i++){ 	//para cada fila
		for(j=0;j<NUM_COLUMNAS-7;j++){ //para cada columna
			if(((cuadricula[i][j] & 0x0800) == 0) && !sudoku_candidatos_thumb(cuadricula,i,j)){ //determinar candidatos si no es pista
							celdasVacias++; //actualizar contador de celdas vacias
			}
		}
	}
	return celdasVacias; //retornar el numero de celdas vacias
}

////////////////////////////////////////////////////////////////////////////////
// valida un tablero (9x9) comparandolo con otro cuyos resultados son correctos
//retorna 1 si y solo si son iguales, 0 en caso contrario
////////////////////////////////////////////////////////////////////////////////
int validar(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS], CELDA resultado_C[NUM_FILAS][NUM_COLUMNAS]) {
	int i,j,iguales=TRUE;
	for(i=0;i<NUM_FILAS;i++) {
		for(j=0;j<NUM_COLUMNAS;j++) {
			if(cuadricula[i][j]!=resultado_C[i][j]){
				iguales = FALSE;
				return iguales;
			}
		}
	}
	return iguales;
}

////////////////////////////////////////////////////////////////////////////////
// Reinicia un tablero (9x9) al tablero por inicial
////////////////////////////////////////////////////////////////////////////////
void reiniciar_tablero(CELDA tablero [NUM_FILAS][NUM_COLUMNAS]) {
	int i,j;
	for(i=0;i<NUM_FILAS;i++) {
		for(j=0;j<NUM_COLUMNAS-7;j++) {
			tablero[i][j] = cOriginal[i][j];
		}
	}
	primeraEjecucion = TRUE;
	tiempoCalculo = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Recalcula todo el sudoku
////////////////////////////////////////////////////////////////////////////////
void iniciarArmThumb(){
	uint32_t tiempo, t1, t2, t;

	if(primeraEjecucion){
		int i,j;
		for(i=0;i<NUM_FILAS;i++) {
			for(j=0;j<NUM_COLUMNAS;j++) {
				cOriginal[i][j] = cuadricula[i][j];
			}
		}
		primeraEjecucion = FALSE;
	} else {
		reiniciar_tablero(cuadricula);
	}
	Timer2_Inicializar();
	Timer2_Empezar();				// Comienza la cuenta del timer 2.
	t1 = Timer2_Leer();				// Leemos el tiempo de la partida.
	sudoku_recalcular_arm_thumb(cuadricula);
	t2=Timer2_Leer();
	tiempoCalculo+=(t2-t1);
	setTiempoCalculo(tiempoCalculo);
}

////////////////////////////////////////////////////////////////////////////////
// Inserta un valor en una celda y recalcula todo el sudoku, si y solo si
// dicha celda no es pista. Devuelve las celdas vacias del sudoku.
////////////////////////////////////////////////////////////////////////////////
int insertValor(int fila, int columna, int valor){
	if(!esPista(fila-1,columna-1)){
		uint32_t tiempo;
		celda_poner_valor(&cuadricula[fila-1][columna-1], valor);
		Timer2_Inicializar();
		uint32_t t1 = Timer2_Leer();		// Leemos el tiempo de la partida.
		int celdasVacias = sudoku_recalcular_arm_thumb(cuadricula);
		uint32_t t2=Timer2_Leer();
		tiempoCalculo+=(t2-t1);
		setTiempoCalculo(tiempoCalculo);
		return celdasVacias;
	}
}

////////////////////////////////////////////////////////////////////////////////
// proceso principal del juego que recibe el tablero,
// y la señal de ready que indica que se han actualizado fila y columna
////////////////////////////////////////////////////////////////////////////////
void sudoku9x9() {
	uint32_t t1, t2;
	int i,j,todos_correctos=TRUE;
	for(i=0;i<NUM_FILAS;i++) {
		for(j=0;j<NUM_COLUMNAS;j++) {
			cAux[i][j] = cuadricula[i][j];
			cOriginal[i][j] = cuadricula[i][j];
		}
	}
	t1=Timer2_Leer();
	sudoku_recalcular_c(cuadricula);
	t2=Timer2_Leer();
	t2=t2-t1;

	t1=Timer2_Leer();
	sudoku_recalcular_c_arm(cAux);
	t2=Timer2_Leer();
	t2=t2-t1;
	if(!validar(cAux, cuadricula)) 	todos_correctos=FALSE;
	reiniciar_tablero(cAux);

	t1=Timer2_Leer();
	sudoku_recalcular_c_thumb(cAux);
	t2=Timer2_Leer();
	t2=t2-t1;
	if(!validar(cAux, cuadricula)) 	todos_correctos=FALSE;
	reiniciar_tablero(cAux);

	t1=Timer2_Leer();
	sudoku_recalcular_arm(cAux);
	t2=Timer2_Leer();
	t2=t2-t1;
	if(!validar(cAux, cuadricula)) 	todos_correctos=FALSE;
	reiniciar_tablero(cAux);

	t1=Timer2_Leer();
	sudoku_recalcular_arm_c(cAux);
	t2=Timer2_Leer();
	t2=t2-t1;
	if(!validar(cAux, cuadricula)) 	todos_correctos=FALSE;
	reiniciar_tablero(cAux);

	t1=Timer2_Leer();
	sudoku_recalcular_arm_thumb(cAux);
	t2=Timer2_Leer();
	t2=t2-t1;
	if(!validar(cAux, cuadricula)) 	todos_correctos=FALSE;
}

