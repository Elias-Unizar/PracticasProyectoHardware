#ifndef SVC_ALARMA_H
#define SVC_ALARMA_H
#include <stdint.h>
#include <stdbool.h>
#include "svc_diagnostico.h"
#include "drv_tiempo.h"
#include "svc_GE.h"

err_t svc_alarma_iniciar(void (*cb_a_llamar)());

uint32_t svc_alarma_codificar(bool periodico, Tiempo_ms_t retardo_ms, uint8_t flags);

err_t svc_alarma_activar(uint32_t alarma_flags, EVENTO_T ID_evento, uint32_t auxData);

void svc_alarma_actualizar(EVENTO_T evento, uint32_t aux);

#endif
