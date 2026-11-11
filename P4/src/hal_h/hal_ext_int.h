#ifndef HAL_EXT_INT_H
#define HAL_EXT_INT_H

#include <stdint.h>

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
void hal_ext_int_config_pin(uint32_t pin);

/**
 * @brief Deshabilita la interrupci?n para un pin.
 */
void hal_ext_int_deshabilitar(uint32_t pin);

/**
 * @brief Habilita la interrupci?n para un pin.
 */
void hal_ext_int_habilitar(uint32_t pin);

#endif // HAL_EXT_INT_H