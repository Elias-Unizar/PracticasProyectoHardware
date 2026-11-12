#include "rt_GE.h"
#include "drv_consumo.h"
#include "rt_fifo.h"

#define rt_GE_MAX_SUSCRITOS 4
err_t ERROR_EVENTO_NO_RESERVADO = -1;
err_t ERROR_EVENTO_OVERFLOW_SUSCRITOS = -2;
err_t ERROR_EVENTO_FUNCION_INEXISTENTE = -3;

typedef void (*CALLBACK_T)();
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

void svc_GE_iniciar(){
	for (uint32_t i = 0; i < EVENT_TYPES+1; i++){
		for (uint32_t j = 0; j < rt_GE_MAX_SUSCRITOS; j++){
			eventoFuncionesCallback[i][j].funcion = NULL;
		}
		eventoNumeroSuscritos[i] = 0;
		eventosOcupados[i] = 0;//Igual que el VOID, pero como el void nunca se movera de la posicion 0, funcionara para indicar que todavia no hay evento
	}
	eventosOcupados[ev_VOID] = 1;
}

EVENTO_T svc_GE_reservar_ID(){
	uint32_t evento = 0;
	while(evento < EVENT_TYPES+1 && eventosOcupados[evento] != 0){
		evento++;
	}
	if(evento < EVENT_TYPES+1){
		eventosOcupados[evento] = 1;
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
	for (uint32_t j = 0; j < rt_GE_MAX_SUSCRITOS; j++){
			eventoFuncionesCallback[ID_evento][j].funcion = NULL;
	}
	eventoNumeroSuscritos[ID_evento] = 0;
	eventosOcupados[ID_evento] = 0;
	return 0;
}

err_t svc_GE_suscribir(EVENTO_T ID_evento, uint8_t prioridad , void (*f_callback)()){
	if(eventosOcupados[ID_evento] == 0){
		return ERROR_EVENTO_NO_RESERVADO;
	}
	if(eventoFuncionesCallback[ID_evento][rt_GE_MAX_SUSCRITOS-1].funcion == NULL){//existe un hueco para la callback
		funcionEvento funcionMasPrio = {f_callback, prioridad};
		funcionEvento aux;
		for(uint32_t i = 0; i < rt_GE_MAX_SUSCRITOS-1; i++){
			if(funcionMasPrio.prioridad < eventoFuncionesCallback[ID_evento][i].prioridad){
				aux = eventoFuncionesCallback[ID_evento][i];
				eventoFuncionesCallback[ID_evento][i] = funcionMasPrio;
				funcionMasPrio = aux;
			}
		}
		return 0;
	}else{
		svc_DIAGNOSTICO_error();
		return ERROR_EVENTO_OVERFLOW_SUSCRITOS;
	}
}

err_t svc_GE_cancelar(EVENTO_T ID_evento, void (*f_callback)()){
	if(eventosOcupados[ID_evento] == 0){
		return ERROR_EVENTO_NO_RESERVADO;
	}
	int32_t compactado = -1;
	for(uint32_t i = 0; i < rt_GE_MAX_SUSCRITOS; i++){
		if(f_callback == eventoFuncionesCallback[ID_evento][i].funcion){
			compactado = i;
		}
	}
	if(compactado != -1){
		for(uint32_t i = compactado; i < rt_GE_MAX_SUSCRITOS-1; i++){
			eventoFuncionesCallback[ID_evento][i] = eventoFuncionesCallback[ID_evento][i+1];
			eventoFuncionesCallback[ID_evento][i+1].funcion = NULL;
			eventoFuncionesCallback[ID_evento][i].prioridad = -1;//Desborda a la peor prioridad por si acaso
		}
		return 0;
	}else{
		return ERROR_EVENTO_FUNCION_INEXISTENTE;
	}
}

//IMPLEMENTACION DE FUNCIONES DE RT_GE.H

void rt_GE_actualizar(EVENTO_T ID_evento, uint32_t auxiliar){
	if(ID_evento == ev_INACTIVIDAD){
		drv_consumo_dormir();
	}else if(ID_evento == ev_USUARIO){
		//reiniciar temporizador inactividad
	}else{
		//No coincide, algun error en algun svc_subscribir, o no se han reservado bien los eventos
	}
}

err_t rt_GE_ejecutarFuncionesSubscritas(EVENTO eventoExtraido){
	if(eventosOcupados[eventoExtraido.ID_EVENTO] == 0){
		return ERROR_EVENTO_NO_RESERVADO;
	}
	uint32_t i = 0;
	while(eventoFuncionesCallback[eventoExtraido.ID_EVENTO][i].funcion != NULL){
		eventoFuncionesCallback[eventoExtraido.ID_EVENTO][i].funcion(eventoExtraido.ID_EVENTO, eventoExtraido.auxData);
		i++;
	}
}

void rt_GE_lanzador(){
	//iniciar temporizador inactividad
	EVENTO eventoExtraido;
	while(1){
		if(rt_FIFO_extraer(&eventoExtraido) != 0){
			rt_GE_ejecutarFuncionesSubscritas(eventoExtraido);
		}else{
			drv_consumo_esperar();
		}
	}
}

void rt_GE_iniciar(){
	svc_GE_iniciar();
	ev_USUARIO = svc_GE_reservar_ID();
	ev_INACTIVIDAD = svc_GE_reservar_ID();
	//Si no se han podido reservar, han marcado ya internamente el canal de error
	err_t err;
	err = svc_GE_suscribir(ev_USUARIO, 5, rt_GE_actualizar);
	if(err == ERROR_EVENTO_NO_RESERVADO){svc_DIAGNOSTICO_error();}
	svc_GE_suscribir(ev_INACTIVIDAD, 0, rt_GE_actualizar);
	if(err == ERROR_EVENTO_NO_RESERVADO){svc_DIAGNOSTICO_error();}
}
