#include "svc_alarma.h"
#include "svc_GE.h"

#define svc_ALARMAS_MAX 4
err_t ERROR_ALARMA_DESBORDE_ALARMA;

typedef struct{
	bool activa;
	Tiempo_ms_t retardo_ms;
	Tiempo_ms_t tiempoRestante_ms;
	bool esPeriodica;
	EVENTO_T idEvento;
	uint32_t auxData;
}ALARMA;

static ALARMA alarmasActivas[svc_ALARMAS_MAX];

static EVENTO_T ev_A_PERIODICA;
static Tiempo_ms_t periodo_timer_periodico;
static Tiempo_ms_t ultimaActualizacion;
static void (*f_cb)();

//-----INTERNA----------//
//---_CODIFICACION DE FLAGS 32B -> 1b = PERIODICA 7b = flags extra 24b = ms
void decodificar_flags(uint32_t alarma_flags, bool *esPeriodico, Tiempo_ms_t *retardo_ms, uint8_t *flags){
	*esPeriodico = ((alarma_flags & 0x80000000) == 0) ? false : true;
	*flags = (alarma_flags & 0x7F000000) >> 24;
	*retardo_ms = (Tiempo_ms_t)(alarma_flags & 0x00FFFFFF);
}

void programarAlarma(uint32_t indiceAlarma, uint32_t alarma_flags, EVENTO_T id_evento, uint32_t auxData){
	bool esPeriodico;
	Tiempo_ms_t retardo_ms;
	uint8_t flags;
	decodificar_flags(alarma_flags, &esPeriodico, &retardo_ms, &flags);
	if(retardo_ms == 0){
		alarmasActivas[indiceAlarma].activa = false;
	}else{
		alarmasActivas[indiceAlarma].activa = true;
		alarmasActivas[indiceAlarma].auxData = auxData;
		alarmasActivas[indiceAlarma].esPeriodica = esPeriodico;
		alarmasActivas[indiceAlarma].idEvento = id_evento;
		alarmasActivas[indiceAlarma].retardo_ms = retardo_ms;
		alarmasActivas[indiceAlarma].tiempoRestante_ms = retardo_ms;
	}
}

Tiempo_ms_t recalcularTimerPeriodico(){
	Tiempo_ms_t periodoMinimo = 0x00FFFFFF;
	for(uint32_t i = 0; i < svc_ALARMAS_MAX; i++){
		if(alarmasActivas[i].activa == true && alarmasActivas[i].tiempoRestante_ms < periodoMinimo){
			periodoMinimo = alarmasActivas[i].tiempoRestante_ms;
		}
	}
	return periodoMinimo;
}

void actualizarTimers(Tiempo_ms_t tiempoTranscurrido){
	for(uint32_t i = 0; i < svc_ALARMAS_MAX; i++){
		if(alarmasActivas[i].activa == true){
			if(alarmasActivas[i].tiempoRestante_ms < tiempoTranscurrido){
				alarmasActivas[i].tiempoRestante_ms = 0;//Por si nos hemos atrasado, y hay algo negativo
			}else{
				alarmasActivas[i].tiempoRestante_ms -= tiempoTranscurrido;//En principio no habria tiempos negativos, si los hay es que nos hemos retrasado en activar timers
			}
		}
	}
}

//-------FUNCIONES EXTERNAS---------//
err_t svc_alarma_iniciar(void (*cb_a_llamar)()){
	ev_A_PERIODICA = svc_GE_reservar_ID();
	svc_GE_suscribir(ev_A_PERIODICA, 0, svc_alarma_actualizar);
	periodo_timer_periodico = 0x00FFFFFF;//Si no hay alarmas, que no moleste mucho a ser posible
	f_cb = cb_a_llamar;
	drv_tiempo_periodico_ms(periodo_timer_periodico, f_cb, ev_A_PERIODICA);
	ultimaActualizacion = drv_tiempo_actual_ms();
}

uint32_t svc_alarma_codificar(bool periodico, Tiempo_ms_t retardo_ms, uint8_t flags){
	uint32_t flag_final = (periodico == true) ? 0x80000000 : 0x00000000;
	flag_final |= (flags << 24);
	flag_final |= retardo_ms;
	return flag_final;
}

err_t svc_alarma_activar(uint32_t alarma_flags, EVENTO_T ID_evento, uint32_t auxData){
	int32_t hueco_alarma = -1;
	int32_t alarma_encontrada = -1;
	for(uint32_t i = 0; i < svc_ALARMAS_MAX; i++){
		if(alarmasActivas[i].activa == false && hueco_alarma == -1){
			hueco_alarma = i;//El primer hueco encontrado
		}
		if(alarmasActivas[i].idEvento == ID_evento){
			alarma_encontrada = i;
			break;
		}
	}
	//Aqui, o i es mayor a ALARMAS_MAX, o es la posicion de una alarma libre
	if(alarma_encontrada != -1){//reprogramar
		programarAlarma(alarma_encontrada, alarma_flags, ID_evento, auxData);
	}else if(hueco_alarma != -1){//crear nueva, pero hay hueco
		programarAlarma(hueco_alarma, alarma_flags, ID_evento, auxData);		
	}else{//No hay hueco, ni se reprograma
		return ERROR_ALARMA_DESBORDE_ALARMA;
	}
	Tiempo_ms_t actAnt = ultimaActualizacion;
	ultimaActualizacion = drv_tiempo_actual_ms();
	actualizarTimers(ultimaActualizacion - actAnt);
	periodo_timer_periodico = recalcularTimerPeriodico();
	drv_tiempo_periodico_ms(periodo_timer_periodico, f_cb, ev_A_PERIODICA);
	return 0;
}

void svc_alarma_actualizar(EVENTO_T evento, uint32_t aux){
	if(evento == ev_A_PERIODICA){
		actualizarTimers(periodo_timer_periodico);
		for(uint32_t i = 0; i < svc_ALARMAS_MAX; i++){
			if(alarmasActivas[i].activa == true && alarmasActivas[i].tiempoRestante_ms <= 0){
				f_cb(alarmasActivas[i].idEvento, alarmasActivas[i].auxData);
				if(!alarmasActivas[i].esPeriodica){
					alarmasActivas[i].activa = false;
				}else{
					alarmasActivas[i].tiempoRestante_ms = alarmasActivas[i].retardo_ms;
				}
			}
		}
		periodo_timer_periodico = recalcularTimerPeriodico();
		drv_tiempo_periodico_ms(periodo_timer_periodico, f_cb, ev_A_PERIODICA);
	}
}