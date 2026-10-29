#ifndef DRV_MONITOR_H
#define DRV_MONITOR_H

#include <stdint.h>

typedef uint8_t MONITOR_id_t;

int drv_monitor_iniciar(void);

int drv_monitor_marcar(MONITOR_id_t id);

int drv_monitor_desmarcar(MONITOR_id_t id);

#endif
