#include <LPC210x.H>
#include "hal_ext_int.h"
#include "board.h"
/**
 * @brief Inicializa el perif?rico GPIOTE y la ISR.
 * @param f_callback_isr Puntero a la funci?n (drv_botones_callback)
 * que debe llamar la ISR cuando ocurra un evento.
 */
 
void (*funcion_ISR)(uint32_t);
 
void EXTINT_ISR(void) __irq {
	//Añadida esta linea, porque en el LPC(prob tmb en el nrf) el bit de EXTINT es hardware, se mantiene en 1 todo el rato mientras está pulsado, aunque se limpie
	//Por tanto revisamo QUE botones PUEDEN haber activado la ISR, y luego vemos que BOTONES ACTIVOS están pulsados, ejecutamos solo 1 boton por ISR
	//De hecho, modificacion, si se pulsan dos botones a la vez(o casi casi), se activa la ISR mientras este EXTINT activo y vic tambien
	uint32_t habilitadas = (VICIntEnable >> 14) & 0x7;
	uint32_t pinActivado = EXTINT & habilitadas;
	if((pinActivado & 0x1) != 0){//EXTINT0
		funcion_ISR(16);//pin del EXTINT0
	}else if((pinActivado & 0x2) != 0){//EXTINT1
		funcion_ISR(14);//pin del EXTINT1
	}else if((pinActivado & 0x4) != 0){//EXTINT2
		funcion_ISR(15);//pin del EXTINT2
	}
	VICVectAddr = 0;
}

static uint8_t iniciado = 0;
void hal_ext_int_iniciar(void (*f_callback_isr)(uint32_t pin)){
	if(iniciado == 0){
		funcion_ISR = f_callback_isr;
		VICVectCntl4 = 0x20 | 14; //Si pudiera, colocaria en el mismo VICCntl4 las 3 fuentes, 14, 15 y 16, pero no puedo
		VICVectAddr4 = (unsigned long)EXTINT_ISR;
		VICVectCntl5 = 0x20 | 15;
		VICVectAddr5 = (unsigned long)EXTINT_ISR;
		VICVectCntl6 = 0x20 | 16;
		VICVectAddr6 = (unsigned long)EXTINT_ISR;
		
		EXTINT = 7;//Todas las interrupciones cleareadas
		VICIntEnClr = 7 << 14;
		EXTWAKE = 0;
		iniciado = 1;
	}
}

/**
 * @brief Configura un pin para que genere una interrupci?n.
 * @param pin El pin a configurar (ej. 13) COMIENZAN DESHABILITADAS
 */
void hal_ext_int_config_pin(uint32_t pin){
	if(pin >= 14 && pin <= 16){
		if(pin == 16){//EINT0
			VICIntEnClr = 1 << 14;
			EXTINT = 1; //Quitamos activacion anterior del EINT0
			PINSEL1 |= 1;
		}else if(pin == 14){//EINT1
			VICIntEnClr = 1 << 15;
			EXTINT = 2; //Quitamos activacion anterior del EINT1
			PINSEL0 |= (0x2 << (pin*2));
		}else{//pin 15, EINT2
			VICIntEnClr = 1 << 16;
			EXTINT = 4; //Quitamos activacion anterior del EINT2
			PINSEL0 |= (0x2 << (pin*2));
		}
	}
}

/**
 * @brief Deshabilita la interrupci?n para un pin.
 */
void hal_ext_int_deshabilitar(uint32_t pin){
	if(pin >= 14 && pin <= 16){
		if(pin == 16){//EINT0
			VICIntEnClr = 1 << 14;
			EXTINT = 1; //Quitamos activacion anterior del EINT0
		}else if(pin == 14){//EINT1
			VICIntEnClr = 1 << 15;
			EXTINT = 2; //Quitamos activacion anterior del EINT1 o EINT2(Sale )
		}else{//pin 15, EINT2
			VICIntEnClr = 1 << 16;
			EXTINT = 4; //Quitamos activacion anterior del EINT1 o EINT2(Sale )
		}
	}
}

/**
 * @brief Habilita la interrupci?n para un pin.
 */
void hal_ext_int_habilitar(uint32_t pin){
	if(pin >= 14 && pin <= 16){
		if(pin == 16){//EINT0
			EXTINT = 1; //Quitamos activacion anterior del EINT0
			VICIntEnable = 1 << 14;
		}else if(pin == 14){//EINT1
			EXTINT = 2; //Quitamos activacion anterior del EINT1 o EINT2(Sale )
			VICIntEnable = 1 << 15;
		}else{//pin 15, EINT2
			EXTINT = 4; //Quitamos activacion anterior del EINT1 o EINT2(Sale )
			VICIntEnable = 1 << 16;
		}
	}
}

void hal_ext_int_habilitar_despertar(uint32_t pin){
	if(pin >= 14 && pin <= 16){
		if(pin == 16){
			EXTWAKE |= 0x1;
		}else{
			EXTWAKE |= (pin-13)*2;
		}
	}
}

void hal_ext_int_deshabilitar_despertar(uint32_t pin){
	if(pin >= 14 && pin <= 16){
		if(pin == 16){
			EXTWAKE = EXTWAKE & ~(0x1);
		}else{
			EXTWAKE = EXTWAKE & ~((pin-13)*2);
		}
	}
}


uint32_t hal_ext_int_activo(uint32_t pin){
	if(pin >= 14 && pin <= 16){
		if(pin == 16){//EINT0
			EXTINT = 1; 
			return (EXTINT & 1) != 0;
		}else if(pin == 14){//EINT1
			EXTINT = 2;
			return (EXTINT & 2) != 0;
		}else{//pin 15, EINT2
			EXTINT = 4;
			return (EXTINT & 4) != 0;
		}
	}
	return 0;
}
