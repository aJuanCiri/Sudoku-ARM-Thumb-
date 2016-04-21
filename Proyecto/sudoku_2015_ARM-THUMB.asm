/************************************************************************************************/
/* Fichero: sudoku_2015_ARM-THUMB.asm															*/
/* Alejandro Guiu Pérez - 680669 																*/
/* Álvaro Juan Ciriaco - 682531											    				    */
/************************************************************************************************/

.arm
.data
.ltorg 								/*Garantiza la alineación*/
NUM_FILAS:		.long 9				/*Cantidad de filas en la memoria*/
NUM_COLUMNAS:	.long 16 			/*Cantidad de columnas en la memoria*/
.text

.global sudoku_recalcular_arm_thumb
.global sudoku_candidatos_thumb

/**************************************************************************************************/
/* Recalcula el tablero y retorna el número de celdas vacías, los candidatos se calculan en THUMB */
/* Parámetros:											    								      */
/* r0 = @inicio del sudoku																	      */
/**************************************************************************************************/

sudoku_recalcular_arm_thumb:
		PUSH {lr}					/*Se apila la dirección de salto desde la que se ha saltado*/
		PUSH {r4-r8}				/*Se apilan los registros que se van a utilizar*/
		mov r1, #0					/*i=0 (iterador de filas)*/
		mov r2, #0					/*j=0 (iterador de columnas)*/
		mov r3, #0					/*variable: Celdasvacias=0*/
		mov r7, #0					/*Vale 1 si ha habido alguna celda con error*/
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
		PUSH {r0-r3}				/*Si no es pista, preparamos pila para bl*/
		bl sudoku_candidatos_thumb	/* Llama a sudoku_candidatos_thumb */
		mov r8, r0					/*r8 guarda el resultado de la funcion, para poder restaurar los registros r0-r3*/
		POP {r0-r3}					/*Se restauran los valores de los registros*/
		cmp r8, #0
		addeq r3, r3, #1			/*Si es 0 incrementamos celdasVacias*/
		cmp r8, #0
		addlt r7, #1				/*si hay error se incrementa el contador de errores*/
		add r2, r2, #1				/*Incrementamos j*/
		b for2

finFila:
		add r1, r1, #1				/*incrementamos i*/
 		mov r2, #0					/*reiniciamos j*/
		b for1
finalRec:
		mov r0, r3					/*Se sitúa el resultado en el registro 0 para devolverlo*/
		cmp r7, #0
		ble terminar
		mov r3, #0
		sub r3, r3, #1
		mov r7, r0
		mul r0, r7, r3				/*Se actualiza para que celdasVacias sea negativo*/
terminar:
		POP {r4-r8}					/*Se desapilan los valores iniciales de los registros*/
		POP {pc}					/*Se salta a la función inicial*/

/***********************************************************************************/
/* Encuentra todos los candidatos de una celda, y los actualiza en la propia celda */
/* Tras lo anterior, devuelve cierto si contiene un valor, o falso si esta vacía   */
/***********************************************************************************/

/*Paso de ARM a Thumb*/
sudoku_candidatos_thumb:
		PUSH {lr}					/*Se apila la dirección desde la que se ha saltado*/
		ldr r3, = sudoku_candidatos_thumb2+1
		bx r3						/*Salta a la función en modo thumb*/

/*******************************************************************/
/* Búsqueda y actualización de los candidatos de una celda en THUMB*/
/* Parámetros:													   */
/* r0 = @inicio del sudoku										   */
/* r1 = fila													   */
/* r2 = columna													   */
/*******************************************************************/
.thumb
sudoku_candidatos_thumb2:
		PUSH {r0-r2,r4-r7}			/*Se apilan los registros a usar*/
		mov r3, #0					/*Se inicializa el iterador*/
		LSL r1, #5					/*Se desplaza la fila 5 bits, teniendo los bytes respecto a la inicial*/
		add r0, r0, r1				/*Situamos el puntero en el inicio de la fila con los candidatos a revisar*/
		mov r1, r2
		LSL r1, #1
		add r7, r0, r1				/*r7 = @ de la celda principal*/
		ldr r1, =0x01FF				/*r1 = Futuro valor de la casilla a la que estamos mirando candidatos*/
		ldr r6, =NUM_COLUMNAS		/*Se carga el número de columnas*/
		ldr r6, [r6]
		sub r6, r6, #7

		/* ----------- WHILE QUE REVISA LA FILA ------------- */
