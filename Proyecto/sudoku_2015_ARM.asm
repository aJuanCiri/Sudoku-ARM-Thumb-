/************************************************************************************************/
/* Fichero: sudoku_2015_ARM.asm																	*/
/* Alejandro Guiu Pérez - 680669 																*/
/* Álvaro Juan Ciriaco - 682531											    				    */
/************************************************************************************************/

.global start
.arm
.data
.ltorg 								/*Garantiza la alineación*/
NUM_FILAS:		.long 9				/*Cantidad de filas en la memoria*/
NUM_COLUMNAS:	.long 16 			/*Cantidad de columnas en la memoria*/
.text

.global sudoku_recalcular_arm_c
.global sudoku_recalcular_arm
.global sudoku_candidatos_arm

/************************************************************************************************/
/* Recalcula el tablero y retorna el número de celdas vacías, los candidatos se calculan en ARM */
/* Parámetros:											    								    */
/* r0 = @inicio del sudoku																	    */
/************************************************************************************************/

sudoku_recalcular_arm:
		PUSH {lr}					/*Se apila la dirección de salto desde la que se ha saltado*/
		PUSH {r4-r8}				/*Se apilan los registros que se van a utilizar*/
		mov r7, r0					/*R7 = dirección inicial del sudoku*/
		mov r1, #0					/*i=0 (iterador de filas)*/
		mov r2, #0					/*j=0 (iterador de columnas)*/
		mov r3, #0					/*variable: Celdasvacias=0*/
		ldr r4, =NUM_FILAS			/*Se carga el número de filas*/
		ldr r4, [r4]
		ldr r5, =NUM_COLUMNAS		/*Se carga el número de columnas*/
		ldr r5, [r5]
		sub r5, r5, #7				/*r5 = valor real de columnas (NUM_COLUMNAS-7)*/
for1:
		cmp r1, r4					/*Se compara i con el número de filas*/
		bne for2					/*Si son distintos, hace otra iteración*/
		b finalRec					/*Si son iguales, ha terminado, y se finaliza el bucle*/
for2:
		cmp r2, r5					/*Se compara j con el número de columnas*/
		beq finFila					/*Si son iguales, se comprueba si hay que avanzar a la siguiente fila*/
		ldrh r6, [r7], #2			/*r6 tenemos valor de @memoria*/
		TST r6, #0x0800				/*Se comprueba si la celda esta vacía y no es pista*/
		addne r2, r2, #1			/*Si es pista aumentamos j*/
		bne for2					/*Se pasa a la siguiente iteración al ser pista*/
		PUSH {r0-r3}				/*Si no es pista, preparamos pila para bl*/
		bl sudoku_candidatos_arm	/* Llama a sudoku_candidatos_arm */
		mov r8, r0					/*r8 guarda el resultado de la funcion, para poder restaurar los registros r0-r3*/
		POP {r0-r3}					/*Se restauran los valores de los registros*/
		cmp r8, #0
		addeq r3, r3, #1			/*Si es 0 incrementamos celdasVacias*/
		add r2, r2, #1				/*Incrementamos j*/
		b for2

finFila:
		add r1, r1, #1				/*incrementamos i*/
 		mov r2, #0					/*reiniciamos j*/
 		add r7, r7, #14				/*desplazamos el puntero a sudoku para que apunte sig linea*/
		b for1
finalRec:
		mov r0, r3					/*Se sitúa el resultado en el registro 0 para devolverlo*/
		POP {r4-r8}					/*Se desapilan los valores iniciales de los registros*/
		POP {pc}					/*Se salta a la función inicial*/

/**********************************************************************************************/
/* Recalcula el tablero y retorna el número de celdas vacías, los candidatos se calculan en C */
/* Parámetros:											    								  */
/* r0 = @inicio del sudoku																	  */
/**********************************************************************************************/
sudoku_recalcular_arm_c:
		PUSH {lr}					/*Se apila la dirección de salto desde la que se ha saltado*/
		PUSH {r4-r8}				/*Se apilan los registros que se van a utilizar*/
		mov r7, r0					/*R7 = dirección inicial del sudoku*/
		mov r1, #0					/*i=0 (iterador de filas)*/
		mov r2, #0					/*j=0 (iterador de columnas)*/
		mov r3, #0					/*variable: Celdasvacias=0*/
		ldr r4, =NUM_FILAS			/*Se carga el número de filas*/
		ldr r4, [r4]
		ldr r5, =NUM_COLUMNAS		/*Se carga el número de columnas*/
		ldr r5, [r5]
		sub r5, r5, #7				/*r5 = valor real de columnas (NUM_COLUMNAS-7)*/
for1C:
		cmp r1, r4					/*Se compara i con el número de filas*/
		bne for2C					/*Si son distintos, hace otra iteración*/
		b finalRec					/*Si son iguales, ha terminado, y se finaliza el bucle*/
