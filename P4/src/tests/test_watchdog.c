#include "env.h"

#if defined(ACTIVAR_TEST) && defined(TEST_WATCHDOG)

#include <stdint.h>
#include <stdbool.h>
#include "test_watchdog.h"
#include "drv_watchdog.h"
#include "drv_tiempo.h"
#include "rt_fifo.h"

#define SEGUNDOS_WDT   2
#define ESPERA_MS      (SEGUNDOS_WDT * 1000 + 1000)   /* mas de lo que tarda en saltar */

/* Alimenta todos los canales cada 500 ms durante "ms" milisegundos */
static void alimentar_todos_durante(uint32_t ms){
	uint32_t n = drv_watchdog_numero_canales();
	Tiempo_ms_t t0 = drv_tiempo_actual_ms();
	while (drv_tiempo_actual_ms() - t0 < ms){
		for (canal_watchdog_t c = 0; c < n; c++) drv_watchdog_alimentar(c);
		drv_tiempo_esperar_ms(500);
	}
}

/* Caso normal: hay canales y siempre dice lo mismo */
static uint8_t test_drv_watchdog_numero_canales(void){
	uint32_t n = drv_watchdog_numero_canales();
	if (n == 0) return 0;
	if (drv_watchdog_numero_canales() != n) return 0;
	return 1;
}

/* Con 0 segundos no arranca: si arrancara, reseteaba y no llegariamos al return.
 * OJO: tiene que ir ANTES de cualquier iniciar() real (el nrf no puede parar el WDT) */
static uint8_t test_drv_watchdog_iniciar_cero(void){
	drv_watchdog_iniciar(0);
	drv_tiempo_esperar_ms(ESPERA_MS);
	return 1;
}

/* Canal que no existe (desbordar): se ignora, no peta */
static uint8_t test_drv_watchdog_canal_invalido(void){
	uint32_t n = drv_watchdog_numero_canales();
	drv_watchdog_configurar(n);
	drv_watchdog_configurar(n + 100);
	drv_watchdog_alimentar(n);
	drv_watchdog_alimentar(n + 100);
	return 1;
}

/* Llenar: configurar todos los canales, iniciar y alimentar: no debe resetear */
static uint8_t test_drv_watchdog_alimentar(void){
	uint32_t n = drv_watchdog_numero_canales();
	for (canal_watchdog_t c = 0; c < n; c++) drv_watchdog_configurar(c);
	drv_watchdog_iniciar(SEGUNDOS_WDT);
	alimentar_todos_durante(2 * ESPERA_MS);    /* mas del doble del timeout */
	return 1;                                  /* si llegamos aqui, no reseteo */
}

uint8_t test_drv_watchdog_todos(void){
	uint8_t ok = 1;
	ok &= test_drv_watchdog_numero_canales();
	ok &= test_drv_watchdog_iniciar_cero();
	ok &= test_drv_watchdog_canal_invalido();
	ok &= test_drv_watchdog_alimentar();
	return ok;
}

/* ---- BLOQUEANTE ---- */

/* Hay que llamarlo despues de test_drv_watchdog_todos (WDT ya iniciado).
 * Alimentar un canal que no existe no sirve: tiene que resetear la placa.
 * Si sale del bucle sin resetear, falla. */
uint8_t test_drv_watchdog_reset(void){
	uint32_t n = drv_watchdog_numero_canales();
	alimentar_todos_durante(2 * ESPERA_MS);
	rt_FIFO_inicializar();
	for(uint32_t i = 0; i < 32; i++)rt_FIFO_encolar(1,1);
	rt_FIFO_encolar(1,1);
	return 0;                                  /* no ha reseteado */
}

#endif
