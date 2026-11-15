#include "env.h"

#if defined(ACTIVAR_TEST) && defined(TEST_CONSUMO)

#include <stdint.h>
#include "test_consumo.h"
#include "drv_tiempo.h"
#include "drv_consumo.h"

static void cb(void){ ; }

static uint8_t test_consumo_esperar(void){
	drv_tiempo_iniciar();
	Tiempo_ms_t tiempoPeriodico = 20;
	Tiempo_ms_t t0 = drv_tiempo_actual_ms();
	drv_tiempo_periodico_ms(tiempoPeriodico, cb, 0);
	drv_consumo_esperar();//Si no se queda bloqueado aqui y avanza, entonces no espera el tiempo necesario, y se sabrá
	drv_tiempo_parar_periodico();
	Tiempo_ms_t diffTiempo = drv_tiempo_actual_ms() - t0;//Solo positivo
	return (diffTiempo < tiempoPeriodico + 2) && (diffTiempo >= tiempoPeriodico);
}

uint8_t test_consumo_todos(void){
	uint8_t ok = 1;
	ok &= test_consumo_esperar();
	return ok;
}

#endif
