/* *****************************************************************************
 * P.H.2025: Driver/Manejador de los temporizadores
 * suministra los servicios independientemente del hardware
 *
 * usa los servicios de hal_tiempo.h: 
 */
 
#include "drv_tiempo.h"
#include "hal_tiempo.h"

static hal_tiempo_info_t infoReloj = {0, 0, 0};
/*
    uint32_t ticks_per_us;    ticks de HW por microsegundo (p.ej., 15 a 15 MHz) 
    uint8_t  counter_bits;    ancho del contador libre (p.ej., 32) 
    uint32_t counter_max;     (1u<<counter_bits)-1 
*/

/**
 * inicializa el reloj y empieza a contar
 */
bool drv_tiempo_iniciar(){
	hal_tiempo_iniciar_tick(&infoReloj);
	return true;
}

/**
 * tiempo desde que se inicio el temporizador en microsegundos
 */
Tiempo_us_t drv_tiempo_actual_us(){
	uint64_t ticksNow = hal_tiempo_actual_tick64();
	return ticksNow / infoReloj.ticks_per_us;
}

/**
 * tiempo desde que se inicio el temporizador en milisegundos
 */
Tiempo_ms_t drv_tiempo_actual_ms(){
	uint64_t ticksNow = hal_tiempo_actual_tick64();
	return ticksNow / (infoReloj.ticks_per_us * 1000);
}

/**
 * retardo: esperar un cierto tiempo en milisegundos
 */
void drv_tiempo_esperar_ms(Tiempo_ms_t ms){
	uint64_t tiempoEsperarTick = (uint64_t)ms*1000*infoReloj.ticks_per_us + hal_tiempo_actual_tick64();
	while (hal_tiempo_actual_tick64() < tiempoEsperarTick){;}
}
(uint64_t)
Tiempo_ms_t drv_tiempo_esperar_hasta_ms(Tiempo_ms_t ms){
	uint64_t tiempoEsperarTick = (uint64_t)ms*1000*infoReloj.ticks_per_us;
	while (hal_tiempo_actual_tick64() < tiempoEsperarTick){;}
	return hal_tiempo_actual_tick64()/(infoReloj.ticks_per_us * 1000);
}

void drv_tiempo_periodico_ms(Tiempo_ms_t ms, void (*funcion_callback_drv)(), uint32_t ID_evento){
	uint32_t tiempoTickExtra = (uint32_t)((uint64_t)ms*1000*infoReloj.ticks_per_us);
	hal_tiempo_reloj_periodico_tick(tiempoTickExtra, funcion_callback_drv);
}
