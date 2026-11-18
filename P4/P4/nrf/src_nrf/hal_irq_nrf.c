#include <nrf.h>
#include "hal_irq.h"

uint32_t hal_irq_deshabilitar(void){
	uint32_t previo = __get_PRIMASK();
	__disable_irq();
	return previo;
}

void hal_irq_restaurar(uint32_t estado_previo){
	__set_PRIMASK(estado_previo);
}