whileColumnas:
		cmp r3, r6					/*Se comprueba si se han revisado todas las columnas de la fila*/
		bge finColumnas
		add r3, r3, #1				/*Se actualiza el iterador de celdas recorridas*/
		ldrh r2, [r0]				/*r2=valor que hay en @sudoku*/
		mov r5, r0
		add r0, r0, #2				/*r0 apunta a siguiente @sudoku*/
		cmp r5, r7
		beq whileColumnas
		ldr r5, =0xF000				/*Se carga la máscara que permite averiguar si la celda esta vacía*/
		TST r2, r5					/*Se compara la celda con la máscara*/
		beq whileColumnas			/*Si es vacia, se comprueba el siguiente, si no lo es, se elimina el candidato*/

		/* Eliminación de un candidato: */
		LSR r2, r2, #12				/*Sacamos el valor de la cuadricula en la que estamos (4 bits)*/
		sub r2, r2, #1				/*Se resta uno al candidato para el desplazamiento*/
		mov r4, #1					/*Ponemos el 1 y lo desplazamos hasta la posicion a borrar*/
		LSL r4, r4, r2				/*Se desplaza el 1 para eliminarlo*/
		mvn r5, r4					/*Negamos los bits*/
		and r1, r5					/*Pone a 0 el bit que corresponde al candidato eliminado*/
		b whileColumnas

finColumnas:
		mov r4, r1					/*Movemos candidatos a r4 para no perderlo al restaurar registros*/
		POP {r0-r2}					/*Restauramos los registros con la columna, fila y @inicio del sudoku*/
		PUSH {r0-r2}				/*Se mantienen guardados para su futura reutilización*/
		mov r1, r4					/*Volvemos a meter candidatos a r1*/
		LSL r2, #1					/*Se desplaza la columna 1 bit, teniendo los bytes respecto a la inicial*/
		add r0, r0, r2  			/*Se posiciona en la columna que hay que revisar*/
		ldr r6, =NUM_FILAS			/*Se carga el número de filas*/
		ldr r6, [r6]
		mov r3, #0					/*Reiniciamos el iterador*/

		/* ------------ WHILE QUE REVISA LA COLUMNA --------------*/
whileFilas:
		cmp r3, r6					/*Se comprueba si se han revisado todas las filas de la columna*/
		beq revisionRegion			/*si es asi, se pasa a revisar la región*/
		add r3, r3, #1				/*Se incrementa el iterador*/
		ldrh r2, [r0]				/*Se carga en r2 el valor*/
		mov r5, r0
		add r0, r0, #32				/*Se pasa a la siguiente fila*/
		cmp r5, r7
		beq whileFilas
		ldr r5, =0xF000				/*Se carga la máscara que permite averiguar si la celda esta vacía*/
		TST r2, r5					/*Se compara la celda con la máscara*/
		beq whileFilas				/*Si es vacia, se comprueba el siguiente, si no lo es, se elimina el candidato*/

		/* Eliminación de un candidato: */
		LSR r2, r2, #12				/*Sacamos el valor de la cuadricula en la que estamos (4 bits)*/
		sub r2, r2, #1				/*Se resta uno al candidato para el desplazamiento*/
		mov r4, #1					/*Ponemos el 1 y lo desplazamos hasta la posicion a borrar*/
		LSL r4, r4, r2				/*Se desplaza el 1 para eliminarlo*/
		mvn r5, r4					/*Negamos los bits*/
		and r1, r5					/*Pone a 0 el bit que corresponde al candidato eliminado*/
		b whileFilas

		/* ------------ WHILE QUE REVISA LA REGIÓN -------------*/
revisionRegion:
		mov r10, r1					/*Guardamos los candidatos actuales en R10*/
		POP {r0-r2}					/*Restauramos la fila, columna y @ de inicio del sudoku en r1, r2 y r0 respectivamente*/
		PUSH {r0}					/*Se guarda la @ de inicio del sudoku*/

		/* Cálculo de la primera celda de la región */
		cmp r1, #6					/*Se calcula si la primera fila del cuadrante es 0,3 o 6*/
		blt fmenor6					/*Si es menor de 6 solo puede ser la fila 0 o la 3*/
		mov r3, #6
		b recCol
fmenor6:cmp r1, #3					/*Si es menor de 3 solo puede ser la fila 0*/
		blt fmenor3
		mov r3, #3
		b recCol
fmenor3:mov r3, #0					/*r3= FilaInicio */

recCol: cmp r2, #6					/*Se calcula si la primera columna del cuadrante es 0,3 o 6*/
		blt cmenor6					/*Si es menor de 6 solo puede ser la columna 0 o la 3*/
		mov r4, #6
		b finrev
cmenor6:cmp r2, #3
		blt cmenor3					/*Si es menor de 3 solo puede ser la columna 0*/
		mov r4, #3
		b finrev
cmenor3:mov r4, #0					/*r4= ColumnaInicio */

		/* Fin del Cálculo de la primera celda de la región */
finrev:	mov r5, r3					/* filaX */
		mov r6, r4					/* columnaX */

		/*FilaInicio=r3 ColumnaInicio=r4*/
		mov r7, r3
		LSL r7, #5					/*Se calcula el desplazamiento por filas*/
		add r0, r0, r7				/*Se aplica al puntero*/
		mov r7, r4
		LSL r7, #1					/*Se calcula el desplazamiento por columnas*/
		add r0, r0, r7				/*Se aplica al puntero*/
		mov r3, #0					/*r3=iterador (se reinicia a 0)*/

