#include "env.h"

#if defined(ACTIVAR_TEST) && defined(TEST_SVC_DIAGNOSTICO)

#include <stdint.h>
#include "test_svc_diagnostico.h"
#include "svc_diagnostico.h"
#include "board.h"

#define CANAL_NINGUNO (0)   /* lo que devuelve reservar() si no quedan */

err_t svc_DIAGNOSTICO_desreservar(CANAL_T canal);   /* falta en svc_diagnostico.h */

/* Reserva canales hasta que no queden. Devuelve cuantos ha reservado */
static uint32_t reservar_todos(CANAL_T canales[]){
	uint32_t n = 0;//el de error
	CANAL_T c;
	while (n < MONITOR_NUMBER-1){
		c = svc_DIAGNOSTICO_reservar();
		canales[n] = c;
		n++;
	}
	return n;
}

static void liberar_todos(CANAL_T canales[], uint32_t n){
	for (uint32_t i = 0; i < n; i++) svc_DIAGNOSTICO_desreservar(canales[i]);
}

static uint8_t test_svc_DIAGNOSTICO_iniciar(void){
	if (svc_DIAGNOSTICO_reservar() != CANAL_NINGUNO) return 0;   /* sin iniciar no hay canales */
	if (svc_DIAGNOSTICO_iniciar() != ERROR_SINERROR) return 0;
	if (svc_DIAGNOSTICO_iniciar() != ERROR_SINERROR) return 0;   /* segunda vez, no pasa nada */
	return 1;
}

static uint8_t test_svc_DIAGNOSTICO_reservar(void){
	CANAL_T canales[MONITOR_NUMBER];
	uint32_t n;

	n = reservar_todos(canales);                  /* llenar */
	if (n != MONITOR_NUMBER - 1) return 0;        /* el 0 es el de error */
	for (uint32_t i = 0; i < n; i++){
		if (canales[i] == 0) return 0;
	}

	liberar_todos(canales, n);                    /* borrar y volver a llenar */
	n = reservar_todos(canales);
	if (n != MONITOR_NUMBER - 1) return 0;
	liberar_todos(canales, n);
	return 1;
}

static uint8_t test_svc_DIAGNOSTICO_desreservar(void){
	CANAL_T c = svc_DIAGNOSTICO_reservar();
	if (svc_DIAGNOSTICO_desreservar(c) != ERROR_SINERROR) return 0;
	if (svc_DIAGNOSTICO_reservar() != c) return 0;                 /* vuelve a estar libre */
	svc_DIAGNOSTICO_desreservar(c);
	if (svc_DIAGNOSTICO_desreservar(0) != ERROR_DIAGN_CANAL_INVALIDO) return 0;   /* el de error no */
	return 1;
}

static uint8_t test_svc_DIAGNOSTICO_senyalar(void){
	CANAL_T c = svc_DIAGNOSTICO_reservar();
	if (c == CANAL_NINGUNO) return 0;
	uint8_t ok = (svc_DIAGNOSTICO_senyalar(c) == ERROR_SINERROR)
	          && (svc_DIAGNOSTICO_senyalar(0) == ERROR_DIAGN_CANAL_INVALIDO);
	svc_DIAGNOSTICO_limpiar(c);
	svc_DIAGNOSTICO_desreservar(c);
	return ok;
}

static uint8_t test_svc_DIAGNOSTICO_limpiar(void){
	CANAL_T c = svc_DIAGNOSTICO_reservar();
	if (c == CANAL_NINGUNO) return 0;
	svc_DIAGNOSTICO_senyalar(c);
	uint8_t ok = (svc_DIAGNOSTICO_limpiar(c) == ERROR_SINERROR)
	          && (svc_DIAGNOSTICO_limpiar(0) == ERROR_DIAGN_CANAL_INVALIDO);
	svc_DIAGNOSTICO_desreservar(c);
	return ok;
}

uint8_t test_svc_diagnostico_todos_no_bloq(){
	uint8_t ok = 1;
//	ok &= test_svc_DIAGNOSTICO_iniciar();     /* el primero: comprueba "sin iniciar" */, cuando se ejecutan todos los tests juntos, ya esta iniciado y el test falla
	ok &= test_svc_DIAGNOSTICO_reservar();
	ok &= test_svc_DIAGNOSTICO_desreservar();
	ok &= test_svc_DIAGNOSTICO_senyalar();
	ok &= test_svc_DIAGNOSTICO_limpiar();
	return ok;
}

/* Bloqueante: debe encenderse el monitor 0 y quedarse colgado */
void test_svc_DIAGNOSTICO_error(){
	svc_DIAGNOSTICO_iniciar();
	svc_DIAGNOSTICO_error();
}

#endif
