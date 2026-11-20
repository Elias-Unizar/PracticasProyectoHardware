#include "rt_fifo.h"
#include "svc_diagnostico.h"
#include "drv_sc.h"
typedef uint8_t indice_cola_t; //Tenemos una cola de 32 eventos, con este tipo de indice el maximo serían: 256 eventos

#define MAX_EVENTOS 32

//Creacion de la FIFO interna del runtime
static EVENTO ev_FIFO[MAX_EVENTOS];
static indice_cola_t siguiente_a_tratar; //Post-indexado para obtener valor de la cola
static indice_cola_t ultimo_insertado; //Pre-indexado para insertar uno nuevo
static uint32_t tamagnoActualCola;

//variables internas de estadisticas
static uint32_t veces_ejecutadas_EVENTO_T[EVENT_TYPES+1];

static uint8_t ya_iniciado = 0;

err_t rt_FIFO_inicializar(){
	if(ya_iniciado != 0){return ERROR_SINERROR;}
	drv_tiempo_iniciar();
	svc_DIAGNOSTICO_iniciar();
	siguiente_a_tratar = 0; //como el siguiente a tratar seria la posicion 0 del vector local, en una cola circular la posicion anterior al inicio es el ultimo elemento
	ultimo_insertado = MAX_EVENTOS-1;//Que inserte desde el principio, ya que estamos para pedir
	tamagnoActualCola = 0;
	ya_iniciado = 1;
	return ERROR_SINERROR;
}

void rt_FIFO_encolar(EVENTO_T ID_evento, uint32_t auxData){
	uint32_t estadoPrev = drv_sc_enterSC();
	if(tamagnoActualCola == MAX_EVENTOS){
		drv_sc_exitSC(estadoPrev);
		svc_DIAGNOSTICO_error();
	}else{
		tamagnoActualCola += 1;
		ultimo_insertado = (ultimo_insertado + 1) % MAX_EVENTOS;
		drv_sc_exitSC(estadoPrev);
		Tiempo_us_t eventoTS = drv_tiempo_actual_us();
		EVENTO eventoAInsertar = {ID_evento, auxData, eventoTS};		
		ev_FIFO[ultimo_insertado] = eventoAInsertar;
		veces_ejecutadas_EVENTO_T[ID_evento] += 1;
		veces_ejecutadas_EVENTO_T[ev_VOID] += 1;
	}
}

uint8_t rt_FIFO_extraer(EVENTO *eventoAExtraer){
	uint32_t estadoPrev = drv_sc_enterSC();
	if(tamagnoActualCola == 0){
		drv_sc_exitSC(estadoPrev);
		return 0;
	}
	tamagnoActualCola -= 1;
	drv_sc_exitSC(estadoPrev);
	*eventoAExtraer = ev_FIFO[siguiente_a_tratar];
	siguiente_a_tratar = (siguiente_a_tratar + 1) % MAX_EVENTOS;
	return tamagnoActualCola+1;
}

uint32_t rt_FIFO_estadisticas(EVENTO_T ID_evento){
	return veces_ejecutadas_EVENTO_T[ID_evento];
}
