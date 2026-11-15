#ifndef TEST_SVC_GE_H
#define TEST_SVC_GE_H

#include <stdint.h>

/* Devuelve 1 si pasan todos, 0 si falla alguno. No incluye los bloqueantes.
 * Asume que nadie ha reservado IDs antes (no llamar antes a rt_GE_iniciar). */
uint8_t test_svc_GE_todos_no_bloq(void);

/* Bloqueantes: acaban en svc_DIAGNOSTICO_error(). Se comprueban a mano. */
void test_svc_GE_reservar_ID_desborde(void);
void test_svc_GE_suscribir_desborde(void);

#endif
