// Proyecto Hardware 2026 — Capa neuronal fully-connected en Q12 con AAPCS
// Referencia C + verificación + main de prueba.
// ARM7TDMI (ARMv4T) / Keil µVision.
// v0.9 - Enrique Torres && chatGPT

// ======================================================================
// INCLUDES Y DEFINICIONES
// ======================================================================
#include "dense_q12.h"
#include <stdint.h>

// ======================================================================
// REFERENCIA EN C
// ======================================================================

/* ============================================================
   Neurona fully-connected en Q12
   ============================================================ */

int16_t neuron_q12_C(
    const int16_t *input,
    const int16_t *weights,
    uint16_t n,
    int16_t bias_q12,
    int16_t clamp_min,
    int16_t clamp_max) {

    // El bias está en Q12; el acumulador trabaja en Q24.
    // Por tanto, bias_q12 se desplaza 12 bits a la izquierda.
    int32_t acc = ((int32_t)bias_q12) << Q_SHIFT;

    for (uint16_t i = 0; i < n; ++i) {
        // input[i] y weights[i] están en Q12.
        // Su producto queda en Q24 y se acumula directamente.
        acc += (int32_t)input[i] * (int32_t)weights[i];
    }

    // Volvemos de Q24 a Q12.
    // En ARM esto corresponde naturalmente a un desplazamiento aritmético ASR #12.
    int32_t result_q12 = acc >> Q_SHIFT;

    // Activación/saturación final.
    return clamp_i16_q12(result_q12, clamp_min, clamp_max);
}

/* ============================================================
   Capa fully-connected completa en Q12
   ============================================================ */

uint32_t dense_layer_q12_C(
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
}

/* ============================================================
   Capa C llamando a neurona ARM/Thumb
   ============================================================ */

