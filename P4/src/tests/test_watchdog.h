#ifndef TEST_WATCHDOG_H
#define TEST_WATCHDOG_H

#include <stdint.h>

/* Devuelve 1 si pasan todos, 0 si falla alguno.
 * Necesita drv_tiempo funcionando. Deja el watchdog iniciado (2 s). */
uint8_t test_drv_watchdog_todos(void);

/* BLOQUEANTE: llamar despues de test_drv_watchdog_todos.
 * Si funciona, la placa se resetea y no devuelve nada; si devuelve 0, el watchdog no salto. */
uint8_t test_drv_watchdog_reset(void);

#endif