whileRegion:
		cmp r3, #9					/*Se itera mientras r3 distinto de 9, siendo 9 el número de celdas a revisar*/
		bne revisarCandidatoRegion
		POP {r0}					/*Obtenemos @sudoku*/
		LSL r1, #5					/*Nos situamos en la fila correcta*/
		add r0, r0, r1
		LSL r2, #1					/*Nos situamos en la columna correcta*/
		add r0, r0, r2
		mov r6, r10					/*Sacamos @ del dato del sudoku a actualizar candidatos:*/
		ldrh r5, [r0]
		LSR r5, r5, #9
		LSL r5, r5, #9
		add r6, r5, r6
		mov r1, r6
		LSR r6, r6, #12				/*Se saca el valor de la celda*/
		cmp r6, #0					/*comprueba si tiene valor o no*/
		beq false					/*No tiene, y devuelve FALSE*/

		mov r7, r10					/*Candidatos de la celda*/
		sub r5, r6, #1				/*Se resta uno al valor de la celda*/
		mov r4, #1					/*Ponemos el 1 en r4 para comprobar si esta en candidatos*/
		LSL r4, r4, r5				/*Se desplaza el 1*/
		and r4, r4, r7				/*Si tiene valor 1 es que el valor estaba en candidatos*/
		cmp r4, #0
		beq rechazamos

		ldr r2, =0xFBFF				/*Eliminamos el bit error*/
		and r1, r2
		strh r1, [r0]				/*Actualizamos los candidatos en memoria*/
		mov r0, #1					/*Devuelve TRUE porque valor de la celda es distinto de 0*/
		POP {r4-r7}					/*Restaura el valor de los registros usados*/
		POP {r3}
		bx r3						/*FINAL DE LA EJECUCION DE LA FUNCIÓN: Salta a la función de llamada*/

rechazamos:
		ldr r2, =0x400
		add r1, r1, r2				/*Añadimos bit error*/
		strh r1, [r0]				/*Actualizamos los candidatos en memoria*/
		mov r0, #0					/*Devuelve -1 porque da error, el valor de la celda no esta en candidatos*/
		sub r0, r0, #1
		POP {r4-r7}					/*Restaura valor de los registros usados*/
		POP {r3}
		bx r3						/*FINAL DE LA EJECUCION DE LA FUNCIÓN: Salta a la función de llamada*/

false:
		ldr r2, =0xFBFF				/*Eliminamos bit error*/
		and r1, r2
		strh r1, [r0]				/*Actualizamos los candidatos en memoria*/
		mov r0, #0					/*Devuelve FALSE porque valor de la celda es = 0*/
		POP {r4-r7}					/*Restaura el valor de los registros usados*/
		POP {r3}
		bx r3						/*FINAL DE LA EJECUCION DE LA FUNCIÓN: Salta a la función de llamada*/

revisarCandidatoRegion:
		mov r8, r4					/*Guardamos r4 ya que se utiliza poco*/
		cmp r1, r5					/*Comparamos filaX con fila*/
		beq avanzaEnRegion
		cmp r2, r6					/*Comparamos columnaX con columna*/
		beq avanzaEnRegion
		ldrh r4, [r0]				/*Sacamos el dato*/
		ldr r7, =0xF000				/*Se carga la máscara que permite averiguar si la celda esta vacía*/
		TST r4, r7					/*Se compara la celda con la máscara*/
		beq avanzaEnRegion			/*Si es vacia, se comprueba el siguiente, si no lo es, se elimina el candidato*/

		/* Eliminación de un candidato: */
		mov r9, r5					/*Se guarda r5 para poder utilizar el registro*/
		mov r7, r10					/*r7 = candidatos actuales*/
		LSR r4, r4, #12				/*Se saca el valor de la celda actual*/
		sub r4, r4, #1				/*Se resta uno al candidato para el desplazamiento*/
		mov r5, #1					/*Se pone el 1 y se desplaza hasta la posición a borrar*/
		LSL r5, r5, r4				/*Se desplaza el 1 para eliminarlo*/
		mvn r5, r5					/*Se niegan los bits*/
		and r7, r5					/*Se pone a 0 el bit que corresponde al candidato eliminado*/
		mov r10, r7					/*Guaramos r10 los candidatos actualizados*/
		mov r5, r9					/*Se restaura r5 con el valor de "fila"*/

avanzaEnRegion:
		add r3, r3, #1				/*i++*/
		mov r4, r8					/*Se restaura el valor de r4*/
		add r7, r4, #2				/*temp = ColumnaInicio+2*/
		cmp r6, r7					/*¿columnaX==ColumnaInicio+2?*/
		bne cambioColumnaEnRegion	/*Si no ha llegado al límite de la región, pasa a la siguiente columna*/
		mov r6, r4					/*Si ha llegado, se recoloca la columna y cambia de línea: ColumnaX=ColumnaInicio*/
		add r5, r5, #1				/*filaX++*/
		add r0, r0, #28				/*Cambio de fila dentro de la región*/
		b whileRegion
cambioColumnaEnRegion:
		add r0, r0, #2				/*Cambio de columna dentro del cuadrante*/
		add r6, r6, #1				/*ColumnaX++*/
		b whileRegion

fin: B .
	.end
