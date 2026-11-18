#include "env.h"

#if defined(ACTIVAR_TEST) && defined(TEST_SVC_ALARMA)

#include <stdint.h>
#include <stdbool.h>
#include "test_svc_alarma.h"
#include "svc_alarma.h"
#include "svc_GE.h"
#include "drv_tiempo.h"

/* falta en svc_alarma.h */
void decodificar_flags(uint32_t alarma_flags, bool *esPeriodico, Tiempo_ms_t *retardo_ms, uint8_t *flags);

/* Hace de rt_FIFO_encolar: solo apunta lo que le llega */
static volatile uint32_t nCb = 0;
static volatile EVENTO_T ultimoEv = 0;
static volatile uint32_t ultimoAux = 0;
static void cb_alarma(EVENTO_T ev, uint32_t aux){
	nCb++;
	ultimoEv = ev;
	ultimoAux = aux;
}

/* Alarma de un solo disparo; con ms = 0 la desactiva */
static err_t activar(EVENTO_T id, Tiempo_ms_t ms){
	return svc_alarma_activar(svc_alarma_codificar(false, ms, 0), id, 0);
}

static uint8_t test_svc_alarma_iniciar(void){
	svc_GE_iniciar();       /* alarma reserva un ID del GE; si drv_tiempo necesita iniciarse, hazlo antes */
	if (svc_alarma_iniciar(cb_alarma) != ERROR_SINERROR) return 0;
	if (svc_alarma_iniciar(cb_alarma) != ERROR_SINERROR) return 0;   /* segunda vez, no pasa nada */
	return 1;
}

static uint8_t test_svc_alarma_codificar(void){
	if (svc_alarma_codificar(false, 100, 0) != 0x00000064u) return 0;
	if (svc_alarma_codificar(true,  100, 0) != 0x80000064u) return 0;
	if (svc_alarma_codificar(false, 5,   3) != 0x03000005u) return 0;
	return 1;
}

static uint8_t test_decodificar_flags(void){
	bool periodica;
	Tiempo_ms_t ms;
	uint8_t flags;
	decodificar_flags(svc_alarma_codificar(true, 1234, 5), &periodica, &ms, &flags);
	return (periodica == true) && (ms == 1234) && (flags == 5);
}

static uint8_t test_svc_alarma_activar(void){
	/* llenar las 4 alarmas (svc_ALARMAS_MAX) */
	for (EVENTO_T id = 1; id <= 4; id++){
		if (activar(id, 1000) != ERROR_SINERROR) return 0;
	}
	if (activar(9, 1000) == ERROR_SINERROR) return 0;     /* una 5a distinta: no hay hueco */
	if (activar(2, 500) != ERROR_SINERROR) return 0;      /* reprogramar una existente si cabe */

	/* borrar (retardo 0) y volver a llenar con otros eventos */
	for (EVENTO_T id = 1; id <= 4; id++){
		if (activar(id, 0) != ERROR_SINERROR) return 0;
	}
	for (EVENTO_T id = 5; id <= 8; id++){
		if (activar(id, 1000) != ERROR_SINERROR) return 0;
	}
	for (EVENTO_T id = 5; id <= 8; id++){
		activar(id, 0);
	}
	return 1;
}

static uint8_t test_svc_alarma_actualizar(void){
	nCb = 0;
	svc_alarma_activar(svc_alarma_codificar(false, 20, 0), 3, 7);   /* evento 3, aux 7, a los 20 ms */
	drv_tiempo_esperar_ms(25);
	if (nCb == 0) return 0;                               /* el timer no ha avisado */
	svc_alarma_actualizar(ultimoEv, ultimoAux);           /* lo que haria el GE al sacarlo de la FIFO */
	return (ultimoEv == 3) && (ultimoAux == 7);
}

static uint8_t test_svc_alarma_ids(void){
	Tiempo_ms_t t0;
	svc_alarma_activar(svc_alarma_codificar(false, 20, 0), 5, 11);   /* ID 5, aux 11, a los 20 ms */
	svc_alarma_activar(svc_alarma_codificar(false, 40, 0), 6, 22);   /* ID 6, aux 22, a los 40 ms */

	nCb = 0;
	t0 = drv_tiempo_actual_ms();
	while (nCb == 0 && drv_tiempo_actual_ms() - t0 < 100) {;}       /* salta el timer */
	if (nCb == 0) return 0;
	svc_alarma_actualizar(ultimoEv, ultimoAux);                      /* lo que haria el GE */
	if (ultimoEv != 5 || ultimoAux != 11) return 0;

	nCb = 0;
	t0 = drv_tiempo_actual_ms();
	while (nCb == 0 && drv_tiempo_actual_ms() - t0 < 100) {;}
	if (nCb == 0) return 0;
	svc_alarma_actualizar(ultimoEv, ultimoAux);
	return (ultimoEv == 6) && (ultimoAux == 22);
}


