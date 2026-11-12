#ifndef RT_GE_H
#define RT_GE_H
#include <stdint.h>
#include <stddef.h>
#include "svc_GE.h"

void rt_GE_lanzador(void);

void rt_GE_iniciar(void);

void rt_GE_actualizar(EVENTO_T ID_evento, uint32_t auxiliar);

#endif
