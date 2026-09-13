AREA codigo, CODE, READONLY
EXPORT  dense_layer_q12_ARM_C
IMPORT  neuron_q12_C ;Esto cambiar a ARM y hacer neuron_q12_ARM? Por lo demas es igual
	
dense_layer_q12_ARM_C							
	PUSH {r4-r11, lr}		; Añadiremos en pila desde r4 a r7 los valores de los registros parámetro (r0-r3), el resto de parámetros que no caben entre r0 y r3 y las variables checksum y o 
	mov r4, r0              ; input
    mov r5, r1          	; weights
    mov r6, r2              ; bias
    mov r7, r3              ; output
    ldr r8, [sp, #36]       ; input_size
    ldr r9, [sp, #40]       ; output_size
    mov r10, #0             ; o = 0
    mov r11, #0             ; checksum = 0
buc	
	cmp r9, r10
	beq finbuc
	mul r12, r10, r8        ; r12 = o * input_size   (elementos)
    mov r12, r12, LSL #1    ; r12 = o * input_size * 2  (bytes, los datos son de "half word")
    add r1,  r5, r12        ; r1 = weights_o  preparamos el peso para neuron
	mov r12, r10, LSL #1    ; r12 = o*2 (bytes) — reutilizamos r12
    ldrh r3, [r6, r12]      ; r3 = bias[o], para neuron
	ldr r0, [sp, #48]		; r0=clamp_max
	str r0, [sp, #-4]!		; clamp_max se almacena en sp, sp empuja a la direccion original de sp
	ldr r0, [sp, #44]		; r0=clamp_min
	str r0, [sp, #-4]!		; clamp_min se almacena en sp, sp empuja a la direccion original de sp
	mov r0, r4				; r0=input (primer argumento de neuron)
	mov r2, r8				; r2=input_size (tercer argumento de neuron)
	bl neuron_q12_C
	add sp, sp, #8			;"Desapilamos" los clamps de neuron
	mov r12, r10, LSL #1
	strh r0, [r7, r12]		; output[o] = y
	add r11, r11, LSL #5	; checksum=checksum*33u (añadir a memoria que hay otra solucion (mul r11, #33, r11)
	and r0, r0, #0xFFFF		; (uint16_t)y, añadir a memoria, hace un and y mantiene las "direcciones bajas" dentro del registro dejando a 0 las direcciones altas, forzando a que esté a 16 bits (media palabra)
	add r11, r11, r0
	add r10, r10, #1
	b buc
finbuc
	mov r0, r11						; Devuelve el resultado de checksum en r0, como es un argumento a punto de salir se puede machacar el resultado de r0
	POP {r4-r11, pc}
	
	END