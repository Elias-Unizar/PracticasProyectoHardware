#include "nrf52840.h"
#include "hal_consumo.h"
#include "nrf52840_bitfields.h"

void hal_consumo_iniciar(){
}


void hal_consumo_esperar(){
	__WFI();
}


void hal_consumo_dormir(){
	NRF_POWER->SYSTEMOFF=POWER_SYSTEMOFF_SYSTEMOFF_Enter;
	while(1){
		__WFE();
	}; //Depuración, en modo debug la CPU larpea dormir y solo se apaga de verdad con WFE, despertándose cuando llega un evento que atender
}