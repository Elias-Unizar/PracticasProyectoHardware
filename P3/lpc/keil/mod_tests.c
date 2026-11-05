#define TEST_LEDS    1 //Tests basicos de leds y monitores
#define TEST_TIEMPO  2 //Tests muy basicos de temporizadores
#define TEST_TIEMPO_ERROR_US 20 //La cantidad de error que exigimos al temporizador(tener en cuenta lo que se tarda en llamar a las funciones
#define TEST_CONSUMO 3 //Tests manuales, en los que comprobar
#define TEST_RT_FIFO 4
//Se selecciona el tipo de test a realizar abajo
#define TIPOTEST TEST_TIEMPO

#include <stdint.h>
#include <stdbool.h>

#if TIPOTEST == TEST_LEDS
//Vamos a comprobar por encima que funcionan bien las funciones
#include "drv_leds.h"
#include "hal_gpio.h"
#include "drv_monitor.h"
#include "board.h"

static const HAL_GPIO_PIN_T monitoresPosibles[] = MONITOR_LIST;
uint8_t usarSinInicializar(){
	LED_status_t sts;
	if (drv_led_establecer(1, LED_ON) != 0) return 0;
	if (drv_led_estado(1, &sts) != 0)       return 0;
	if (drv_led_conmutar(1) != 0)           return 0;
	return 1;
}

uint8_t usarLedsInexistentes(){
	LED_status_t sts;
	if (drv_led_establecer(0, LED_ON) != 0) return 0;
	if (drv_led_estado(0, &sts) != 0)       return 0;
	if (drv_led_conmutar(0) != 0)           return 0;

	if (drv_led_establecer(LEDS_NUMBER+1, LED_ON) != 0) return 0;
	if (drv_led_estado(LEDS_NUMBER+1, &sts) != 0)       return 0;
	if (drv_led_conmutar(LEDS_NUMBER+1) != 0)           return 0;
	return 1;
}

uint8_t test_leds_iniciar(){
	unsigned int n = drv_leds_iniciar();
	if (n != LEDS_NUMBER) return 0;
	
	LED_status_t sts;
	for (LED_id_t id = 1; id <= LEDS_NUMBER; id++){
		if (!drv_led_estado(id, &sts)) 	return 0;
		if (sts != LED_OFF) 						return 0;
	}
	return 1;
}

uint8_t test_led_encender_apagar_doble(){
	LED_status_t sts;
	for (LED_id_t id = 1; id <= LEDS_NUMBER; id++){
		if (!drv_led_establecer(id, LED_ON)) return 0;
		if (!drv_led_establecer(id, LED_ON)) return 0;
		if (!drv_led_estado(id, &sts)) return 0; 
		if (sts != LED_ON) return 0;

		if (!drv_led_establecer(id, LED_OFF)) return 0;
		if (!drv_led_establecer(id, LED_OFF)) return 0;
		if (!drv_led_estado(id, &sts)) return 0; 
		if (sts != LED_OFF) return 0;
	}
	return 1;
}

uint8_t test_monitor(){
	if (drv_monitor_iniciar() != MONITOR_NUMBER) return 0;
	uint32_t activo = (LEDS_ACTIVE_STATE) ? 1u : 0u; //Vemos como se enciende el monitor, con que valor de corriente
	for (MONITOR_id_t id = 1; id <= MONITOR_NUMBER; ++id){
		drv_monitor_marcar(id);
		if(hal_gpio_leer(monitoresPosibles[id-1]) != activo) return 0;
		drv_monitor_desmarcar(id);
		if(hal_gpio_leer(monitoresPosibles[id-1]) == activo) return 0;
	}
	return 1;
}
#endif

#if TIPOTEST == TEST_TIEMPO
#include "drv_tiempo.h"
#include "hal_tiempo.h"

static volatile uint32_t contCb = 0;
void cb_contador(){ contCb++; }
uint8_t test_tiempo_monotono(){
	Tiempo_us_t anterior = drv_tiempo_actual_us(); //Mejorar para comprobar cuando ya desborda el reloj, será un test largo
	uint32_t diffTiempo = ((drv_tiempo_actual_ms()/1000) - anterior);//Deberia ser siempre positivo la diferencia
	if (diffTiempo > TEST_TIEMPO_ERROR_US) return 0;//Si esta desfada en 20us ya es un error gordo, la CPU trabaja en microsegundos
	Tiempo_us_t ahora;
	for (uint32_t i = 0; i < 1000; i++){
		ahora = drv_tiempo_actual_us();
		if (ahora < anterior) return 0;
		anterior = ahora;
	}
	return 1;
}

#endif

#if TIPOTEST == TEST_CONSUMO
#include "drv_tiempo.h"
#include "drv_consumo.h"
void cb(){;}
uint8_t test_consumo_esperar(){
	Tiempo_ms_t tiempoPeriodico = 20;
	Tiempo_us_t t0 = drv_tiempo_actual_us();
	drv_tiempo_periodico_ms(tiempoPeriodico, cb, 0);
	drv_consumo_esperar();//Si no se queda bloqueado aqui y avanza, entonces no espera el tiempo necesario, y se sabrá
	Tiempo_us_t diffTiempo = drv_tiempo_actual_us() - t0 - tiempoPeriodico*1000;//Puede ser negativo, se ha acelerado, o positivo se ha atrasado
	return (diffTiempo >= 18u) && (diffTiempo <= 25u);
}
#endif

#if TIPOTEST == TEST_RT_FIFO
#endif

int main(void){
#if TIPOTEST == TEST_LEDS

#endif

#if TIPOTEST == TEST_TIEMPO
#endif

#if TIPOTEST == TEST_CONSUMO
#endif

#if TIPOTEST == TEST_RT_FIFO
#endif
	while(1){;}
}