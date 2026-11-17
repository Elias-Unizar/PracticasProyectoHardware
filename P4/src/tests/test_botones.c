#include "env.h"

#if defined(ACTIVAR_TEST) && defined(TEST_BOTONES)

#include <stdint.h>
#include "test_botones.h"
#include "drv_botones.h"
#include "svc_GE.h"
#include "svc_alarma.h"
#include "drv_tiempo.h"
#include "board.h"

#define T_BOTON_MS 20     /* los 15-20 ms de la maquina de estados de drv_botones.c */
#define TOL_MS     5

static volatile uint32_t nCb;
static EVENTO_T ultimoEv;
static uint32_t ultimoAux;
static EVENTO_T ev_pulsa;                                   /* se reserva en test_drv_botones_iniciar */

/* Hace de callback del servicio de alarmas: lo que seria encolar en la FIFO */
static void cb_test(EVENTO_T ev, uint32_t aux){
	nCb++;
	ultimoEv = ev;
	ultimoAux = aux;
}

static uint8_t test_drv_botones_iniciar(void){
	svc_alarma_iniciar(cb_test);
	ev_pulsa = svc_GE_reservar_ID();
	if (ev_pulsa == ev_VOID) return 0;
	if (drv_botones_iniciar(cb_test, ev_pulsa) != BUTTONS_NUMBER) return 0;
	return 1;
}

static uint8_t test_drv_botones_pulsado(void){
	for (uint32_t i = 0; i < BUTTONS_NUMBER; i++){            /* sin tocar nada, ninguno pulsado */
		if (drv_botones_pulsado(i) != BOTON_NO_PULSADO) return 0;
	}
	return 1;
}

static uint8_t test_drv_botones_pulsacion(void){
	for (uint32_t boton = 0; boton < BUTTONS_NUMBER; boton++){
		drv_botones_actualizar(ev_pulsa, boton);               /* lo que haria el GE al llegar la pulsacion */
		if (drv_botones_pulsado(boton) != BOTON_PULSADO) return 0;                 /* rebotes */

		for (uint32_t paso = 0; paso < 3; paso++){             /* rebotes -> muestreo -> salida -> reposo */
			nCb = 0;
			drv_tiempo_esperar_ms(T_BOTON_MS + TOL_MS);
			if (nCb == 0) return 0;                            /* no ha saltado el timer */
			svc_alarma_actualizar(ultimoEv, ultimoAux);        /* sale el evento de la alarma */
			if (ultimoAux != boton) return 0;                  /* y es de este boton */
			drv_botones_actualizar(ultimoEv, ultimoAux);       /* lo que haria el GE */
			if (paso == 0 && drv_botones_pulsado(boton) != BOTON_PULSADO) return 0;    /* muestreo */
			if (paso != 0 && drv_botones_pulsado(boton) != BOTON_NO_PULSADO) return 0; /* soltado */
		}
	}
	return 1;
}

static boton_status_t esperado(uint32_t pasos){               /* rebotes y muestreo: pulsado; salida y reposo: no */
	return (pasos < 2) ? BOTON_PULSADO : BOTON_NO_PULSADO;
}

/* Dos botones a la vez, el 1 pulsado a mitad de tiempo del 0: cada uno avanza con su alarma.
 * Necesita T_BOTON_MS >= 2 para que el desfase no sea 0 */
static uint8_t test_drv_botones_dos_botones(void){
	uint32_t pasos[2] = {0, 0};
	drv_botones_actualizar(ev_pulsa, 0);
	drv_tiempo_esperar_ms(T_BOTON_MS / 2);
	drv_botones_actualizar(ev_pulsa, 1);                       /* se pulsa el 1 con el 0 a medias */

	for (uint32_t i = 0; i < 6; i++){                          /* 3 pasos por boton: salen 0,1,0,1,0,1 */
		nCb = 0;
		while (nCb == 0) {;}                                   /* espera al timer */
		svc_alarma_actualizar(ultimoEv, ultimoAux);
		uint32_t boton = ultimoAux;
		if (boton != i % 2) return 0;                          /* cada alarma con su aux */
		drv_botones_actualizar(ultimoEv, boton);               /* lo que haria el GE */
		pasos[boton]++;
		if (drv_botones_pulsado(0) != esperado(pasos[0])) return 0;
		if (drv_botones_pulsado(1) != esperado(pasos[1])) return 0;
	}
	return 1;
}

uint8_t test_drv_botones_todos(void){
	uint8_t ok = 1;
	ok &= test_drv_botones_iniciar();
	ok &= test_drv_botones_pulsado();
	ok &= test_drv_botones_pulsacion();
	ok &= test_drv_botones_dos_botones();
	return ok;
}

#endif
