.global start
.arm
.data
.ltorg /*Garantiza la alineación*/
.text

.global Data_abort_test

Data_abort_test:
	PUSH {lr}				/* Guarda la dirección desde la que se ha saltado */
	ldr r0, =0x4001			/* Apuntamos a una @memoria impar */
	mov r1, #2				/* r1 = 2 */
	str r2, [r0]			/* Guardamos en @r2 el valor de r1 */
	POP {lr}				/* Direccion de retorno */

fin: B .
	.end
