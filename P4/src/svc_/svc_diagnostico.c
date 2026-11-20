#include "svc_diagnostico.h"
#include "drv_monitor.h"
#include "board.h"

#define MAX_MONITORES MONITOR_NUMBER
//Al final incluyo el board.h, total, ya se encuentra en todos los drivers.....
static uint32_t total_monitores = 0;
static MONITOR_id_t canalError;
uint8_t monitorOcupado[MAX_MONITORES];

static uint8_t iniciado = 0;

err_t svc_DIAGNOSTICO_iniciar(){
	if(iniciado != 0){return ERROR_SINERROR;}
	total_monitores = drv_monitor_iniciar();
	
	if (total_monitores == 0 || total_monitores > MAX_MONITORES){
		return ERROR_DIAGN_0_N_MONITORES;
	}
	for (uint8_t i = 0; i < total_monitores; i++){
		monitorOcupado[i] = 0;
	}
	canalError = 0;
	monitorOcupado[canalError] = 1;//Canal de error
	iniciado = 1;
	return ERROR_SINERROR;
}
 
CANAL_T svc_DIAGNOSTICO_reservar(){
	if(total_monitores > 0){
		uint32_t monitor = 1;
		while(monitor < total_monitores && monitorOcupado[monitor] != 0){
			monitor++;
		}//Cuando sale del bucle, monitor es la posicion del vector que corresponde con un monitor sin asignar
		if (monitor < total_monitores){//Ha conseguido un monitor
			monitorOcupado[monitor] = 1;
			return (CANAL_T)monitor;
		}
	}
	svc_DIAGNOSTICO_error();
	return 0;//O sin canales, o sin iniciar
}

err_t svc_DIAGNOSTICO_desreservar(CANAL_T canal){
	err_t err = svc_DIAGNOSTICO_limpiar(canal) ;
	if(err != 0){
		return err;
	}
	monitorOcupado[canal] = 0;
	return ERROR_SINERROR;
}
 
err_t svc_DIAGNOSTICO_senyalar(CANAL_T canal){
	if(canal != 0){
		drv_monitor_marcar((MONITOR_id_t)(canal+1));
		return ERROR_SINERROR;
	}else{
		return ERROR_DIAGN_CANAL_INVALIDO;
	}
}
 
err_t svc_DIAGNOSTICO_limpiar(CANAL_T canal){
	if(canal != 0){
		drv_monitor_desmarcar((MONITOR_id_t)(canal+1));
		return ERROR_SINERROR;
	}else{
		return ERROR_DIAGN_CANAL_INVALIDO;
	}
}
	
	
void svc_DIAGNOSTICO_error(){
	if(total_monitores > 0){
		drv_monitor_marcar((MONITOR_id_t)(canalError+1));
		while(1){;}
	}
}
