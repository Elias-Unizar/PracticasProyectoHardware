#include "svc_alarma.h"
#include "svc_GE.h"

#define svc_ALARMAS_MAX 4
typedef struct{
	bool activa;
	Tiempo_ms_t retardo_ms;
	Tiempo_ms_t tiempoRestante_ms;
	bool esPeriodica;
	EVENTO_T idEvento;
	uint32_t auxData;
}ALARMA;

static ALARMA alarmasActivas[svc_ALARMAS_MAX];

//-----INTERNA----------//
void decodificar_flags(uint32_t alarma_flags, bool *esPeriodico, Tiempo_ms_t *retardo_ms, uint8_t *flags){
}


err_t svc_alarma_iniciar(void (*cb_a_llamar)());

uint32_t svc_alarma_codificar(bool periodico, Tiempo_ms_t retardo_ms, uint8_t flags);

void  svc_alarma_activar(uint32_t alarma_flags, EVENTO_T ID_evento, uint32_t auxData);

void svc_alarma_actualizar(EVENTO_T evento, uint32_t aux);