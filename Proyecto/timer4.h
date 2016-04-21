/*
 * gestorPantalla.h
 *
 *  Created on: 14/12/2015
 *      Author: a680669
 */
#include <inttypes.h>

#ifndef TIMER4_H_
#define TIMER4_H_

int desactivarActualizacion();
int hayQueActualizarTiempo();
uint32_t leerTiempoTimer4();
void timer4_stop(void);
void timer4_init(void);
void timer4_reset(void);

#endif /* TIMER4_H_ */
