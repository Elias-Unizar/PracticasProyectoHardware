#ifndef DRV_SC_H
#define DRV_SC_H

#include <stdint.h>

uint32_t drv_sc_enterSC(void);

void drv_sc_exitSC(uint32_t previo);

#endif
