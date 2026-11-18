#ifndef TEST_DRV_BOTONES_H
#define TEST_DRV_BOTONES_H

#include <stdint.h>

/* Devuelve 1 si pasan todos, 0 si falla alguno.
 * Solo se puede llamar una vez (drv_botones_iniciar reserva un ID y suscribe funciones).
 * No pulsar ningun boton mientras se ejecuta. */
uint8_t test_drv_botones_todos(void);

#endif
