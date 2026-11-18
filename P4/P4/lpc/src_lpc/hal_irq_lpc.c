/* *****************************************************************************
 * P.H.2025: Temporizadores en LPC2105, Timer 0 y Timer 1
 * implementacion para cumplir el hal_tiempo.h
 */
 
#include <LPC210x.H>
#include "hal_irq.h"

uint32_t hal_irq_deshabilitar(void){
	return __disable_irq();
}

void hal_irq_restaurar(uint32_t estado_previo){
	if(estado_previo == 0){
		__enable_irq();
	}
}
