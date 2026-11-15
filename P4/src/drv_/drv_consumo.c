#include "drv_consumo.h"
#include "hal_consumo.h"

static CANAL_T canalConsumo;

uint8_t iniciado = 0;

err_t drv_consumo_iniciar(){
	if(iniciado != 0){return ERROR_SINERROR;}
	svc_DIAGNOSTICO_iniciar();
	hal_consumo_iniciar();
	canalConsumo = svc_DIAGNOSTICO_reservar();
	svc_DIAGNOSTICO_senyalar(canalConsumo);
	iniciado = 1;
	return ERROR_SINERROR;
}


void drv_consumo_esperar(){
	svc_DIAGNOSTICO_limpiar(canalConsumo);
	hal_consumo_esperar();
	svc_DIAGNOSTICO_senyalar(canalConsumo);
}


void drv_consumo_dormir(){
	svc_DIAGNOSTICO_limpiar(canalConsumo);
	hal_consumo_dormir();
	svc_DIAGNOSTICO_senyalar(canalConsumo);
}
