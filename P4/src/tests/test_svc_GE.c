#include "env.h"

#if defined(ACTIVAR_TEST) && defined(TEST_SVC_GE)

#include <stdint.h>
#include "test_svc_GE.h"
#include "svc_GE.h"
#include "rt_GE.h"
#include "rt_fifo.h"

err_t svc_GE_desreservar_ID(EVENTO_T ID_evento);                  /* falta en svc_GE.h */
err_t rt_GE_ejecutarFuncionesSubscritas(EVENTO *eventoExtraido);   /* falta en rt_GE.h */

/* Cada callback anade su numero a "llamadas": 1 y luego 2 -> 12 */
static volatile uint32_t llamadas;
static void f1(EVENTO_T e, uint32_t a){ llamadas = llamadas * 10 + 1; }
static void f2(EVENTO_T e, uint32_t a){ llamadas = llamadas * 10 + 2; }
static void f3(EVENTO_T e, uint32_t a){ llamadas = llamadas * 10 + 3; }
static void f4(EVENTO_T e, uint32_t a){ llamadas = llamadas * 10 + 4; }

/* Ejecuta las funciones suscritas al evento y devuelve el codigo de "llamadas" */
static uint32_t ejecutar(EVENTO_T id){
	EVENTO ev;
	ev.ID_EVENTO = id;
	ev.auxData = 0;
	llamadas = 0;
	rt_GE_ejecutarFuncionesSubscritas(&ev);
	return llamadas;
}

static uint8_t test_svc_GE_iniciar(void){
	if (svc_GE_iniciar() != ERROR_SINERROR) return 0;
	if (svc_GE_iniciar() != ERROR_SINERROR) return 0;   /* segunda vez, no pasa nada */
	return 1;
}

static uint8_t test_svc_GE_reservar_ID(void){
	EVENTO_T ids[EVENT_TYPES];
	for (uint32_t vuelta = 0; vuelta < 2; vuelta++){          /* llenar, borrar, llenar */
		for (uint32_t i = 0; i < EVENT_TYPES; i++){
			ids[i] = svc_GE_reservar_ID();
			if (ids[i] == ev_VOID) return 0;
		}
		for (uint32_t i = 0; i < EVENT_TYPES; i++){
			svc_GE_desreservar_ID(ids[i]);
		}
	}
	return 1;
}

static uint8_t test_svc_GE_desreservar_ID(void){
	EVENTO_T id = svc_GE_reservar_ID();
	if (svc_GE_desreservar_ID(id) != ERROR_SINERROR) return 0;
	if (svc_GE_desreservar_ID(id) != ERROR_EVENTO_NO_RESERVADO) return 0;       /* ya liberado */
	if (svc_GE_desreservar_ID(ev_VOID) != ERROR_EVENTO_NO_RESERVADO) return 0;
	return 1;
}

static uint8_t test_svc_GE_suscribir(void){
	EVENTO_T id = svc_GE_reservar_ID();
	svc_GE_desreservar_ID(id);
	if (svc_GE_suscribir(id, 0, f1) != ERROR_EVENTO_NO_RESERVADO) return 0;     /* sin reservar */

	id = svc_GE_reservar_ID();                                /* llenar: se llaman en orden */
	if (svc_GE_suscribir(id, 0, f1) != ERROR_SINERROR) return 0;
	if (svc_GE_suscribir(id, 0, f2) != ERROR_SINERROR) return 0;
	if (svc_GE_suscribir(id, 0, f3) != ERROR_SINERROR) return 0;
	if (svc_GE_suscribir(id, 0, f4) != ERROR_SINERROR) return 0;
	if (ejecutar(id) != 1234) return 0;
	svc_GE_desreservar_ID(id);

	id = svc_GE_reservar_ID();                                /* prioridad: el numero menor va antes */
	svc_GE_suscribir(id, 5, f1);
	svc_GE_suscribir(id, 0, f2);
	uint8_t ok = (ejecutar(id) == 21);
	svc_GE_desreservar_ID(id);
	return ok;
}

