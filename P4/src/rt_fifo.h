#ifndef RT_FIFO_H
#define RT_FIFO_H
#include "stdint.h"
#include "svc_GE.h"
#include "drv_tiempo.h"

typedef struct{
	EVENTO_T ID_EVENTO;
	uint32_t auxData;
	Tiempo_us_t TS;
}EVENTO;

void rt_FIFO_inicializar(uint32_t monitor_overflow);

void rt_FIFO_encolar(EVENTO_T ID_evento, uint32_t auxData);

uint8_t rt_FIFO_extraer(EVENTO *eventoAExtraer);

uint32_t rt_FIFO_estadisticas(EVENTO_T ID_evento);

#endif
