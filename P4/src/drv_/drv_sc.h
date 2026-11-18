#ifndef DRV_SC_H
#define DRV_SC_H

#include <stdint.h>

uint32_t enterSC(void);

void exitSC(uint32_t previo);

#endif
