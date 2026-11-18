#ifndef DRV_WATCHDOG_H
#define DRV_WATCHDOG_H

#include <stdint.h>
#include <stdbool.h>

//Para aprovechar la capacidad del nrf
typedef uint32_t canal_watchdog_t;

uint32_t drv_watchdog_numero_canales(void);

void drv_watchdog_iniciar(uint32_t segundos);

void drv_watchdog_configurar(canal_watchdog_t canalWatch);

void drv_watchdog_alimentar(canal_watchdog_t canalWatch);//Al alimentar se detienen las interrupciones, no abusar de alimentar al watchdog

#endif
