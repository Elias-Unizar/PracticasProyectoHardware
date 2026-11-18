#ifndef TEST_SVC_ALARMA_H
#define TEST_SVC_ALARMA_H

#include <stdint.h>

/* Devuelve 1 si pasan todos, 0 si falla alguno.
 * Necesita svc_GE iniciado (lo hace el propio test) y drv_tiempo funcionando. */
uint8_t test_svc_alarma_todos(void);

#endif
