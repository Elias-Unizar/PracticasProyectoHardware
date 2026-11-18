/* *****************************************************************************
 * P.H.2025: TODO
 * implementacion para cumplir el hal_tiempo.h
 */

#include <nrf.h>
#include "hal_ext_int.h"
//Asocia canales GPIO(posiciones del vector) a los pines que contiene el vector
static uint32_t canalGPIOTE_pin[8];

static void (*funcion_ISR)(uint32_t) = 0;
//ISR del GPIOTE
void GPIOTE_IRQHandler(void) {
    uint32_t i;
    //Si estabamos dormidos, nos podemos haber despertado por esto, limpiamos interrupcion
    NRF_GPIOTE->EVENTS_PORT = 0u;  

    for (i = 0; i < 8; i++) {
			if (NRF_GPIOTE->EVENTS_IN[i] != 0u) {
				NRF_GPIOTE->EVENTS_IN[i] = 0u;
				//se rellena EVENT aunque no esten activas SUS interrupciones, y por tatno no queremos que se activen
				if ((NRF_GPIOTE->INTENSET & (1u << i)) != 0u){
					funcion_ISR(canalGPIOTE_pin[i]);
					break;//Prefiero hacer un boton por ISR, no varios a la vez
				}
			}
		}
}

/**
 * @brief Inicializa GPIOTE y habilita su interrupcion en el NVIC.
 */
static uint8_t  iniciado = 0;
void hal_ext_int_iniciar(void (*f_callback_isr)(uint32_t pin)) {
	if (iniciado == 0) {
		iniciado = 1;
		funcion_ISR = f_callback_isr;
		
		NRF_GPIOTE->INTENCLR = 0xFFFFFFFFu;//Dehabilitamos toda interrupcion	
		for(uint8_t i = 0; i < 8; i++) {
			NRF_GPIOTE->EVENTS_IN[i] = 0u;
			canalGPIOTE_pin[i] = 0xFFFFFFFF;
		}		
		
		NVIC_EnableIRQ(GPIOTE_IRQn);
	}
}

uint32_t obtenerCanalGPIOTE(uint32_t pin){
	uint32_t canalEncontrado = 9;
	uint32_t canalLibre = 9;
	for(uint32_t i = 0; i < 8; i++){
		if(canalGPIOTE_pin[i] == pin){
			canalEncontrado = i;
		}
		if(canalGPIOTE_pin[i] == 0xFFFFFFFF){
			canalLibre = i;
		}
	}
	return (canalEncontrado == 9) ? canalLibre : canalEncontrado;
}

/**
 * @brief Configura un pin como entrada con interrupcion por flanco. COMIENZA DESHABILITADA.
 * NRF_GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F)) 
*/
void hal_ext_int_config_pin(uint32_t pin) {
    uint32_t canalGpiote = obtenerCanalGPIOTE(pin); /* reutiliza canal si ya estaba, si no devuelve el primer libre, si esta lleno devuelve 9 */
    if (canalGpiote == 9) return;
		
    NRF_GPIOTE->INTENCLR = (1u << canalGpiote);//empiezan deshabilitadas
    canalGPIOTE_pin[canalGpiote] = pin;

		if((pin & 0x20) == 0){//puerto 0, puerto 1 usaria NRF_P1, de momento no se usa nunca asi que no lo implemento
			NRF_GPIO->PIN_CNF[pin & 0x1F] = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos)
																		|(GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
																		|(GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)
																		|(GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
																		|(GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
			
			NRF_GPIOTE->CONFIG[canalGpiote] = (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos)
																			|((pin & 0x1F) << GPIOTE_CONFIG_PSEL_Pos)
																			|(((pin & 0x20) >> 5) << GPIOTE_CONFIG_PORT_Pos)
																			|(GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos);

			NRF_GPIOTE->EVENTS_IN[canalGpiote] = 0u;//Descartar eventos pasados
		}
}

/**
 * @brief Deshabilita la interrupcion de un pin.
 */
void hal_ext_int_deshabilitar(uint32_t pin) {
    int canal = obtenerCanalGPIOTE(pin);
    if (canal == 9 || canalGPIOTE_pin[canal] != pin) return;//o lleno o libre, pero no es el propio pin
	
    NRF_GPIOTE->INTENCLR = (1u << canal);
    NRF_GPIOTE->EVENTS_IN[canal] = 0u;
}

/**
 * @brief Habilita la interrupcion de un pin.
 */
void hal_ext_int_habilitar(uint32_t pin) {
    int canal = obtenerCanalGPIOTE(pin);
    if (canal == 9 || canalGPIOTE_pin[canal] != pin) return;//o lleno o libre, pero no es el propio pin

    NRF_GPIOTE->EVENTS_IN[canal] = 0u;
    NRF_GPIOTE->INTENSET = (1u << canal);
}

void hal_ext_int_habilitar_despertar(uint32_t pin) {
    uint32_t canal = obtenerCanalGPIOTE(pin);
    if (canal == 9 || canalGPIOTE_pin[canal] != pin) return;
		
		if((pin & 0x20) == 0){//puerto 0, puerto 1 usaria NRF_P1, de momento no se usa nunca asi que no lo implemento

			NRF_GPIOTE->INTENCLR = GPIOTE_INTENCLR_PORT_Msk;//Deshabilitamos port, luego configuramos sources, luego borramos INT pasadas, reactivamos INT

			uint32_t confAct = NRF_GPIO->PIN_CNF[pin & 0x1F];
			NRF_GPIO->PIN_CNF[pin & 0x1F] = confAct | (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);

			NRF_GPIOTE->EVENTS_PORT = 0u;
			NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Msk;
		}
}

void hal_ext_int_deshabilitar_despertar(uint32_t pin) {
  uint32_t canal = obtenerCanalGPIOTE(pin);
  if (canal == 9 || canalGPIOTE_pin[canal] != pin) return;	
	if((pin & 0x20) == 0){//puerto 0, puerto 1 usaria NRF_P1, de momento no se usa nunca asi que no lo implemento
    NRF_GPIO->PIN_CNF[pin & 0x1F] &= ~(GPIO_PIN_CNF_SENSE_Msk);//La Msk ya son los propios bits de posicion del sense a 1, 
		//negarlos los pone todos a 0, y el resto de bits que estaban todos a 0, a 1, asi que no borra configuracion previa
	}
}
