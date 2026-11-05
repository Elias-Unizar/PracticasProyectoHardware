#ifndef DRV_CONSUMO_H
#define DRV_CONSUMO_H

#include <stdint.h>
#include <stddef.h>

typedef uint32_t Monitor_id_t;

void drv_consumo_iniciar(Monitor_id_t monitorConsumoEsperar, Monitor_id_t monitorConsumoDormir);


void drv_consumo_esperar(void);


void drv_consumo_dormir(void);

#endif