#define TOL_MS 5
/* Periodica: salta cada 20 ms con su ID y su aux; al cancelarla deja de saltar */
static uint8_t test_svc_alarma_periodica(void){
	svc_alarma_activar(svc_alarma_codificar(true, 20, 0), 5, 3);     /* ID 5, aux 3, cada 20 ms */
	for (uint32_t i = 0; i < 3; i++){
		nCb = 0;
		drv_tiempo_esperar_ms(20 + TOL_MS);
		if (nCb == 0) return 0;                                      /* el timer no ha avisado */
		svc_alarma_actualizar(ultimoEv, ultimoAux);                  /* lo que haria el GE */
		if (ultimoEv != 5 || ultimoAux != 3) return 0;
	}
	svc_alarma_activar(svc_alarma_codificar(false, 0, 0), 5, 3);     /* cancelar: mismo ID y aux */
	nCb = 0;
	drv_tiempo_esperar_ms(20 + TOL_MS);
	return nCb == 0;                                                 /* ya no salta */
}

/* Reprogramar el mismo ID: solo vale la ultima; la antigua (200 ms) no salta */
static uint8_t test_svc_alarma_reprogramar(void){
	svc_alarma_activar(svc_alarma_codificar(false, 200, 0), 5, 1);
	drv_tiempo_esperar_ms(2);
	svc_alarma_activar(svc_alarma_codificar(false, 20, 0), 5, 1);    /* mismo ID: sustituye */
	nCb = 0;
	drv_tiempo_esperar_ms(20 + TOL_MS);	///JAJAJAJAJAJ mierda LPC y sus muertos, no al hacer un segundo timer, Timer0 no resetea su TC, y por tanto nada funciona
	if (nCb == 0) return 0;
	svc_alarma_actualizar(ultimoEv, ultimoAux);
	if (ultimoEv != 5 || ultimoAux != 1) return 0;                   /* salta la nueva (mismo ID y aux) */
	nCb = 0;
	drv_tiempo_esperar_ms(200 + TOL_MS);
	return nCb == 0;                                                 /* la de 200 ms ya no existe */
}

/* 4 alarmas, la ultima en activarse es la primera en saltar: deben saltar en orden
 * inverso al de activacion y cada una a su hora (retardo +-5 ms) */
static uint8_t test_svc_alarma_tiempos(void){
	const Tiempo_ms_t retardo[4] = {100, 80, 60, 40};                /* IDs 5, 6, 7, 8 */
	Tiempo_ms_t t0, t;
	int32_t i;

	t0 = drv_tiempo_actual_ms();
	for (i = 0; i < 4; i++){
		if (svc_alarma_activar(svc_alarma_codificar(false, retardo[i], 0), 5 + i, 100 + i) != ERROR_SINERROR) return 0;
	}
	for (i = 3; i >= 0; i--){                                        /* salta primero la ultima */
		nCb = 0;
		while (nCb == 0) {;}                                         /* espera al timer */
		t = drv_tiempo_actual_ms() - t0;
		svc_alarma_actualizar(ultimoEv, ultimoAux);
		if (ultimoEv != (EVENTO_T)(5 + i) || ultimoAux != (uint32_t)(100 + i)) return 0;
		if (t + TOL_MS < retardo[i] || t > retardo[i] + TOL_MS) return 0;
	}
	return 1;
}


uint8_t test_svc_alarma_todos(void){
	uint8_t ok = 1;
	ok &= test_svc_alarma_iniciar();
	ok &= test_svc_alarma_codificar();
	ok &= test_decodificar_flags();
	ok &= test_svc_alarma_activar();
	ok &= test_svc_alarma_actualizar();
	ok &= test_svc_alarma_ids();
	ok &= test_svc_alarma_periodica();
	ok &= test_svc_alarma_reprogramar();
	ok &= test_svc_alarma_tiempos();
	return ok;
}

#endif
