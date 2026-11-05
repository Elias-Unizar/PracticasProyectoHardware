#ifndef DRV_MONITOR_H
#define DRV_MONITOR_H

#include <stdint.h>

typedef uint32_t MONITOR_id_t;

uint32_t drv_monitor_iniciar(void);

void drv_monitor_marcar(MONITOR_id_t id);

void drv_monitor_desmarcar(MONITOR_id_t id);

#endif
