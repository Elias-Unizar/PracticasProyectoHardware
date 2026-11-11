#ifndef SVC_GE_H
#define SVC_GE_H
#include <stdint.h>
#include <stddef.h>

#define EVENT_TYPES 8
typedef uint32_t EVENTO_T;

EVENTO_T svc_GE_reservar_ID();

svc_GE_suscribir(EVENTO_T ID_evento, uint8_t prioridad , void (*f_callback)());

#endif
