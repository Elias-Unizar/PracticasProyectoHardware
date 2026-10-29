#include "hal_consumo.h"
#include <LPC210x.H>

extern void switch_to_PLL(void);

void EINT0_IRQ(void) __irq {
	VICVectAddr = 0;
	EXTINT = 1;
}

void hal_consumo_iniciar(){
	// Un dato interesante, no necesito asignar a EINT0 una ISR ya que no me interesa ejecutar nada especialmente, solo despertar al procesador. 
	// SI se quisiera, se podria asignar una, desactivarla al llamar a sleep y reactivarla al salir
	//Como usare el EXTINT0 para poder despertar al procesador, seria inconveniente que lo usaran a la vez para otras cosas, no?
	//EXTMODE |= EXTMODE 0x1 //--solo en flancos, no permitir que el boton importe si se mantiene pulasado
	//VICIntEnClr = 1 << 14; al parecer es solo en SLEEP, en ESPERAR tiene que haber una INT
	//VICVectAddr4 = (unsigned long)EINT0_IRQ;
	//VICVectCntl4 = 0x20 | 14; 
	PINSEL1 |= 0x01; //Activar el EINT0 en vez de GPIO pin 16
	EXTINT = 1; //Quitamos activacion anterior por sia caso
	EXTWAKE = 1; // EXTINT0 will awake the processor, mi querido boton 3; valores [1, 2, o 4]
	//PLL maximo de 75MHz, pero el minimo son 10MHz 
	//Locura que es el PLL con multiplicaciones y divisiones de frecuencias jajaja
	//Futura posibiblidad de ahorrar energia con PCONP = Perifericos a activar y desactivar
}


void hal_consumo_esperar(){
	PCON = 0x01;
}


void hal_consumo_dormir(){
	EXTINT = 1;
	PCON = 0x02; 
	switch_to_PLL();
}
