/* *****************************************************************************
 * P.H.2025: Driver/Manejador de los temporizadores
 * suministra los servicios independientemente del hardware
 *
 * usa los servicios de hal_tiempo.h: 
 */
 
#include "drv_tiempo.h"
#include "hal_tiempo.h"
#include <stddef.h>   // define NULL

static hal_tiempo_info_t infoReloj = {0, 0, 0};

static hal_tiempo_info_t infoRelojPer = {0, 0, 0};


#ifdef TEST_TIEMPO
#include "hal_tiempo.h"
void drv_tiempo_info_relojes(hal_tiempo_info_t *out_info_mon, hal_tiempo_info_t *out_info_per){
	out_info_mon = &infoReloj;
	out_info_per ) &infoRelojPer;
}

#endif
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
	hal_tiempo_iniciar_tick_periodico(&infoRelojPer);
	return infoReloj.ticks_per_us != 0 && infoRelojPer.ticks_per_us != 0;
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

Tiempo_ms_t drv_tiempo_esperar_hasta_ms(Tiempo_ms_t ms){
	uint64_t tiempoEsperarTick = (uint64_t)ms*1000*infoReloj.ticks_per_us;
	while (hal_tiempo_actual_tick64() < tiempoEsperarTick){;}
	return hal_tiempo_actual_tick64()/(infoReloj.ticks_per_us * 1000);
}

static void (*app_cb)(uint32_t) = 0;
static uint32_t param_interno; //ID_evento

void funcion_callback_drv(){
	app_cb(param_interno);
}

void drv_tiempo_registrar_callback(void (*funcion_callback)(), uint32_t param1){
	app_cb = funcion_callback;
	param_interno = param1;
}

void drv_tiempo_periodico_ms(Tiempo_ms_t ms, void (*funcion_callback_app)(), uint32_t ID_evento){
	uint32_t tiempoTickExtra = ms*1000*infoRelojPer.ticks_per_us;
		
	drv_tiempo_registrar_callback(funcion_callback_app, ID_evento);
	
	hal_tiempo_reloj_periodico_tick(tiempoTickExtra, funcion_callback_drv);
}
