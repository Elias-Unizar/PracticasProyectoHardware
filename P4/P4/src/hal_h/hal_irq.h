#ifndef HAL_IRQ
#define HAL_IRQ

#include <stdint.h>

uint32_t hal_irq_deshabilitar(void);

void hal_irq_restaurar(uint32_t estado_previo);

#endif // HAL_TIEMPO
