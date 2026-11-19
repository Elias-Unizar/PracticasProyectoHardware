#include "env.h"

#if defined(ACTIVAR_TEST) && defined(TEST_TIEMPO)

#include <stdint.h>
#include "test_tiempo.h"
#include "drv_tiempo.h"
#include "hal_tiempo.h"

//static volatile uint32_t contCb = 0;
//static void cb_contador(){ contCb++; }

static uint8_t test_tiempo_monotono(){
	Tiempo_us_t anterior = drv_tiempo_actual_us(); //Mejorar para comprobar cuando ya desborda el reloj, será un test largo
	int32_t diffTiempo = (drv_tiempo_actual_us() - anterior);//Deberia ser siempre positivo la diferencia
	if (diffTiempo <= 0) return 0;
	Tiempo_us_t ahora;
	for (uint32_t i = 0; i < 1000; i++){
		ahora = drv_tiempo_actual_us();
		if (ahora < anterior) return 0;
		anterior = ahora;
	}
	return 1;
}

uint8_t test_tiempo_todos(void){
	drv_tiempo_iniciar();
	uint8_t ok = 1;
	ok &= test_tiempo_monotono();
	return ok;
}

#endif