for2C:
		cmp r2, r5					/*Se compara j con el número de columnas*/
		beq finFilaC				/*Si son iguales, se comprueba si hay que avanzar a la siguiente fila*/
		ldrh r6, [r7], #2			/*r6 tenemos valor de @memoria*/
		TST r6, #0x0800				/*Se comprueba si la celda esta vacía y no es pista*/
		addne r2, r2, #1			/*Si es pista aumentamos j*/
		bne for2C					/*Se pasa a la siguiente iteración al ser pista*/
		PUSH {r0-r3}				/*Si no es pista, preparamos pila para bl*/
		bl sudoku_candidatos_c		/* Llama a sudoku_candidatos_c */
		mov r8, r0					/*r8 guarda el resultado de la funcion, para poder restaurar los registros r0-r3*/
		POP {r0-r3}					/*Se restauran los valores de los registros*/
		cmp r8, #0
		addeq r3, r3, #1			/*Si es 0 incrementamos celdasVacias*/
		add r2, r2, #1				/*Incrementamos j*/
		b for2C

finFilaC:
		add r1, r1, #1				/*incrementamos i*/
 		mov r2, #0					/*reiniciamos j*/
 		add r7, r7, #14				/*desplazamos el puntero a sudoku para que apunte sig linea*/
		b for1C

/*******************************************************************/
/* Búsqueda y actualización de los candidatos de una celda en ARM  */
/* Parámetros:													   */
/* r0 = @inicio del sudoku										   */
/* r1 = fila													   */
/* r2 = columna													   */
/*******************************************************************/

sudoku_candidatos_arm:
		PUSH {lr}					/*Se apila la dirección desde la que se ha saltado*/
		PUSH {r0,r4-r10}			/*Se apilan los registros a usar*/
		mov r8, r2					/*Guardamos en r8 la columna*/
		mov r9, r1					/*Guardamos en r9 la fila*/
		mov r3, #0					/*Iterador*/
		add r0, r0, r1, LSL #5		/*Situamos el puntero en el inicio de la fila con los candidatos a revisar*/
		ldr r1, =0x01FF				/*r1 = Futuro valor de la casilla a la que estamos mirando candidatos*/
		ldr r6, =NUM_COLUMNAS		/*Se carga el número de columnas*/
		ldr r6, [r6]
		sub r6, r6, #7

/* ----------- WHILE QUE REVISA LA FILA ------------- */
whileColumnas:
		cmp r3, r6					/*Se comprueba si se han revisado todas las columnas de la fila*/
		add r3, r3, #1				/*Se incrementa el iterador*/
		beq finColumnas				/*Si se han revisado todas las celdas de la fila, se va a fincolumnas*/
		ldrh r2, [r0], #2			/*r2=valor que hay en @sudoku*/
		TST r2, #0xF000				/*Se carga la máscara que permite averiguar si la celda esta vacía*/
		beq whileColumnas			/*Si es 0 vamos siguiente iteracion*/

		/* Eliminación del candidato: */
		mov r4, r2, LSR #12			/*Sacamos el valor (4 bits de mas peso)*/
		sub r4, r4, #1				/*Se resta uno al candidato para el desplazamiento*/
		mov r2, #1					/*Se establece el 1 a mover a la posición a borrar*/
		mvn r4, r2, LSL r4			/*Se desplaza el 1 a la posición a borrar y negamos los bits*/
		and r1, r1, r4				/*Pone a 0 el bit que corresponde al candidato eliminado*/
		b whileColumnas
finColumnas:
		POP {r0}					/*Al terminar reiniciamos r0 para revisar las columnas*/
		PUSH {r0}
		mov r3, r8
		LSL r3, #1					/*r3 = r3*2, donde 2 es la cantidad a desplazar por columna*/
		add r0, r0, r3  			/* Se posiciona en la columna que hay que revisar*/
		mov r3, #0					/*reiniciamos r3*/
		ldr r6, =NUM_FILAS			/*Se carga el número de filas*/
		ldr r6, [r6]
/* ------------ WHILE QUE REVISA LA COLUMNA --------------*/
whileFilas:
		cmp r3, r6					/*Se comprueba si se han revisado todas las filas de la columna*/
		add r3, r3, #1				/*Se incrementa el iterador*/
		beq revisionRegion			/*Si se han revisado todas las celdas de la columna, se va a revisar la región*/
		ldrh r2, [r0], #32			/*r2=valor que hay en @sudoku*/
		TST r2, #0xF000				/*Se carga la máscara que permite averiguar si la celda esta vacía*/
		beq whileFilas				/*Si es 0 vamos siguiente iteracion*/

		/* Eliminación del candidato: */
		mov r4, r2, LSR #12			/*Sacamos el valor (4 bits de mas peso)*/
		sub r4, r4, #1				/*Se resta uno al candidato para el desplazamiento*/
		mov r2, #1					/*Se establece el 1 a mover a la posición a borrar*/
		mvn r4, r2, LSL r4			/*Se desplaza el 1 a la posición a borrar y negamos los bits*/
		and r1, r1, r4				/*Pone a 0 el bit que corresponde al candidato eliminado*/
		b whileFilas

