#include "env.h"

#if defined(ACTIVAR_TEST) && defined(TEST_LEDS)

#include <stdint.h>
#include "test_led.h"
#include "drv_leds.h"
#include "hal_gpio.h"
#include "drv_monitor.h"
#include "board.h"

//Vamos a comprobar por encima que funcionan bien las funciones
static const HAL_GPIO_PIN_T monitoresPosibles[] = MONITOR_LIST;

static uint8_t usarLedsInexistentes(void){
	LED_status_t sts;
	if (drv_led_establecer(0, LED_ON) != 0) return 0;
	if (drv_led_estado(0, &sts) != 0)       return 0;
	if (drv_led_conmutar(0) != 0)           return 0;

	if (drv_led_establecer(LEDS_NUMBER+1, LED_ON) != 0) return 0;
	if (drv_led_estado(LEDS_NUMBER+1, &sts) != 0)       return 0;
	if (drv_led_conmutar(LEDS_NUMBER+1) != 0)           return 0;
	return 1;
}

static uint8_t test_leds_iniciar(void){
	unsigned int n = drv_leds_iniciar();
	if (n != LEDS_NUMBER) return 0;

	LED_status_t sts;
	for (LED_id_t id = 1; id <= LEDS_NUMBER; id++){
		if (!drv_led_estado(id, &sts)) 	return 0;
		if (sts != LED_OFF) 						return 0;
	}
	return 1;
}

static uint8_t test_led_encender_apagar_doble(void){
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

static uint8_t test_monitor(void){
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

uint8_t test_leds_todos(void){
	uint8_t ok = 1;
	/* El orden importa: "sin inicializar" va antes de drv_leds_iniciar() */
	ok &= test_leds_iniciar();
	ok &= usarLedsInexistentes();
	ok &= test_led_encender_apagar_doble();
	ok &= test_monitor();
	return ok;
}

#endif
