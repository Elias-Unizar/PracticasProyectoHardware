;===============================
;	int16_t neuron_q12_THB{}
;===============================

				AREA thumbNeuron, CODE, READONLY
				THUMB
neuron_q12_THB	
				PUSH{r4-r7} ; -----PILAAAAA
				MOV r7, r13
				LSLS r2, r2, #1
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
				
	
const int16_t *input,
    const int16_t *weights,
    const int16_t *bias,
    int16_t *output,
    uint16_t input_size,
    uint16_t output_size,
    int16_t clamp_min,
    int16_t clamp_max) {

    uint32_t checksum = 0;

    for (uint16_t o = 0; o < output_size; ++o) {
        const int16_t *weights_o = &weights[o * input_size];

        int16_t y = neuron_q12_C(
            input,
            weights_o,
            input_size,
            bias[o],
            clamp_min,
            clamp_max
        );

        output[o] = y;
        checksum = checksum * 33u + (uint16_t)y;
    }

    return checksum;
				
				EXPORT dense_layer_q12_THB
				EXPORT neuron_q12_THB
				
				END