#include "rt_fifo.h"
#include "drv_tiempo.h"
#include "drv_monitor.h"

typedef uint8_t indice_cola_t; //Tenemos una cola de 32 eventos, con este tipo de indice el maximo serían: 256 eventos
#define MAX_EVENTOS 32
//Creacion de la FIFO interna del runtime
static EVENTO ev_FIFO[MAX_EVENTOS];
static MONITOR_id_t overflowMonitor;
indice_cola_t siguiente_a_tratar; //Post-indexado para obtener valor de la cola
indice_cola_t ultimo_insertado; //Pre-indexado para insertar uno nuevo
static uint32_t tamagnoActualCola;
// 10 - 20
// 
//variables internas de estadisticas
static uint32_t veces_ejecutadas_EVENTO_T[EVENT_TYPES];

void rt_FIFO_inicializar(MONITOR_id_t monitor_overflow){
	siguiente_a_tratar = 0; //como el siguiente a tratar seria la posicion 0 del vector local, en una cola circular la posicion anterior al inicio es el ultimo elemento
	ultimo_insertado = MAX_EVENTOS-1;//Que inserte desde el principio, ya que estamos para pedir
	tamagnoActualCola = 0;
	overflowMonitor = monitor_overflow;
}
//uint32 o TIPO EVENTO
void rt_FIFO_encolar(uint32_t ID_evento, uint32_t auxData){
	if(ID_evento >= EVENT_TYPES){return;}
	tamagnoActualCola += 1;
	if(tamagnoActualCola <= MAX_EVENTOS){
		Tiempo_us_t eventoTS = drv_tiempo_actual_us();
		ultimo_insertado = uint8_t(ultimo_insertado + 1) % MAX_EVENTOS;
		EVENTO eventoAInsertar = {ID_evento, auxData, eventoTS};
		ev_FIFO[ultimo_insertado] = eventoAInsertar;
		veces_ejecutadas_EVENTO_T[ID_evento] += 1;
	}else{
		drv_monitor_marcar(overflowMonitor);
		while(1){;}
	}
}

uint8_t rt_FIFO_extraer(uint32_t *ID_evento, uint32_t* auxData, Tiempo_us_t *TS){
	if(tamagnoActualCola == 0){return 0;}
	*ID_evento = ev_FIFO[siguiente_a_tratar].ID_EVENTO;
	*auxData = ev_FIFO[siguiente_a_tratar].auxData;
	*TS = ev_FIFO[siguiente_a_tratar].TS;
	tamagnoActualCola -= 1;
	return tamagnoActualCola+1; //
}

uint32_t rt_FIFO_estadisticas(EVENTO_T ID_evento){
	return veces_ejecutadas_EVENTO_T[ID_evento];
}
