/*
 * timer.h
 *
 *  Created on: 24/11/2015
 *      Author: a680669
 */

#ifndef TIMER_H_
#define TIMER_H_

void timer0_ISR(void);
void timer0_init(int tiempo);
void gestionarRebotes(int boton);
void reiniciarValoresTimer0();

#endif /* TIMER_H_ */
