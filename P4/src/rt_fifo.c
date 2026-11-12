#include "rt_fifo.h"
#include "drv_tiempo.h"
#include "svc_diagnostico.h"

#define MAX_EVENTOS 32
typedef uint8_t indice_cola_t; //Tenemos una cola de 32 eventos, con este tipo de indice el maximo serían: 256 eventos


//Creacion de la FIFO interna del runtime
static EVENTO ev_FIFO[MAX_EVENTOS];
indice_cola_t siguiente_a_tratar; //Post-indexado para obtener valor de la cola
indice_cola_t ultimo_insertado; //Pre-indexado para insertar uno nuevo
static uint32_t tamagnoActualCola;

//variables internas de estadisticas
static uint32_t veces_ejecutadas_EVENTO_T[EVENT_TYPES];

void rt_FIFO_inicializar(){
	siguiente_a_tratar = 0; //como el siguiente a tratar seria la posicion 0 del vector local, en una cola circular la posicion anterior al inicio es el ultimo elemento
	ultimo_insertado = MAX_EVENTOS-1;//Que inserte desde el principio, ya que estamos para pedir
	tamagnoActualCola = 0;
}
//uint32 o TIPO EVENTO
void rt_FIFO_encolar(uint32_t ID_evento, uint32_t auxData){
	if(ID_evento >= EVENT_TYPES){return;}
	tamagnoActualCola += 1;
	if(tamagnoActualCola <= MAX_EVENTOS){
		Tiempo_us_t eventoTS = drv_tiempo_actual_us();
		ultimo_insertado = (ultimo_insertado + 1) % MAX_EVENTOS;
		EVENTO eventoAInsertar = {ID_evento, auxData, eventoTS};
		ev_FIFO[ultimo_insertado] = eventoAInsertar;
		veces_ejecutadas_EVENTO_T[ID_evento] += 1;
		veces_ejecutadas_EVENTO_T[ev_VOID] += 1;
	}else{
		svc_DIAGNOSTICO_error();
	}
}

uint8_t rt_FIFO_extraer(EVENTO *eventoAExtraer){
	if(tamagnoActualCola == 0){return 0;}
	*eventoAExtraer = ev_FIFO[siguiente_a_tratar];
	tamagnoActualCola -= 1;
	siguiente_a_tratar += 1;
	return tamagnoActualCola+1; //
}

uint32_t rt_FIFO_estadisticas(EVENTO_T ID_evento){
	return veces_ejecutadas_EVENTO_T[ID_evento];
}
