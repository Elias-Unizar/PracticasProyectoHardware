#ifndef TEST_SVC_DIAGNOSTICO_H
#define TEST_SVC_DIAGNOSTICO_H

#include <stdint.h>

/* Devuelve 1 si pasan todos, 0 si falla alguno. No incluye el bloqueante. */
uint8_t test_svc_diagnostico_todos_no_bloq(void);

/* Bloqueante: marca el canal de error y se queda en bucle infinito.
 * Se comprueba a mano con el analizador. Llamarlo el ultimo. */
void test_svc_DIAGNOSTICO_error(void);

#endif
