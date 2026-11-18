#ifndef HAL_WATCHDOG
#define HAL_WATCHDOG

#include <stdint.h>

uint32_t hal_watchdog_numeroCanales(void);

void hal_watchdog_iniciar(uint32_t segundos);

//Solo se pueden añadir canales de watchdog antes de que este inicie, en el nrf
void hal_watchdog_configurar(uint32_t canalWatchdog);

//Al alimentar se detienen las interrupciones, no abusar de alimentar al watchdog
void hal_watchdog_alimentar(uint32_t canalWatchdog);

#endif // HAL_TIEMPO
