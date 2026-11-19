#include "rt_GE.h"
#include "drv_consumo.h"
#include "rt_fifo.h"
#include "svc_alarma.h"
#include "bot

#define rt_GE_MAX_SUSCRITOS 4

typedef struct{
	CALLBACK_T funcion;
	uint32_t prioridad;
}funcionEvento;


static uint8_t eventosOcupados[EVENT_TYPES+1];
static uint32_t eventoNumeroSuscritos[EVENT_TYPES+1];
static funcionEvento eventoFuncionesCallback[EVENT_TYPES+1][rt_GE_MAX_SUSCRITOS];
//TODAS LAS FUNCIONES ASIGNADAS AL RT DEBEN SER DE LA FORMA VOID FUNC(EVENTO, UINT32_T)

static EVENTO_T ev_USUARIO;
static EVENTO_T ev_INACTIVIDAD;

//IMPLEMENTACION DE FUNCIONES DE SVC_GE.H
static uint8_t ya_iniciado = 0;

err_t svc_GE_iniciar(){
	if(ya_iniciado != 0){return ERROR_SINERROR;}
	svc_DIAGNOSTICO_iniciar();
	
	for (uint32_t i = 0; i < EVENT_TYPES+1; i++){
		for (uint32_t j = 0; j < rt_GE_MAX_SUSCRITOS; j++){
			eventoFuncionesCallback[i][j].funcion = NULL;
		}
		eventoNumeroSuscritos[i] = 0;
		eventosOcupados[i] = 0;//Igual que el VOID, pero como el void nunca se movera de la posicion 0, funcionara para indicar que todavia no hay evento
	}
	eventosOcupados[ev_VOID] = 1;
	ya_iniciado = 1;
	return ERROR_SINERROR;
}

EVENTO_T svc_GE_reservar_ID(){
	uint32_t evento = 1;
	while(evento < EVENT_TYPES+1 && eventosOcupados[evento] != 0){
		evento++;
	}
	if(evento < EVENT_TYPES+1){
		for (uint32_t j = 0; j < rt_GE_MAX_SUSCRITOS; j++){
			eventoFuncionesCallback[evento][j].funcion = NULL;
		}
		eventosOcupados[evento] = 1;
		eventoNumeroSuscritos[evento] = 0;
		return (EVENTO_T)evento;
	}else{
		svc_DIAGNOSTICO_error();
		return ev_VOID;
	}
}

err_t svc_GE_desreservar_ID(EVENTO_T ID_evento){
	if(eventosOcupados[ID_evento] == 0 || ID_evento == ev_VOID){
		return ERROR_EVENTO_NO_RESERVADO;
	}
	eventosOcupados[ID_evento] = 0;
	return ERROR_SINERROR;
}

void insertarEnMedio(funcionEvento vector[], uint32_t indiceComienzo, funcionEvento *funcInsertar){
	funcionEvento auxGuardar;
	funcionEvento auxSustituir = *funcInsertar;
	for(uint32_t i = indiceComienzo; i < rt_GE_MAX_SUSCRITOS; i++){
		auxGuardar = vector[i];
		vector[i] = auxSustituir;
		auxSustituir = auxGuardar;
	}
}

err_t svc_GE_suscribir(EVENTO_T ID_evento, uint8_t prioridad , CALLBACK_T f_callback){
	if(eventosOcupados[ID_evento] == 0){
		return ERROR_EVENTO_NO_RESERVADO;
	}
	uint32_t funcSusc = eventoNumeroSuscritos[ID_evento];
	if(funcSusc < rt_GE_MAX_SUSCRITOS){	
		funcionEvento funcionAIns = {f_callback, prioridad};
		uint8_t insertado = 0;
		for(uint32_t i = 0; i < funcSusc; i++){
			if(funcionAIns.prioridad < eventoFuncionesCallback[ID_evento][i].prioridad){
				insertado = 1;
				insertarEnMedio(eventoFuncionesCallback[ID_evento], i, &funcionAIns);
				break;
			}
		}
		if(insertado == 0){//No se inserta en medio, sino al final
			eventoFuncionesCallback[ID_evento][funcSusc] = funcionAIns;
		}
		//Aqui deberia haberse insertado en algun momento
		eventoNumeroSuscritos[ID_evento]++;
		return ERROR_SINERROR;
	}else{
		svc_DIAGNOSTICO_error();
		return ERROR_EVENTO_OVERFLOW_SUSCRITOS;
	}
}

