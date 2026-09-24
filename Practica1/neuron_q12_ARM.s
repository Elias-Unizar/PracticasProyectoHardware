; Traducción de neuron_q12_C, recibe como parámetros const int16_t *input, const int16_t *weights, const int16_t *bias, int16_t *output, uint16_t input_size, uint16_t output_size, int16_t clamp_min, int16_t clamp_max
; al igual que la función original en C, solo que en formato "half word" para datos de 16 bits (2 bytes).
; Los primeros 4 argumentos se guardan en los registros r0-r3, el resto de parámetros se guardan en pila.
; Registros utilizados: r0-r3, registros de tipo "argumento" los cuales pueden ser tanto los argumentos const int16_t *input, const int16_t *weights, const int16_t *bias, int16_t *output
; como result en el caso de r0 o los clamps en los casos de r1 y r2, r4 es una copia de input, r5 es una copia de weights, r6 de n y r7 de bias, r8 es la variable acc,
; r9 es la variable i del bucle, r10 es una variable para alinear las direcciones de input a 2 bytes (halfword), r13 (sp) es el puntero a la cima de la pila, r14 (lr) guarda la dirección de retorno de la subrutina.

	AREA codigo, CODE, READONLY
	EXPORT neuron_q12_ARM
	
neuron_q12_ARM
	PUSH {r4-r10, lr}
	mov r4, r0 				; input
	mov r5, r1 				; weights
	mov r6, r2 				; n
	mov r7, r3 				; bias
	mov r8, r7, LSL #12		; int32_t acc = ((int32_t)bias_q12) << Q_SHIFT, donde Q_SHIFT tiene un valor de 12, es decir, lo convertimos en un numero Q24
	mov r9, #0				; i=0 del bucle
buc
	cmp r9, r6
	beq finbuc
	mov r10, r9, LSL #1 	; Alineamos a palabras de 2 bytes (ya que input y weitghts son uint16_t)
	ldrsh r0, [r4, r10]		; input[i]
	ldrsh r1, [r5, r12]		; weights[i]
	mla r8, r0, r1, r8		; acc += (int32_t)input[i] * (int32_t)weights[i], añadir en memoria, porque se puede disgregar en un mul y un add
	add r9, r9, #1			; i++
	b buc
finbuc
	mov r0, r8, ASR #12		; int32_t result_q12 = acc >> Q_SHIFT, guardamos el resultado de la acumulación y lo volvemos a convertir en un Q12
	ldr r1, [sp, #36]		; r1 recibe el parametro clamp_min
	ldr r2, [sp, #40]		; r2 recibe el parametro clamp_max
							; No se llama a la función de saturación ya que la función de saturación es un static y no se pueden llamar desde otro fichero (static otorga encapsulamiento)
	cmp     r0, r1
    movlt   r0, r1          ; if (x < clamp_min) x = clamp_min
    cmp     r0, r2
    movgt   r0, r2          ; if (x > clamp_max) x = clamp_max
	POP {r4-r10, pc}
	END
	
	
	