static uint8_t test_svc_GE_cancelar(void){
	EVENTO_T id = svc_GE_reservar_ID();
	svc_GE_suscribir(id, 0, f1);
	svc_GE_suscribir(id, 0, f2);
	if (svc_GE_cancelar(id, f1) != ERROR_SINERROR) return 0;
	if (ejecutar(id) != 2) return 0;                                            /* solo queda f2 */
	if (svc_GE_cancelar(id, f1) != ERROR_EVENTO_FUNCION_INEXISTENTE) return 0;
	svc_GE_desreservar_ID(id);
	if (svc_GE_cancelar(id, f2) != ERROR_EVENTO_NO_RESERVADO) return 0;         /* sin reservar */

	id = svc_GE_reservar_ID();
	for (uint32_t vuelta = 0; vuelta < 2; vuelta++){          /* llenar, borrar, llenar */
		if (svc_GE_suscribir(id, 0, f1) != ERROR_SINERROR) return 0;
		if (svc_GE_suscribir(id, 0, f2) != ERROR_SINERROR) return 0;
		if (svc_GE_suscribir(id, 0, f3) != ERROR_SINERROR) return 0;
		if (svc_GE_suscribir(id, 0, f4) != ERROR_SINERROR) return 0;
		if (ejecutar(id) != 1234) return 0;
		svc_GE_cancelar(id, f1);
		svc_GE_cancelar(id, f2);
		svc_GE_cancelar(id, f3);
		svc_GE_cancelar(id, f4);
		if (ejecutar(id) != 0) return 0;                      /* no debe quedar ninguna */
	}
	svc_GE_desreservar_ID(id);
	return 1;
}

static uint8_t test_rt_GE_ejecutarFuncionesSubscritas(void){
	EVENTO ev;
	EVENTO_T id = svc_GE_reservar_ID();
	svc_GE_desreservar_ID(id);
	ev.ID_EVENTO = id;
	ev.auxData = 0;
	if (rt_GE_ejecutarFuncionesSubscritas(&ev) != ERROR_EVENTO_NO_RESERVADO) return 0;

	id = svc_GE_reservar_ID();
	svc_GE_suscribir(id, 0, f1);
	svc_GE_suscribir(id, 0, f2);
	uint8_t ok = (ejecutar(id) == 12);
	svc_GE_desreservar_ID(id);
	return ok;
}

/* Solo se puede llamar una vez, y deja 2 IDs reservados: va el ultimo */
static uint8_t test_rt_GE_iniciar(void){
	EVENTO_T ids[EVENT_TYPES - 2];
	rt_GE_iniciar();
	for (uint32_t i = 0; i < EVENT_TYPES; i++){
		svc_GE_desreservar_ID(i+1);
	}
	for (uint32_t i = 0; i < EVENT_TYPES - 2; i++){           /* quedan EVENT_TYPES-2 libres */
		ids[i] = svc_GE_reservar_ID();
		if (ids[i] == ev_VOID) return 0;
	}
	for (uint32_t i = 0; i < EVENT_TYPES - 2; i++){
		svc_GE_desreservar_ID(ids[i]);
	}
	return 1;
}

uint8_t test_svc_GE_todos_no_bloq(void){
	uint8_t ok = 1;
	ok &= test_svc_GE_iniciar();
	ok &= test_svc_GE_reservar_ID();
	ok &= test_svc_GE_desreservar_ID();
	ok &= test_svc_GE_suscribir();
	ok &= test_svc_GE_cancelar();
	ok &= test_rt_GE_ejecutarFuncionesSubscritas();
	ok &= test_rt_GE_iniciar();
	return ok;
}

/* Bloqueante: el ultimo reservar no tiene hueco -> svc_DIAGNOSTICO_error() */
void test_svc_GE_reservar_ID_desborde(void){
	svc_GE_iniciar();
	for (uint32_t i = 0; i < EVENT_TYPES; i++){
		svc_GE_reservar_ID();
	}
	svc_GE_reservar_ID();
}

/* Bloqueante: la 5a suscripcion no tiene hueco -> svc_DIAGNOSTICO_error() */
void test_svc_GE_suscribir_desborde(void){
	svc_GE_iniciar();
	EVENTO_T id = svc_GE_reservar_ID();
	svc_GE_suscribir(id, 0, f1);
	svc_GE_suscribir(id, 0, f2);
	svc_GE_suscribir(id, 0, f3);
	svc_GE_suscribir(id, 0, f4);
	svc_GE_suscribir(id, 0, f1);
}

#endif
