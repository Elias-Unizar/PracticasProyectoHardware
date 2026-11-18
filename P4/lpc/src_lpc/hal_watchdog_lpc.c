#include <LPC210x.H>
#include "hal_watchdog.h"

#include <stdbool.h>  // si usas bool/true/false

static uint8_t configurado = false;
uint32_t hal_watchdog_numeroCanales(){
	return 1;
}

void hal_watchdog_iniciar(uint32_t segundos){
	if(configurado){
		WDTC = segundos * (15000000 / 4);//Tiene un minimo de 0xFF, pero en nuestro caso nunca se alcanza, estamos en escala de segundos
		WDMOD = 0x03;
		hal_watchdog_alimentar(0);
	}
}

void hal_watchdog_configurar(uint32_t canalWatchdog){
	//En LPC no hace nada, pero por mantener consistencia
	if(canalWatchdog == 0)configurado = true;
}
//Al alimentar se detienen las interrupciones, no abusar de alimentar al watchdog
void hal_watchdog_alimentar(uint32_t canalWatchdog){
	if(canalWatchdog != 0) return;
	//en el lpc da igual ese canalWatchdog
	uint32_t estadoIRQ = __disable_irq();//devuelve si han estado habilitadas o no
	WDFEED = 0xAA; // Alimento el WatchDog
	WDFEED = 0x55;
	if(estadoIRQ == 0)__enable_irq();
}
