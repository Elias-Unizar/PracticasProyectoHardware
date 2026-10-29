#include "nrf52840.h"
#include "hal_consumo.h"

void hal_consumo_iniciar(){
}


void hal_consumo_esperar(){
	__WFI();
}


void hal_consumo_dormir(){
	__WFI();
}