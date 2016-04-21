/*********************************************************************************************
* Fichero:		led.c
* Autores:		Alejandro Guiu Perez - 680669 y Alvaro Juan Ciriaco - 682531
* Descrip:		funciones de control de los LED de la placa
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "44blib.h"

/*--- variables globales ---*/
int led_state;							// estado del LED

/*--- declaracion de funciones ---*/
void leds_on();							// todos los leds encendidos
void leds_off();						// todos los leds apagados
void led1_on();							// led 1 encendido
void led1_off();						// led 1 apagado
void led2_on();							// led 2 encendido
void led2_off();						// led 2 apagado
void leds_switch();						// invierte el valor de los leds
void Led_Display(int LedStatus);		// control directo del LED

/*--- codigo de las funciones ---*/
// Pone todos los leds encendidos.
void leds_on()
{
	Led_Display(0x3);
}

// Apaga todos los leds.
void leds_off()
{
	Led_Display(0x0);
}

// Enciende el led 1.
void led1_on()
{
	led_state = led_state | 0x1;
	Led_Display(led_state);
}

// Apaga el led 1.
void led1_off()
{
	led_state = led_state & 0xfe;
	Led_Display(led_state);
}

// Enciende el led 2.
void led2_on()
{
	led_state = led_state | 0x2;
	Led_Display(led_state);
}

// Apaga el led 2.
void led2_off()
{
	led_state = led_state & 0xfd;
	Led_Display(led_state);
}

// Intercambia el estado de los leds.
void leds_switch()
{
	led_state ^= 0x03;
	Led_Display(led_state);

}

// Intercambia el estado de los leds.
void Led_Display(int LedStatus)
{
	led_state = LedStatus;

	if ((LedStatus & 0x01) == 0x01)
		rPDATB = rPDATB & 0x5ff; /* poner a 0 el bit 9 del puerto B */
	else
		rPDATB = rPDATB | 0x200; /* poner a 1 el bit 9 del puerto B */

	if ((LedStatus & 0x02) == 0x02)
		rPDATB = rPDATB & 0x3ff; /* poner a 0 el bit 10 del puerto B */
	else
		rPDATB = rPDATB | 0x400; /* poner a 1 el bit 10 del puerto B */
}
