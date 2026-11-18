#include "env.h"

#if defined(ACTIVAR_TEST) && defined(TEST_TIEMPO)

#include <stdint.h>
#include "test_tiempo.h"
#include "drv_tiempo.h"
#include "hal_tiempo.h"

//static volatile uint32_t contCb = 0;
//static void cb_contador(){ contCb++; }

static uint8_t test_tiempo_monotono(void){
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

uint8_t test_tiempo_todos(void){
	uint8_t ok = 1;
	ok &= test_tiempo_monotono();
	return ok;
}

#endif
