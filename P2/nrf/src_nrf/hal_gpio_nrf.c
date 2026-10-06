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
	//(((port) << 5) | ((pin) & 0x1F)) = gpio
	uint32_t mascaraPin = 1 << (gpio & 0x1F); //Obtenemos el pin(direccion & 0x1F), y calculamos la posicion del uno en la mascara
	//Yo pensaria que habria que comprobar en que puerto hay que cambiar el sentido
	(direccion == HAL_GPIO_PIN_DIR_INPUT) ? (NRF_GPIO->DIRCLR = mascaraPin) : (NRF_GPIO->DIRSET = mascaraPin);

}

/**
 * @brief Lee el valor lógico de un GPIO.
 *
 * @param gpio Pin a leer.
 * @return 0 si el GPIO está a nivel bajo, distinto de 0 si está a nivel alto.
 */
uint32_t hal_gpio_leer(HAL_GPIO_PIN_T gpio){
	uint32_t mascaraPin = 1 << (gpio & 0x1F);
	//Comprobamos en que puerto hay que cambiar el sentido
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

