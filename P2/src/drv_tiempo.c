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

static volatile bool esperaActiva;
void dejarDeEsperar(){
	esperaActiva = false;
}

/**
 * retardo: esperar un cierto tiempo en milisegundos
 */
void drv_tiempo_esperar_ms(Tiempo_ms_t ms){
	uint32_t tiempoTickExtra = ms*1000*infoReloj.ticks_per_us;
	esperaActiva = true;
	hal_tiempo_reloj_periodico_tick(tiempoTickExtra, &dejarDeEsperar);
	while(esperaActiva){}
}

/**
 * esperar hasta un determinado tiempo (en ms), devuelve el tiempo actual
 */
Tiempo_ms_t drv_tiempo_esperar_hasta_ms(Tiempo_ms_t ms){
	uint64_t tiempoReloj0 = hal_tiempo_actual_tick64(); //Es un tiempo total de ejecución, con overflows y demas
	Tiempo_ms_t tiempoFallo = tiempoReloj0/(infoReloj.ticks_per_us * 1000);
	uint64_t tiempoLimite = ms*1000*infoReloj.ticks_per_us;//Suponemos que el usuario no desbordaria cosas, de todas formas comprobamos
	if(tiempoReloj0 > tiempoLimite) return tiempoFallo;//No podemos esperar, el tiempo limite es una poronga
	esperaActiva = true;
	uint64_t tiempoAEsperar = tiempoLimite - tiempoReloj0;
	if(tiempoAEsperar > infoReloj.counter_max) return tiempoFallo;//Supera el limite de lo que nuestro contador puede detectar; Seguro hay mejoras... otras RSI mas robustas
	//En este punto el tiempoAEsperar cabe en nuestro MatchRegister de 32bits
	hal_tiempo_reloj_periodico_tick(tiempoAEsperar, &dejarDeEsperar);
	while(esperaActiva){}
	return ms;
}