uint32_t dense_layer_q12_C_ARM(
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

        int16_t y = neuron_q12_ARM(
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
}

uint32_t dense_layer_q12_C_THB(
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

        int16_t y = neuron_q12_THB(
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
}

// ======================================================================
// VERIFICACIÓN AUTOMÁTICA
// ======================================================================


/**
 * @brief   Ejecuta C, C_ARM, C_THB, ARM_C, ARM y THB y verifica resultados.
 *
 * @param   input        Vector de entrada Q12.
 * @param   weights      Matriz de pesos Q12.
 * @param   bias         Vector de bias Q12.
 * @param   resultado    Se deja aquí la salida de la versión C.
 * @param   input_size   Número de entradas.
 * @param   output_size  Número de salidas/neuronas.
 * @param   clamp_min    Límite inferior Q12.
 * @param   clamp_max    Límite superior Q12.
 * @return  1 si todos devuelven el mismo checksum y el mismo vector; 0 si difieren.
 */
uint8_t dense_q12_verificar(
    const int16_t *input,
    const int16_t *weights,
    const int16_t *bias,
    int16_t *resultado,
    uint16_t input_size,
    uint16_t output_size,
    int16_t clamp_min,
    int16_t clamp_max) {

    static int16_t out_C     [OUTPUT_SIZE];
    static int16_t out_C_ARM [OUTPUT_SIZE];
    static int16_t out_C_THB [OUTPUT_SIZE];
    static int16_t out_ARM_C [OUTPUT_SIZE];
    static int16_t out_ARM   [OUTPUT_SIZE];
    static int16_t out_ARM_OPT   [OUTPUT_SIZE];
    static int16_t out_THB   [OUTPUT_SIZE];

    uint32_t chk_C = dense_layer_q12_C(
        input, weights, bias, out_C,
        input_size, output_size, clamp_min, clamp_max);

    uint32_t chk_C_ARM = dense_layer_q12_C_ARM(
        input, weights, bias, out_C_ARM,
        input_size, output_size, clamp_min, clamp_max);

    uint32_t chk_C_THB = dense_layer_q12_C_THB(
        input, weights, bias, out_C_THB,
        input_size, output_size, clamp_min, clamp_max);

    uint32_t chk_ARM_C = dense_layer_q12_ARM_C(
        input, weights, bias, out_ARM_C,
        input_size, output_size, clamp_min, clamp_max);

		uint32_t chk_ARM_OPT = dense_layer_q12_ARM_OPT( 
				input, weights, bias, out_ARM_OPT,
				input_size, output_size, clamp_min, clamp_max);

    uint32_t chk_ARM = dense_layer_q12_ARM(
        input, weights, bias, out_ARM,
        input_size, output_size, clamp_min, clamp_max);

    uint32_t chk_THB = dense_layer_q12_THB(
        input, weights, bias, out_THB,
        input_size, output_size, clamp_min, clamp_max);

    for (uint16_t i = 0; i < output_size; ++i) {
        resultado[i] = out_C[i];
    }

    if (!(chk_C == chk_C_ARM && chk_C == chk_C_THB &&
          chk_C == chk_ARM_C && chk_C == chk_ARM && 
					chk_C == chk_THB && chk_C == chk_ARM_OPT)) {
        return 0;
    }

    if (!vectores_iguales_i16(out_C, out_C_ARM, output_size)) return 0;
    if (!vectores_iguales_i16(out_C, out_C_THB, output_size)) return 0;
    if (!vectores_iguales_i16(out_C, out_ARM_C, output_size)) return 0;
    if (!vectores_iguales_i16(out_C, out_ARM,   output_size)) return 0;
    if (!vectores_iguales_i16(out_C, out_THB,   output_size)) return 0;
    if (!vectores_iguales_i16(out_C, out_ARM_OPT,   output_size)) return 0;

    return 1;
}

static uint32_t n            = 0;
static uint32_t pasados      = 0;
static uint32_t fallados     = 0;
static uint32_t primer_fallo = 0;
static int16_t resultado[OUTPUT_SIZE] __attribute__((aligned(8)));

void probarAuto(const int16_t *input, const int16_t *weights, const int16_t *bias){                                                                                                                        \
        n++;                                                                 
        if (dense_q12_verificar(input, weights, bias, resultado, INPUT_SIZE,             
																OUTPUT_SIZE, CLAMP_MIN_Q12, CLAMP_MAX_Q12)) pasados++;       
        else { fallados++; if (!primer_fallo) primer_fallo = n; }            
}


/* ============================================================
   MAIN de prueba
   ============================================================ */
int main(void) {
    // Vector de entrada de prueba, en Q12.
    // Se puede visualizar en Keil como enteros int16_t.
    // Ejemplos:
    //   Q_ONE      = 4096  ->  1.0
    //   Q_ONE / 2  = 2048  ->  0.5
    //  -Q_ONE / 4  = -1024 -> -0.25
    static const int16_t input[INPUT_SIZE] __attribute__((aligned(8))) = {
         Q_ONE,        //  1.00
         Q_ONE / 2,    //  0.50
        -Q_ONE / 4,    // -0.25
         3 * Q_ONE / 4,//  0.75
         0,            //  0.00
        -Q_ONE / 2,    // -0.50
         Q_ONE / 4,    //  0.25
         Q_ONE / 8     //  0.125
    };

    // Matriz de pesos: OUTPUT_SIZE filas x INPUT_SIZE columnas, en Q12.
    // Cada fila corresponde a una neurona de salida.
    static const int16_t weights[OUTPUT_SIZE * INPUT_SIZE] __attribute__((aligned(8))) = {
        // Neurona 0
         Q_ONE / 2,   Q_ONE / 2,  -Q_ONE / 4,   Q_ONE / 4,
         0,          -Q_ONE / 8,   Q_ONE / 4,   0,

        // Neurona 1
        -Q_ONE / 2,   Q_ONE / 4,   Q_ONE / 2,  -Q_ONE / 4,
         Q_ONE / 8,   Q_ONE / 4,  -Q_ONE / 8,   Q_ONE / 8,

        // Neurona 2
         Q_ONE,      -Q_ONE,       Q_ONE / 2,   0,
         0,           Q_ONE / 4,   Q_ONE / 4,  -Q_ONE / 2,

        // Neurona 3
        -Q_ONE / 4,   Q_ONE / 4,   Q_ONE / 4,   Q_ONE / 4,
        -Q_ONE / 2,   Q_ONE / 2,   0,           Q_ONE / 8,

        // Neurona 4
         Q_ONE / 8,  -Q_ONE / 8,   Q_ONE / 8,  -Q_ONE / 8,
         Q_ONE / 2,   Q_ONE / 2,   Q_ONE / 4,   Q_ONE / 4
    };

    // Un bias por neurona, también en Q12 e int16_t.
    static const int16_t bias[OUTPUT_SIZE] __attribute__((aligned(8))) = {
         0,
         Q_ONE / 8,
        -Q_ONE / 4,
         Q_ONE / 4,
         0
    };
    // ------------------------------------------------------------------
    // DATOS NUEVOS Y PROBAR AUTO
    // ------------------------------------------------------------------
    // Entradas
    static const int16_t input_unos[INPUT_SIZE] __attribute__((aligned(8))) = {
        Q_ONE, Q_ONE, Q_ONE, Q_ONE, Q_ONE, Q_ONE, Q_ONE, Q_ONE          // todo 1.0
    };
    static const int16_t input_extremos[INPUT_SIZE] __attribute__((aligned(8))) = {
         2 * Q_ONE, -2 * Q_ONE,  2 * Q_ONE, -2 * Q_ONE,
         2 * Q_ONE, -2 * Q_ONE,  2 * Q_ONE, -2 * Q_ONE                  // +-2.0 alternado
    };
    static const int16_t input_minimos[INPUT_SIZE] __attribute__((aligned(8))) = {
        1, 1, 1, 1, 1, 1, 1, 1                                          // 1 LSB
    };

    // Pesos
    static const int16_t weights_ident[OUTPUT_SIZE * INPUT_SIZE] __attribute__((aligned(8))) = {
        Q_ONE, 0, 0, 0, 0, 0, 0, 0,                                     // salida[o] = entrada[o]
        0, Q_ONE, 0, 0, 0, 0, 0, 0,
        0, 0, Q_ONE, 0, 0, 0, 0, 0,
        0, 0, 0, Q_ONE, 0, 0, 0, 0,
        0, 0, 0, 0, Q_ONE, 0, 0, 0
    };
    static const int16_t weights_grandes[OUTPUT_SIZE * INPUT_SIZE] __attribute__((aligned(8))) = {
        2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE,
        2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE,
        2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE,
        2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE,
        2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE, 2 * Q_ONE
    };
    static const int16_t weights_menos1[OUTPUT_SIZE * INPUT_SIZE] __attribute__((aligned(8))) = {
        -1, -1, -1, -1, -1, -1, -1, -1,                                 // 1 LSB negativo
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1
    };

    // Bias
    static const int16_t bias_extremos[OUTPUT_SIZE] __attribute__((aligned(8))) = {
        32767, -32768, 32767, -32768, 0
    };
    static const int16_t bias_medio[OUTPUT_SIZE] __attribute__((aligned(8))) = {
        Q_ONE, -Q_ONE, Q_ONE / 2, -Q_ONE / 2, 0
    };
		
    probarAuto(input,           weights,        bias);          // 1  original
    probarAuto(input_unos,      weights,        bias);          // 2  entrada nueva
    probarAuto(input_extremos,  weights,        bias_extremos); // 3  entrada y bias extremos
    probarAuto(input,           weights_ident,  bias);          // 4  identidad
    probarAuto(input_unos,      weights_ident,  bias_medio);    // 5  identidad + bias
    probarAuto(input_unos,      weights_grandes, bias);         // 6  saturación superior
    probarAuto(input_extremos,  weights_grandes, bias_extremos);// 7  pesos grandes + extremos
    probarAuto(input_minimos,   weights_menos1, bias);          // 8  -1 LSB (comprueba ASR)
    probarAuto(input_minimos,   weights_grandes, bias_medio);   // 9  valores pequeños
    probarAuto(input_extremos,  weights_menos1, bias_medio);    // 10 mezcla

		
    // Punto de parada para depuración: inspeccionar 'pasados', 'fallados',
    // 'primer_fallo' y 'resultado' (salida C de la última prueba).
    while (1) { /* no retornar */ }
	}
