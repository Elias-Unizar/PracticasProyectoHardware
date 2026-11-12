#ifndef SVC_GE_H
#define SVC_GE_H
#include <stdint.h>
#include <stddef.h>
#include "svc_diagnostico.h"

#define EVENT_TYPES 8
typedef uint32_t EVENTO_T;
EVENTO_T ev_VOID = 0;

void svc_GE_iniciar();

EVENTO_T svc_GE_reservar_ID();

err_t svc_GE_suscribir(EVENTO_T ID_evento, uint8_t prioridad , void (*f_callback)());

err_t svc_GE_cancelar(EVENTO_T ID_evento, void (*f_callback)());

#endif
