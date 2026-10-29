/* *****************************************************************************
 * P.H.2025: TODO
 */
#include "nrf.h"
#include "hal_gpio.h"

/**
 * @brief Inicializa el subsistema GPIO.
 *
 * Debe invocarse antes de usar el resto de funciones.
 * Reconfigura todos los pines como entradas para evitar cortocircuitos.
 */
void hal_gpio_iniciar(void){
	NRF_GPIO->DIR = 0x0;
}

/**
 * @brief Configura la dirección (entrada/salida) de un GPIO.
 *
 * @param gpio      Pin a configurar.
 * @param direccion HAL_GPIO_PIN_DIR_INPUT o HAL_GPIO_PIN_DIR_OUTPUT.
 */
void hal_gpio_sentido(HAL_GPIO_PIN_T gpio, hal_gpio_pin_dir_t direccion){
	//uint32_t mascaraPin = 1 << (gpio & 0x1F); //Obtenemos el pin(direccion & 0x1F), y calculamos la posicion del uno en la mascara
	//(((port) << 5) | ((pin) & 0x1F)) = gpio
	if (direccion == HAL_GPIO_PIN_DIR_INPUT){
		NRF_GPIO->PIN_CNF[(gpio & 0x1F)] = ((GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
																			(GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
																			(GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos) |
																			(GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
																			(GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)
																			);
	}else{
		NRF_GPIO->PIN_CNF[(gpio & 0x1F)] = ((GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) | //SE PUEDE AHORRAR ENERGIA QUITANDO ESTO
																			(GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
																			(GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) |
																			(GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos) |
																			(GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
																			);
	}
	


}

/**
 * @brief Lee el valor lógico de un GPIO.
 *
 * @param gpio Pin a leer.
 * @return 0 si el GPIO está a nivel bajo, distinto de 0 si está a nivel alto.
 */
uint32_t hal_gpio_leer(HAL_GPIO_PIN_T gpio){
	uint32_t mascaraPin = 1 << (gpio & 0x1F);
	//Comprobamos en que puerto hay q leer
	return ((NRF_GPIO->IN & mascaraPin) != 0);
}

/**
 * @brief Escribe un valor lógico en un GPIO.
 *
 * @param gpio  Pin a escribir.
 * @param valor Valor a escribir: 0 ? nivel bajo, distinto de 0 ? nivel alto.
 */
void hal_gpio_escribir(HAL_GPIO_PIN_T gpio, uint32_t valor){
	uint32_t mascaraPin = 1 << (gpio & 0x1F);
	//Comprobamos en que puerto hay que cambiar el sentido
	(valor == 0) ? (NRF_GPIO->OUTCLR = mascaraPin) : (NRF_GPIO->OUTSET = mascaraPin);
}