/* ------------ WHILE QUE REVISA LA REGIÓN -------------
Valores que van a tener los registros en esta parte:
r1 guarda la nueva celda con los candidatos actualizados
r3 es el iterador de celdas
r4 es el iterador de columnas (columnaX)
r5 es el iterador de filas (filaX)
r6 es la columna de inicio de la región
r7 es la fila de inicio de la región
r8 tiene la columna de la celda a la que le buscamos candidatos
r9 tiene la fila de la celda a la que le buscamos candidatos */
revisionRegion:
		/* Cálculo de la primera celda de la región */
		cmp r9, #6					/*Se calcula si la primera fila del cuadrante es 0,3 o 6*/
		blt fmenor6					/*Si es menor de 6 solo puede ser la fila 0 o la 3*/
		mov r7, #6
		b recCol
fmenor6:cmp r9, #3					/*Si es menor de 3 solo puede ser la fila 0*/
		blt fmenor3
		mov r7, #3
		b recCol
fmenor3:mov r7, #0					/*r7= FilaInicio */

recCol: cmp r8, #6					/*Se calcula si la primera columna del cuadrante es 0,3 o 6*/
		blt cmenor6					/*Si es menor de 6 solo puede ser la columna 0 o la 3*/
		mov r6, #6
		b finrev
cmenor6:cmp r8, #3
		blt cmenor3					/*Si es menor de 3 solo puede ser la columna 0*/
		mov r6, #3
		b finrev
cmenor3:mov r6, #0					/*r6= ColumnaInicio */

		/* Fin del Cálculo de la primera celda de la región */
finrev:	mov r5, r7					/* filaX */
		mov r4, r6					/* columnaX */
		POP {r0}					/*Ahora se va a posicionar el puntero en el principio del recuadro */
		PUSH {r0}
		add r0, r0, r7, LSL #5		/*Se situa en la fila correcta de la primera celda de la región*/
		add r0, r0, r6, LSL #1		/*Se situa en la columna correcta de la primera celda de la región*/
		mov r3, #0					/* i=0 (se reinicia el iterador) */

whileRegion:
		cmp r3, #9					/*Si se han revisado las 9 celdas de la región, se termina*/
		beq final
		add r3, r3, #1				/*Se incrementa el iterador*/
		ldrh r2, [r0]
		cmp r4, r8					/*Se revisa si estamos en una celda de la columna ya revisada previamente*/
		beq avanzaEnRegion
		cmp r5, r9					/*se revisa si estamos en una celda de la fila ya revisada previamente*/
		beq avanzaEnRegion
		TST r2, #0xF000				/*Se carga la máscara que permite averiguar si la celda esta vacía*/
		beq	avanzaEnRegion			/*Si es vacia, se comprueba el siguiente, si no lo es, se elimina el candidato*/

		/* Eliminación de un candidato */
		mov r2, r2, LSR #12			/*Valor de la celda actual*/
		sub r2, r2, #1				/*Se resta uno al candidato para el desplazamiento*/
		mov r10, #1					/*Se pone el 1 y se desplaza hasta la posición a borrar*/
		mvn r2, r10, LSL r2			/*Se desplaza el 1 para eliminarlo y se niegan los bits*/
		and r1, r1, r2				/*Se pone a 0 el bit que corresponde al candidato eliminado*/
		b avanzaEnRegion

avanzaEnRegion:
		add r10, r6, #2				/*temp = ColumnaInicio+2*/
		cmp r4, r10					/*¿columnaX==ColumnaInicio+2?*/
		bne cambioColumnaEnRegion
		mov r4, r6					/*ColumnaX=ColumnaInicio*/
		add r5, r5, #1				/*filaX++*/
		add r0, r0, #28				/*Cambio de fila dentro del cuadrante*/
		b whileRegion
cambioColumnaEnRegion:
		add r0, r0, #2				/*Cambio de columna dentro del cuadrante*/
		add r4, r4, #1				/*ColumnaX++*/
		b whileRegion

final:
		POP {r0} 					/*Restauramos la @ inicial del sudoku*/
		add r0, r0, r9, LSL #5		/*Se posiciona en la fila*/
		add r0, r0, r8, LSL #1		/*Se posiciona en la columna*/
		strh r1, [r0]				/*Guardamos el nuevo dato con los candidatos actualizados*/
		mov r4, r1, LSR #12			/*Se comprueba si el valor es 0 o no*/
		mov r0, #1					/*Return de la subrutina = TRUE si es distinto de 0*/
		cmp r4, #0xF000
		movne r0, #0				/*Si es 0 devolvemos FALSE*/
		POP {r4-r10}				/*Restaura los registros usados*/
		POP {pc}					/* FINAL DE LA EJECUCIÓN: salta a la función de llamada */

fin: B .

	.end
