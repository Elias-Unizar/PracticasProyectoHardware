#include <LPC210x.H>
#include "hal_ext_int.h"
/**
 * @brief Inicializa el perif?rico GPIOTE y la ISR.
 * @param f_callback_isr Puntero a la funci?n (drv_botones_callback)
 * que debe llamar la ISR cuando ocurra un evento.
 */
void hal_ext_int_iniciar(void (*f_callback_isr)(uint32_t pin));

/**
 * @brief Configura un pin para que genere una interrupci?n.
 * @param pin El pin a configurar (ej. 13)
 */
void hal_ext_int_config_pin(uint32_t pin){
	// Un dato interesante, no necesito asignar a EINT0 una ISR ya que no me interesa ejecutar nada especialmente, solo despertar al procesador. 
	// SI se quisiera, se podria asignar una, desactivarla al llamar a sleep y reactivarla al salir
	//Como usare el EXTINT0 para poder despertar al procesador, seria inconveniente que lo usaran a la vez para otras cosas, no?
	//EXTMODE |= EXTMODE 0x1 //--solo en flancos, no permitir que el boton importe si se mantiene pulasado
	//VICIntEnClr = 1 << 14; al parecer es solo en SLEEP, en ESPERAR tiene que haber una INT
	//VICVectAddr4 = (unsigned long)EINT0_IRQ;
	//VICVectCntl4 = 0x20 | 14; 
	PINSEL1 |= 0x01; //Activar el EINT0 en vez de GPIO pin 16
	EXTINT = 1; //Quitamos activacion anterior por sia caso

}
	

/**
 * @brief Deshabilita la interrupci?n para un pin.
 */
void hal_ext_int_deshabilitar(uint32_t pin);

/**
 * @brief Habilita la interrupci?n para un pin.
 */
void hal_ext_int_habilitar(uint32_t pin);