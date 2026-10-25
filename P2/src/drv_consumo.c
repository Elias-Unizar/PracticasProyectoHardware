
#include "drv_consumo.h"
#include "hal_consumo.h"

Monitor_id_t monitorDeConsumo;

void drv_consumo_iniciar(Monitor_id_t monitorConsumo){
	hal_consumo_iniciar();
}


void drv_consumo_esperar(){
	hal_consumo_esperar();
}


void drv_consumo_dormir(){
	hal_consumo_dormir();
}
