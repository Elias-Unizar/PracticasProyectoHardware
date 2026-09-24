;===============================
;	int16_t neuron_q12_THB{}
;===============================
				AREA thumbNeuron, CODE, READONLY
				PRESERVE8
				IMPORT neuron_q12_THB
dense_layer_q12_THB ;entrada a la funcion, como ARM para hacer un push claro y que consume menos memoria y tiempo
				PUSH{r4-r11, lr}
				SUB r13, r13, #4 ;para preservar el ATCPS, hay que dejar pila alineada a 8 cuando se llamen a funciones(luego se apilan los 2 valores de neuron_THB)
				ADR r4, saltoThumb+1
				ADD r7, r13, #40 ;Para luego cargar con facilidad los parametros que hace falta leer de la pila de llamada a la funcion
				BX r4
				THUMB
saltoThumb
				;movemos a registros altos las direcciones iniciales de vectores, ya que se le pueden sumar otros registros para calcular la siguiente 
				;operacion, y asi moverlos a un registro bajo que machaque su valor trayendo el valor que nos interesa del vector
				MOV r8, r1 ;weigth
				MOV r9, r3 ;output
				LDMIA r7,{r4-r7} ; cargamos los 4 parametros de la funcion que estaban en la pila
				LSLS r5, r5, #1 ; outputSize * 2
				PUSH{r6-r7}; apilamos los clamps para llamar a neuron
				MOV r11, r5; movemos el outputsize*2 a r11, registro alto
				MOVS r5, #0
				MOVS r6, #0
				MOVS r7, r2; movemos a r7 el vector bias
				MOVS r2, r4; movemos a r2 el input_size
				
				;-----r0 = input, r1 = futuro weigth_o, r2 = input_size, r3 = futuro bias[o], r4 = libre, r5 = o, 
				;-----r6 = checksum, r7 = vector bias, r8 = matriz weigth, r9 = vector output, r10 = libre, 
				;-----r11 = outputSize*2

bucFor
				CMP r5, r11 ; for o < outputSize
				BGE finBucFor
				
				MOVS r1, r2; para hacer el inputSize*o, copiamos inputSize aqui
				MULS r1, r5, r1 ; se realiza el inputSize*o, luego habria que realizar un *2 para obtener la direccion alineada a shor int, pero la o ya esta multiplicada por 2 de base
				LDRSH r3, [r7, r5]; vector bias mas offset de la o
				ADD r1, r8; direccion del vector weigths correspondiente a "o"
				MOV r10, r0; movemos la direccion de input a r4 para que se guarde el estado
				MOVS r4, r2; movemos input_size a r, para lo mismo
				BL neuron_q12_THB;--r0 = y; r4 y r7 no tocarlos; solo tocar r0 y r2
				MOV r1, r9 ;vector output, ya incrementado de 2 en 2
				LSLS r2, r6, #5 ; checksum*32 en r2
				MOVS r3, #2		;para incrementar de 2 en 2 la o, no hay registros suficientes para guardar todo bien *EXPLICAR EN LA MEMORIA
				STRH r0, [r1]	; se guarda el resultado de la "y" en el vector output
				LSLS r0, r0, #16; para pasar el valor de signed a unsigned
				ADDS r6, r6, r2; en checksum, guardamos checksum*32+checksum
				MOVS r2, r4 ;este y el r0, r10; sirven para recuperar el estado anterior a la llamada a la funcion
				LSRS r1, r0, #16;terminamos la conversion a unsigned
				MOV r0, r10
				ADDS r5, r3; la o += 2
				ADD r9, r3; el output incrementa en 2
				ADDS r6, r6, r1; checksum*33 + y	
				
				B bucFor
finBucFor
				ADD SP, #12;desapilamos los 2 parametros clampmin y max, que pasabamos a la neurona, fuera del bucle para optimizar, y el hueco de 4 para alinear
				MOVS r0, r6
				ADR r1, saltoARM
				BX r1
				ARM
saltoARM
				POP{r4-r11, PC}
				EXPORT dense_layer_q12_THB

				END				