void compactarVectorFunciones(funcionEvento vector[], uint32_t indiceComienzo, uint32_t max){
	for(uint32_t i = indiceComienzo; i < max-1; i++){
		vector[i] = vector[i+1];
	}
}

err_t svc_GE_cancelar(EVENTO_T ID_evento, CALLBACK_T f_callback){
	if(eventosOcupados[ID_evento] == 0){
		return ERROR_EVENTO_NO_RESERVADO;
	}
	int8_t compactado = 0;
	for(uint32_t i = 0; i < eventoNumeroSuscritos[ID_evento]; i++){
		if(f_callback == eventoFuncionesCallback[ID_evento][i].funcion){
			compactarVectorFunciones(eventoFuncionesCallback[ID_evento], i, eventoNumeroSuscritos[ID_evento]);
			compactado = 1;
			break;
		}
	}
	if(compactado == 1){
		eventoNumeroSuscritos[ID_evento]--;
		return ERROR_SINERROR;
	}else{
		return ERROR_EVENTO_FUNCION_INEXISTENTE;
	}
}

//IMPLEMENTACION DE FUNCIONES DE RT_GE.H

void rt_GE_actualizar(EVENTO_T ID_evento, uint32_t auxiliar){
	if(ID_evento == ev_INACTIVIDAD){
		drv_botones_preparar_despertar();
		drv_consumo_dormir();
	}else if(ID_evento == ev_USUARIO){
		svc_alarma_activar(svc_alarma_codificar(0, 20*1000, 0), ev_INACTIVIDAD, 0); 
	}else{
		//No coincide, algun error en algun svc_subscribir, o no se han reservado bien los eventos
	}
}

err_t rt_GE_ejecutarFuncionesSubscritas(EVENTO *eventoExtraido){
	if(eventosOcupados[(*eventoExtraido).ID_EVENTO] == 0){
		return ERROR_EVENTO_NO_RESERVADO;
	}
	for(uint32_t i = 0; i < eventoNumeroSuscritos[(*eventoExtraido).ID_EVENTO]; i++){
		eventoFuncionesCallback[(*eventoExtraido).ID_EVENTO][i].funcion((*eventoExtraido).ID_EVENTO, (*eventoExtraido).auxData);
	}
	return ERROR_SINERROR;
}

void rt_GE_lanzador(){
	svc_alarma_activar(svc_alarma_codificar(0, 20*1000, 0), ev_INACTIVIDAD, 0); 
	EVENTO eventoExtraido;
	while(1){
		if(rt_FIFO_extraer(&eventoExtraido) != 0){
			rt_GE_ejecutarFuncionesSubscritas(&eventoExtraido);
		}else{
			drv_consumo_esperar();
		}
	}
}

void rt_GE_iniciar(){
	rt_FIFO_inicializar();
	svc_alarma_iniciar(rt_FIFO_encolar);
	svc_GE_iniciar();
	drv_consumo_iniciar();
	
	ev_USUARIO = svc_GE_reservar_ID();
	ev_INACTIVIDAD = svc_GE_reservar_ID();
	//Si no se han podido reservar, han marcado ya internamente el canal de error
	svc_GE_suscribir(ev_USUARIO, 5, rt_GE_actualizar);
	svc_GE_suscribir(ev_INACTIVIDAD, 0, rt_GE_actualizar);
}





/*Alarma en 2s -> ev_ALARMA
Boton se pulsa -> ev_BOTON_PULSADO -> svc_alarma_activar(mismo evento, mismo aux) = sobreescribe la alarma anterior
*/
