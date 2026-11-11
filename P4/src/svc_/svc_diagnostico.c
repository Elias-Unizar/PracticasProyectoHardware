#include "svc_diagnostico.h"
#include "drv_monitor.h"
#define MAX_MONITORES 100
//Aunque sea un limite de 100, en principio siempre operaremos entre el numero maximo de monitores reales con una variable static
err_t ERROR_0_MONITORES = -1;
err_t ERROR_MONITOR_INVALIDO = -2;



static uint32_t total_monitores = 0;
static MONITOR_id_t canalError;
uint8_t monitorDisponible[MAX_MONITORES];

err_t svc_DIAGNOSTICO_iniciar(){
	total_monitores = drv_monitor_iniciar();
	if (total_monitores == 0){
		return ERROR_0_MONITORES;
	}
	for (uint8_t i = 0; i < total_monitores; i++){
		monitorDisponible[i] = 1;
	}
	canalError = 0;
	monitorDisponible[canalError] = 0;//Canal de error
	return 0;
}
 
CANAL_T svc_DIAGNOSTICO_reservar(){
	if(total_monitores > 0){
		uint32_t monitorLibre = 1;
		while(monitorLibre < total_monitores && monitorDisponible[monitorLibre] == 0){
			monitorLibre++;
		}//Cuando sale del bucle, monitorLibre es la posicion del vector que corresponde con un monitor sin asignar
		if (monitorLibre < total_monitores){//Ha conseguido un monitor
			monitorDisponible[monitorLibre] = 0;
			return (CANAL_T)monitorLibre;
		}
	}
	return 0;//O sin canales, o sin iniciar
}

err_t svc_DIAGNOSTICO_desreservar(CANAL_T canal){
	err_t err = svc_DIAGNOSTICO_limpiar(canal) ;
	if(err != 0){
		return err;
	}
	monitorDisponible[canal] = 1;
	return 0;
}
 
err_t svc_DIAGNOSTICO_senyalar(CANAL_T canal){
	if(canal != 0){
		drv_monitor_marcar((MONITOR_id_t)canal);
	}else{
		return ERROR_MONITOR_INVALIDO;
	}
}
 
err_t svc_DIAGNOSTICO_limpiar(CANAL_T canal){
	if(canal != 0){
		drv_monitor_desmarcar((MONITOR_id_t)canal);
	}else{
		return ERROR_MONITOR_INVALIDO;
	}
}
	
	
void svc_DIAGNOSTICO_error(){
	if(total_monitores > 0){
		drv_monitor_marcar(canalError);
		while(1){;}
	}
}