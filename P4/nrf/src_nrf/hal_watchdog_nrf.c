#include <nrf.h>
#include "hal_watchdog.h"

#include <stdbool.h>  // si usas bool/true/false



static uint8_t configurado[8];

uint32_t hal_watchdog_numeroCanales(){
	return 8;
}

void hal_watchdog_iniciar(uint32_t segundos){
	if(segundos == 0)return;
	NRF_WDT->CRV = segundos * 32768 - 1;//Aplicamos la formula del timeout, para ver el valor del CRV a colocar
	NRF_WDT->RREN = 0;
	for(uint32_t i = 0; i < 8; i++){
		if(configurado[i] == 1){
			NRF_WDT->RREN |= (1 << i);
		}
	}
	NRF_WDT->CONFIG = (WDT_CONFIG_SLEEP_Pause  << WDT_CONFIG_SLEEP_Pos)//Si esta dormido, total, da igual
	                | (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos);
	NRF_WDT->TASKS_START = 1;
}

void hal_watchdog_configurar(uint32_t canalWatchdog){
	if(canalWatchdog < 8)configurado[canalWatchdog] = 1;
}

void hal_watchdog_alimentar(uint32_t canalWatchdog){
	if(canalWatchdog < 8 || NRF_WDT->RUNSTATUS == 0) return;
	NRF_WDT->RR[canalWatchdog] = WDT_RR_RR_Reload;
}
