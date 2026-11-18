#ifndef TEST_TIEMPO_H
#define TEST_TIEMPO_H

#include <stdint.h>
#define TEST_TIEMPO_ERROR_US 20 //La cantidad de error que exigimos al temporizador(tener en cuenta lo que se tarda en llamar a las funciones

/* Ejecuta todos los tests de temporizadores.
 * Devuelve 1 si pasan todos, 0 si falla alguno. */
uint8_t test_tiempo_todos(void);

#endif
