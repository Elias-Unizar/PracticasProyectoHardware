;===============================
;	int16_t neuron_q12_THB{}
;===============================

				AREA thumbNeuron, CODE, READONLY
				THUMB
neuron_q12_THB	
				PUSH{r4-r7} ; -----PILAAAAA
				MOV r7, r13
				LSLS r2, r2, #1 ;Usado para incrementar vector en thumb, ademas de ser el maximo * 2(para no tener un incrementador y un cintador de iteraciones)
				ADDS r7, r7, #16
				MOVS r4, #0
				;MOV r8, r6
				;MOV r9, r7
				;r0 = input, r1 = weights, r2 = n, r3 = bias, r8 = min, r9 = max
				;min, max y n solo se usaran para comparaciones, se pueden mover a registros high, y dejar los low disponibles para operar
				LSLS r3, r3, #12 ; bias no se vuelve a utilizar, r3 pasa a ser acc
bucMul
				CMP r4, r2
				BGE finBucMul
				
				LDRSH r5, [r0, r4]
				LDRSH r6, [r1, r4]
				ADDS r4, #2
				MULS r5, r6, r5
				ADDS r3, r3, r5
				
				B bucMul
finBucMul		
				LDMIA r7!, {r5-r6}
				ASRS r3,r3, #12
				;-----CLAMP INLINE
				CMP r3, r5
				BLT retLow
				CMP r3, r6
				BGT retHigh
				;--Sin saturar
				B finalClamp
retLow
				MOVS r3, r5
				B finalClamp
				;---Return low
retHigh
				MOVS r3, r6
				;---Return high
finalClamp
				MOVS r0, r3
				POP{r4-r7}
				BX LR
				
dense_layer_q12_THB
				PUSH{LR}
				PUSH{r4-r7}
				MOV r4, r8
				MOV r5, r9
				MOV r6, r10
				MOV r7, r11
				PUSH{r4-r7}
				MOV r4, r12
				PUSH{r4}
				MOV r7, r13
				MOV r8, r1 ;weigth
				MOV r9, r2 ;bias
				ADDS r7, r7, #40 ;fp casero
				MOV r10, r3 ;output
				MOVS r1, #2 ; incrementador para la o y que sirva de offset
				LDMIA r7,{r4-r7}
				MOV r11, r1
				LSLS r3, r5, #1
				PUSH{r6-r7}
				MOV r12, r3
				MOVS r5, #0
				MOVS r6, #0
				MOVS r2, r4
				;-----r0 = input, r1 = futuro weigth_o, r2 = input_size, r3 = futuro bias[o], r4 = libre, r5 = o, 
				;-----r6 = checksum, r7 = libre, r8 = matriz weigth, r9 = vector bias, r10 = vector output, 
				;-----r11 = #2, r12 = outputSize*2

bucFor
				CMP r5, r12
				BGE finBucFor
				
				MOVS r1, r2
				MOV r3, r9; El vector bias se auto incrementa junto con el vector output, y la o
				MULS r1, r5, r1 ; la o al incrementarse de 2 en 2, no hace falta alinear al tama�o del dato, ya que tambien es de 2B
				LDRH r3, [r3]     
				ADD r1, r8; direccion del vector weigths correspondiente a "o"				
				LSLS  r3, r3, #16;para emular el LDRSH
				ASRS  r3, r3, #16
				MOVS r4, r0
				MOVS r7, r2
				BL neuron_q12_THB;--r0 = y; r4 y r7 no tocarlos; solo tocar r0 y r2
				MOV r1, r10
				LSLS r2, r6, #5
				ADD r5, r11
				STRH r0, [r1]
				LSLS r0, r0, #4
				ADDS r6, r6, r2
				MOVS r2, r7
				LSRS r0, r0, #4
				ADD r10, r11
				ADD r9, r11
				ADDS r6, r6, r0			
				MOVS r0, r4				

				B bucFor
finBucFor
				ADD SP, #8;desapilamos los 2 parametros clampmin y max, que pasabamos a la neurona, fuera del bucle para optimizar
				MOVS r0, r6
				POP{r4}
				MOV r12, r4
				POP{r4-r7}
				MOV r8, r4
				MOV r9, r5
				MOV r10, r6
				MOV r11, r7
				POP{r4-r7}
				POP{r1}
				BX r1
				
				
				EXPORT dense_layer_q12_THB
				EXPORT neuron_q12_THB
				
				END				
