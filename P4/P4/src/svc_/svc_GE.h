#ifndef SVC_GE_H
#define SVC_GE_H
#include <stdint.h>
#include <stddef.h>
#include "svc_diagnostico.h"

#define EVENT_TYPES 8 //Sin el ev_VOID

typedef void (*CALLBACK_T)();//(evento, aux)
typedef uint32_t EVENTO_T;

#define ev_VOID 0


err_t svc_GE_iniciar(void);

EVENTO_T svc_GE_reservar_ID(void);

err_t svc_GE_suscribir(EVENTO_T ID_evento, uint8_t prioridad , CALLBACK_T f_callback);

err_t svc_GE_cancelar(EVENTO_T ID_evento, CALLBACK_T f_callback);

#endif
