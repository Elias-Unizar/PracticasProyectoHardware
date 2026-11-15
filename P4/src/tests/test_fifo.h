#ifndef TEST_RT_FIFO_H
#define TEST_RT_FIFO_H

#include <stdint.h>

/* Ejecuta todos los tests de rt_fifo.
 * Devuelve 1 si pasan todos, 0 si falla alguno.
 * OJO: el ultimo test (desbordamiento) provoca un error a proposito y puede
 * no retornar; por eso este modulo debe ir el ultimo en el main general. */
void test_fifo_desbordamiento(void);

uint8_t test_fifo_todos_no_bloq(void);//No incluye fallo por desbordamiento


#endif
