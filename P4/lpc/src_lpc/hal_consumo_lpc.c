#include "hal_consumo.h"
#include <LPC210x.H>

extern void switch_to_PLL(void);

void EINT0_IRQ(void) __irq {
	VICVectAddr = 0;
	EXTINT = 1;
}

void hal_consumo_iniciar(){}


void hal_consumo_esperar(){
	PCON = 0x01;
}


void hal_consumo_dormir(){
	PCON = 0x02; 
	switch_to_PLL();
	Reset_Handler();
}
