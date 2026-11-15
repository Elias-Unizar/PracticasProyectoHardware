#include "env.h"

#if defined(ACTIVAR_TEST) && defined(TEST_RT_FIFO)

#include <stdint.h>
#include "test_fifo.h"
#include "rt_fifo.h"

#define FIFO_CAPACIDAD 32

/* Vacia la FIFO y devuelve cuantos eventos habia */
static uint32_t vaciar_fifo(void){
	EVENTO ev;
	uint32_t n = 0;
	while (rt_FIFO_extraer(&ev) && n < 100000u) n++;
	return n;
}

/* FIFO vacia -> extraer devuelve 0 */
static uint8_t test_fifo_vacia(void){
	EVENTO ev;
	vaciar_fifo();

	return rt_FIFO_extraer(&ev) == 0;
}

/* Orden FIFO y datos intactos */
static uint8_t test_fifo_orden(void){
	EVENTO ev;
	vaciar_fifo();
	rt_FIFO_encolar(1, 11);
	rt_FIFO_encolar(2, 22);
	rt_FIFO_extraer(&ev);
	if (ev.ID_EVENTO != 1) return 0;
	rt_FIFO_extraer(&ev);
	if (ev.ID_EVENTO != 2) return 0;
	return rt_FIFO_extraer(&ev) == 0;
}

/* Estadisticas: tras encolar n eventos de un tipo, su contador sube n */
static uint8_t test_fifo_estadisticas(void){
	uint32_t antesP, antesB, antesV;
	vaciar_fifo();
	antesV = rt_FIFO_estadisticas(ev_VOID);
	antesP = rt_FIFO_estadisticas(1);
	antesB = rt_FIFO_estadisticas(2);
	for (uint32_t i = 0; i < 3; i++) rt_FIFO_encolar(1, i);
	rt_FIFO_encolar(2, 0);

	if (rt_FIFO_estadisticas(1) - antesP != 3u) return 0;
	if (rt_FIFO_estadisticas(2) - antesB != 1u) return 0;
	if (rt_FIFO_estadisticas(ev_VOID) - antesV != 4u) return 0;
	vaciar_fifo();
	return 1;
}

/* Llenar hasta el tope, extraer todo en orden, y repetirlo con la FIFO ya usada */
static uint8_t test_fifo_llenar_vaciar_llenar(void){
	EVENTO ev;
	uint32_t i, vuelta;
	vaciar_fifo();

	for (vuelta = 0; vuelta < 2; vuelta++){
		for (i = 0; i < FIFO_CAPACIDAD; i++){
			rt_FIFO_encolar(1, i + 100u * vuelta);
		}
		for (i = 0; i < FIFO_CAPACIDAD; i++){
			if (!rt_FIFO_extraer(&ev)) return 0;
			if (ev.ID_EVENTO != 1) return 0;
			if (ev.auxData != i + 100u * vuelta) return 0;   /* orden y datos intactos */
		}
		if (rt_FIFO_extraer(&ev) != 0) return 0;             /* debe quedar vacia */
	}
	return 1;
}

/* Desbordamiento: encolar de mas no cuelga ni corrompe la cola.
 * El monitor de overflow se comprueba a mano con el analizador.
 * No devuelve resultado: la comprobacion es manual. */
void test_fifo_desbordamiento(void){
	vaciar_fifo();
	for (uint32_t i = 0; i < FIFO_CAPACIDAD; i++) rt_FIFO_encolar(1, i);
	rt_FIFO_encolar(1, 1000);   /* aqui debe saltar el error */
}

uint8_t test_fifo_todos_no_bloq(void){
	uint8_t ok = 1;
	rt_FIFO_inicializar();
	ok &= test_fifo_vacia();
	ok &= test_fifo_orden();
	ok &= test_fifo_estadisticas();
	ok &= test_fifo_llenar_vaciar_llenar();
	return ok;
}


#endif
