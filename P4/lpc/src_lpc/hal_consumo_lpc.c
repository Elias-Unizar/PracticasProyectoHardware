#include "hal_consumo.h"
#include <LPC210x.H>

extern void switch_to_PLL(void);

void EINT0_IRQ(void) __irq {
	VICVectAddr = 0;
	EXTINT = 1;
}

void hal_consumo_iniciar(){
	EXTWAKE = 1; // EXTINT0 will awake the processor, mi querido boton 3; valores [1, 2, o 4]
	//PLL maximo de 75MHz, pero el minimo son 10MHz 
	//Locura que es el PLL con multiplicaciones y divisiones de frecuencias jajaja
	//Futura posibiblidad de ahorrar energia con PCONP = Perifericos a activar y desactivar
}


void hal_consumo_esperar(){
	PCON = 0x01;
}


void hal_consumo_dormir(){
	PCON = 0x02; 
	switch_to_PLL();
}
