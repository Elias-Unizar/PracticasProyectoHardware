	AREA codigoOPT, CODE, READONLY
	EXPORT  dense_layer_q12_ARM_OPT
	
dense_layer_q12_ARM_OPT
	STMDB r13!, {r4-r11, LR}; Añadiremos en pila desde r4 a r7 los valores de los registros parámetro (r0-r3), el resto de parámetros que no caben entre r0 y r3 y las variables checksum y o 
	ADD r13, r13, #36
	mov r10, r1          	; weights
    mov r11, r2             ; bias
    LDMIA SP!, {r2, r4}; inputSize, outputSize
	MOV r12, r3				;resultado
    mov r9, #0             ; checksum = 0
	;r0 = input; r1 = libre; r2 = INPUT SIZE; r3 = libre; r4 = outputSize; r5-r8 = libre; r9 = checksum; 
	;r10 = weigths; r11 = bias; r12 = output; r13 = SP; r14 = libre;
	CMP r4, #0				;comprobacion trivial, pero no está mal seguir el codigo en C
	BLE finDense
iniDense
	LDRSH r3, [r11], #2;Cargamos el bias, siempre de #2 en #2, nos da igual mantener el vector entre neuronas
	CMP r2, #0;Compramos igual que con el iterador de Dense, pero ahora es INPUT_SIZE
	MOV r1, r2;Como INPUT_SIZE lo queremos mantener entre iteraciones de neuronas, hay que guardarlo, y SP esta jodido ahora mismo
	BLE finNeuron
	LSL r3, r3, #12; Bias << QSHIFT
iniNeuron
	LDR r5, [r0], #4;Cargamos dos valores de golpe de input(luego restauramos input)
	LDR r7, [r10], #4;Cargamos dos valores de golpe de weigths(se va acumulando el offset)
	;Libres hasta este punto, r6, r8, pero r6 y r8 ahora se rellenaran
	MOV r6, r5, ASR #16;Separamos los dos short int de input (1/3)
	LSL r5, r5, #16;Separamos los dos short int de input (2/3)
	MOV r8, r7, ASR #16;Separamos los dos short int de weigths (1/3)
	LSL r7, r7, #16;Separamos los dos short int de weigths (2/3)
	ASR r5, r5, #16;Separamos los dos short int de input (3/3)
	MLA r3, r6, r8, r3;input[i+1]*weigths[i+1] + acc
	ASR r7, r7, #16;Separamos los dos short int de weigths (3/3)
	SUBS r2, r2, #2;MLA no actualiza flags, asi que esto todavia es util, restar a INPUT-SIZE 2
	MLA r3, r5, r7, r3;input[i]*weigths[i] + acc
	BHI iniNeuron;Salta al inicio si INPUT_Size > 0
finNeuron
	LDMIA SP, {r5, r6}; min y max
	SUB r0, r0, r1, LSL #1;Recuperamos direccion inicial de INPUT, en funcion del numero de veces que hemos incrementado #2
	MOV r2, r1;Recuperamos INPUT_SIZE
	ASR r3, r3, #12
	
	cmp     r3, r5				;r3 es el acc, r5 es el min
    movlt   r3, r5          ; if (x < clamp_min) x = clamp_min
    cmpgt   r3, r6				; y r6 es el max
    movgt   r3, r6          ; if (x > clamp_max) x = clamp_max
	
	;ahora ha acabado el codigo de la neurona, y continua el dense_layer
	;en r3 tenemos el resultado
	;r0 = vector input intacto; r1 = ahora es inutil; r2 = INPUT_SIZE; r3 = resultadoNeurona
	;r4 = OUTPUT_SIZE(iterador de dense_layer); r5-r8 = libre; r9 = checksum
	;r10 = weigths(offset acumulado); r11 = bias(offset acumulado con cada DENSELAYER)
	;r12 = resultado(offset acumulado con cada DENSELAYER); r13 = clamp_min; r14 = clamp_max
	STRH r3, [r12], #2
	LSL r1, r9, #5; 32*Checksum
	ADD r1, r1, r9 ;32*checksum + checksum
	SUBS r4, r4, #1; aprovechamos el salto condicional con el SUBS para ahorrar un CMP, aunque el salto sera en mayoria tomado
	ADD r9, r1, r3; en checksum = 33*checksum + y
	BHI iniDense
finDense
	SUB SP, SP, #44
	MOV r0, r9; movemos el checksum a r0
	LDMIA SP!, {r4-r11, LR}
	BX LR
		
	END
	