#ifndef RT_GE_H
#define RT_GE_H
#include <stdint.h>
#include "svc_GE.h"

#define ERROR_EVENTO_NO_RESERVADO (-1)
#define ERROR_EVENTO_OVERFLOW_SUSCRITOS (-2)
#define ERROR_EVENTO_FUNCION_INEXISTENTE (-3)

void rt_GE_lanzador(void);

void rt_GE_iniciar(void);

void rt_GE_actualizar(EVENTO_T ID_evento, uint32_t auxiliar);

#endif
