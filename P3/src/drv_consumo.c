
#include "drv_consumo.h"
#include "hal_consumo.h"
#include "drv_monitor.h"

static Monitor_id_t int_monitorDeConsumo;

void drv_consumo_iniciar(Monitor_id_t monitorConsumo){
	hal_consumo_iniciar();
	int_monitorDeConsumo = monitorConsumo;
	drv_monitor_marcar(int_monitorDeConsumo);
}


void drv_consumo_esperar(){
	drv_monitor_desmarcar(int_monitorDeConsumo);
	hal_consumo_esperar();
	drv_monitor_marcar(int_monitorDeConsumo);
}


void drv_consumo_dormir(){
	drv_monitor_desmarcar(int_monitorDeConsumo);
	hal_consumo_dormir();
	drv_monitor_marcar(int_monitorDeConsumo);
}
