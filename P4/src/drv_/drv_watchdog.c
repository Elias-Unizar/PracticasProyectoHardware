#include "drv_watchdog.h"
#include "hal_watchdog.h"
#include "board.h"
#include <stddef.h>   // define NULL

static uint32_t numeroCanalesWatchdog = 0;

uint32_t drv_watchdog_numero_canales(){
	numeroCanalesWatchdog = hal_watchdog_numeroCanales();
	return numeroCanalesWatchdog;
}

/*PRE: obligatorio llamar antes a numero_canales()*/
void drv_watchdog_iniciar(uint32_t segundos){
	if(segundos != 0 && numeroCanalesWatchdog != 0){
		hal_watchdog_iniciar(segundos);
	}
}

void drv_watchdog_configurar(canal_watchdog_t canalWatch){
	if(canalWatch < numeroCanalesWatchdog) hal_watchdog_configurar(canalWatch);
}

//Al alimentar se detienen las interrupciones, no abusar de alimentar al watchdog
void drv_watchdog_alimentar(canal_watchdog_t canalWatch){
	if(canalWatch < numeroCanalesWatchdog) hal_watchdog_alimentar(canalWatch);
}
