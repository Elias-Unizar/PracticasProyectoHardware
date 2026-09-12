;Lo de import aún no lo he hecho, esta tarde me pongo con ello y lo soluciono, si lo haces tú, avisa


AREA codigo, CODE, READWRITE ; área de código
	ENTRY
							; Los argumentos tomarán los registros de r0 a r7 (8 argumentos de la función, no meter en pila)
	ldr r0, 				; Mover dirección input
	ldr r1, 				; Mover dirección pesos
	ldr r2, 				; Mover dirección bias
	ldr r3, 				; Mover dirección output
	ldr r4, 				; Mover tamaño input
	ldr r5, 				; Mover tamaño output
	ldr r6, 				; Mover clamp_min
	ldr r7, 				; Mover clamp_max
	bl dense_layer			;Llevamos a subrutina
	
	END
	
dense_layer
	PUSH {r8-r11, lr}		;Metemos en pila las variables para checksum (r8), o (r9),	"y" (r10) y weights_o (r11)
	mov r8, #0				;checksum=0
	mov r9, #0				; o=0
buc	
	cmp r9, r5
	beq finbuc
	mul r10, r9, r4			;Ponemos de forma auxiliar el "offset" para cargar las distintas filas de la matriz de pesos
	ldr r10, [r1, r10]		;Cargamos la direccion inicial de pesos
							; Hacer llamada a neuron_12_C, no sé como hacerlo xd, cargar el resultado en r11
	str r11, [r3, r9]		; Metemos el resultado de y en el output correspondiente
	mul r8, #33, r8			;Multiplicamos por 33, quizá esta linea hay que corregir
	add r8, r8, r11			;sumamos para tener el resultado parcial de checksum
	b buc
fin buc
	mov r0, r8						; Devuelve el resultado de checksum en r0, como es un argumento a punto de salir se puede machacar el resultado de r0
	POP {r8-r11, pc}
	
	
