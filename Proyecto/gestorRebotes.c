/*********************************************************************************************
* Fichero:		gestorRebotes.c
* Autores:		Alejandro Guiu Perez - 680669 y Alvaro Juan Ciriaco - 682531
* Descrip:		funciones de control del timer0 del s3c44b0x
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "44blib.h"
#include "gestorSudoku.h"

enum {botonIzquierdo = 0, botonDerecho = 1};
enum {botonIzqMascara = 6, botonDerMascara = 7};
enum {SinPulsar = 0, Pulsado = 1, Soltando = 2};
enum {false = 0, true = 1};

/*--- variables globales ---*/
int haLlegadoA500ms = false;			//vale true si se ha mantenido el botón izquierdo 500ms
int veces10ms = 0;						//Lleva la cuenta de las veces que han pasado 10ms
int estado = SinPulsar;					//Estado actual de la maquina de estados del gestor
int botonPulsado;						//Guarda el botón pulsado en la máscara
int tiempoAlPulsar_Soltar = 10000;		//Equivale a 250 ms
int tiempoSiguePulsado = 1290;			//Equivale a 10 ms

/*--- declaracion de funciones ---*/
// Para definir la rutina de captura de la interrupción del timer.
void timer0_ISR(void) __attribute__((interrupt("IRQ")));
void timer0_init(int);

/*--- codigo de las funciones ---*/
// Rutina de captura de una interrupción del timer.
void timer0_ISR(void)
{
	/* Si el botón se estaba soltando, actualiza el estado como sinPulsar
	 * y reinicia los valores del timer además de los botones.
	 * Si esta pulsado, solicita al timer0 que le avise en [tiempoSiguePulsado] ms
	 */
	if(estado == Soltando){ 	//estado2
		estado = SinPulsar;  	//va a estado0
		//Se reinician las variables empleadas para el control de la opción de
		//mantener pulsado los botones
		haLlegadoA500ms = false;
		veces10ms = 0;
		//Se reinician los botones
		rINTMSK    &= ~(BIT_EINT4567); // Enmascara todas las lineas excepto eint4567.
		/* Por precaucion, se vuelven a borrar los bits de INTPND y EXTINTPND */
		rI_ISPC    |= (BIT_EINT4567);
		rEXTINTPND = 0xf;
	}
	//Si se ha soltado el botón
	/*else estado es Pulsado*/
	else if(rPDATG & (1<<botonPulsado)){
		timer0_init(tiempoAlPulsar_Soltar);
		estado = Soltando;
	}
	//Si se mantiene pulsado
	else if(!(rPDATG & (1<<botonPulsado))){ //estado1
		//Si se pulsa el botón de selección y se esta manteniendo
		if(botonPulsado==botonIzqMascara){
			veces10ms++;
			//Si lleva 500ms asi se informa al gestor del sudoku
			if(!haLlegadoA500ms && veces10ms==50){
				haLlegadoA500ms = true;
				gestionarSudoku(botonIzquierdo);
				veces10ms = 0;
			}
			//Si ha alcanzado los 500 ms, aumenta cada 300ms
			else if(haLlegadoA500ms && veces10ms==30){
				veces10ms=0;
				gestionarSudoku(botonIzquierdo);
			}
		}
		timer0_init(tiempoSiguePulsado);
	}
	/* borrar bit en I_ISPC para desactivar la solicitud de interrupción*/
	rI_ISPC |= BIT_TIMER0;
}

/**
 * Funcion encargada de guardar una referencia al boton que esta
 * pulsado (izquierdo, derecho...) e inicializa el timer correspondiente.
 */
void gestionarRebotesPulsacion(int boton){
	estado = Pulsado; 		//Avanzamos de estado
	botonPulsado = boton;	//Indicamos que boton esta pulsado
	timer0_init(tiempoAlPulsar_Soltar);	//Inicializamos el timer
}

// Inicializa los valores del timer.
void timer0_init(int tiempo)
{
	/* Configuracion controlador de interrupciones */
	rINTMSK &= ~(BIT_TIMER0); // Emascara el bit del timer 0.

	/* Establece la rutina de servicio para TIMER0 */
	pISR_TIMER0 = (unsigned) timer0_ISR;

	/* Configura el Timer0 */
	rTCFG0 |= 0x000000FF; // ajusta el preescalado a 255
	rTCFG1 &= 0xFFFFFFF0; // selecciona la entrada del mux que proporciona el reloj. La 00 corresponde a un divisor de 1/2.
	rTCNTB0 = tiempo;// valor inicial de cuenta (la cuenta es descendente)
	rTCMPB0 = 0;// valor de comparación
	/* establecer update=manual (bit 1) + inverter=on (¿? será inverter off un cero en el bit 2 pone el inverter en off)*/
	rTCON = (rTCON & 0xFFFFFFF0) | 0x2;
	/* iniciar timer (bit 0) con auto-reload (bit 3)*/
	rTCON = (rTCON & 0xFFFFFFF0) | 0x9;
}

