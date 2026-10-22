/* *****************************************************************************
 * P.H.2025: TODO
 * implementacion para cumplir el hal_tiempo.h
 */

#include <nrf.h>
#include "hal_tiempo.h"

#include <stdbool.h>  // si usas bool/true/false
#include <stddef.h>   // define NULL

#define PCLK_MHZ          1u
#define TICKS_PER_US      PCLK_MHZ
#define COUNTER_BITS      32u
#define COUNTER_MAX       (0xFFFFFFFFu)

/* ---- Tick libre con T1 --------------------------------------------------- */
static volatile uint32_t s_overflows_t0 = 0;  /* cuenta desbordes de T1 */

void TIMER0_IRQHandler(void) __irq {
		volatile uint32_t dummy;  
		/* CC0 al máximo para provocar overflow controlado */
		if (NRF_TIMER0->EVENTS_COMPARE[0] == 1){
			NRF_TIMER0->EVENTS_COMPARE[0] = 0;                  /* clear CC0 int */
			s_overflows_t0++;
			dummy = NRF_TIMER0->EVENTS_COMPARE[0];
			dummy;
		}
}

/* *****************************************************************************
 * Programa un contador de tick sobre Timer1, con maxima precisión y minimas interrupciones
 */
void hal_tiempo_iniciar_tick(hal_tiempo_info_t *out_info) {
		//Si se quiere cambiar a que sea de 32 bits, con otra frecuencia etc... hacerlo antes de recabar la informacion del clock
		NRF_TIMER0->BITMODE    = 3;//0 = 16bits, 1 = 8bits, 2 = 24bits, 3 = 32bits
		out_info->counter_bits = COUNTER_BITS;
    out_info->counter_max  = COUNTER_MAX;
		NRF_TIMER0->PRESCALER  = 4 << TIMER_PRESCALER_PRESCALER_Pos; //fclock = 16MHz / 2^Prescaler ---> 16Mhz / 16
		out_info->ticks_per_us = TICKS_PER_US;
	
		s_overflows_t0 = 0;
    /* Timer1 como free-running: cuenta hasta MAX y se “resetea” por match */
		NRF_TIMER0->CC[0] = COUNTER_MAX;

	  NRF_TIMER0->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;

		// Clear the timer when COMPARE0 event is triggered
		NRF_TIMER0->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;


		NVIC_EnableIRQ(TIMER0_IRQn);
		NRF_TIMER0->TASKS_START = 1;
}

/* Lectura 64-bit sin carrera: lee TC, luego overflows, re-lee TC si hizo wrap */
uint64_t hal_tiempo_actual_tick64(void) {
    uint32_t hi1, hi2, lo2;

    hi1 = s_overflows_t0;
		NRF_TIMER0->TASKS_CAPTURE[2] = 1;
    hi2 = s_overflows_t0;
    
		if (hi2 != hi1) {
        /* ocurrió wrap entre lecturas: usar hi actualizado */
				NRF_TIMER0->TASKS_CAPTURE[2] = 1;
			  lo2 = NRF_TIMER0->CC[2];
        return (((uint64_t)hi2) * ((uint64_t)COUNTER_MAX + 1u)) + lo2;
    }
		lo2 = NRF_TIMER0->CC[2];
    return (((uint64_t)hi1) * ((uint64_t)COUNTER_MAX + 1u)) + lo2;
}

/* ***************************************************************************** */

/* ---- Reloj periódico con T0 ---------------------------------------------- */
static void (*s_cb)() = 0;		//puntero a funcion a llamar cuando salte la RSI (en modo irq)

/* *****************************************************************************
 * Timer Interrupt Service Routine
 * llama a la funcion de callbak que se ejecutara en modo irq
 */

void TIMER1_IRQHandler(void) __irq {
		volatile uint32_t dummy;  
    if (NRF_TIMER1->EVENTS_COMPARE[0]) {
									// Llamo a la función desde la RSI. Gracias a esto suponemos que pasan los 3? 5? ciclos que se exigen del timer
			NRF_TIMER1->EVENTS_COMPARE[0] = 0;
			dummy = NRF_TIMER1->EVENTS_COMPARE[0];
			dummy;
			if (s_cb) s_cb();
		}// Clear interrupt flag
}

static void hal_tiempo_periodico_config_tick(uint32_t periodo_en_tick) {
    /* Config sin arrancar */
    NRF_TIMER1->TASKS_STOP = 1;
    if (periodo_en_tick == 0) return;
    NRF_TIMER1->CC[0] = periodo_en_tick - 1u;
		NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;
		NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
		NRF_TIMER1->BITMODE = 3;
		NRF_TIMER1->PRESCALER = 4 << TIMER_PRESCALER_PRESCALER_Pos;
    NVIC_EnableIRQ(TIMER1_IRQn);
}

static void hal_tiempo_periodico_set_callback(void (*cb)()) {
    s_cb = cb;
}

void hal_tiempo_periodico_enable(bool enable) {
    if (enable) {
        NVIC_EnableIRQ(TIMER1_IRQn);
        NRF_TIMER1->TASKS_START = 1;
    } else {
        NRF_TIMER1->TASKS_STOP = 1;
        NVIC_DisableIRQ(TIMER1_IRQn);
    }
}

void hal_tiempo_reloj_periodico_tick(uint32_t periodo_en_tick, void (*funcion_callback_drv)(void)){
    /* 1) Deshabilitar antes de tocar config/callback (evita ISR en mitad de cambios) */
    hal_tiempo_periodico_enable(false);

    /* 2) Registrar callback (puede ser NULL si vamos a deshabilitar) */
    hal_tiempo_periodico_set_callback(funcion_callback_drv);

    /* 3) Configurar periodo (no arranca el temporizador) */
    hal_tiempo_periodico_config_tick(periodo_en_tick);

    /* 4) Habilitar si hay periodo válido y callback no nulo; deshabilitar en caso contrario */
    if (periodo_en_tick != 0u && funcion_callback_drv != NULL) {
        hal_tiempo_periodico_enable(true);
    } else {
        hal_tiempo_periodico_enable(false);
    }
}

