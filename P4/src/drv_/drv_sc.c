#include "drv_sc.h"
#include "hal_irq.h"

//En este, nuestro programa, tenemos SOLO 2 HILOS, irq y principal(usuario), la unica manera de acumular llamadas a SC, es solo en el hilo principal
//Y ya se encargara el hilo principal de guardar en variables locales el estado de la SC

uint32_t enterSC(void){
	return hal_irq_deshabilitar(); 
}

void exitSC(uint32_t previo){
	hal_irq_restaurar(previo);
}
