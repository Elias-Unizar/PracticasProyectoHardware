
#include "drv_consumo.h"
#include "hal_consumo.h"
#include "drv_monitor.h"

static Monitor_id_t int_monitorConsumoEsperar;
static Monitor_id_t int_monitorConsumoDormir;

void drv_consumo_iniciar(Monitor_id_t monitorConsumoEsperar, Monitor_id_t monitorConsumoDormir){
	hal_consumo_iniciar();
	int_monitorConsumoEsperar = monitorConsumoEsperar;
	int_monitorConsumoDormir = monitorConsumoDormir;
	drv_monitor_marcar(int_monitorConsumoEsperar);
	drv_monitor_marcar(int_monitorConsumoDormir);
}


void drv_consumo_esperar(){
	drv_monitor_desmarcar(int_monitorConsumoEsperar);
	hal_consumo_esperar();
	drv_monitor_marcar(int_monitorConsumoEsperar);
}


void drv_consumo_dormir(){
	drv_monitor_desmarcar(int_monitorConsumoDormir);
	hal_consumo_dormir();
	drv_monitor_marcar(int_monitorConsumoDormir);
}
