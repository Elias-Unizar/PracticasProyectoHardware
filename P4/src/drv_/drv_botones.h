#ifndef DRV_BOTONES_H
#define DRV_BOTONES_H
#include <stdint.h>
#include <stdbool.h>
#include "svc_GE.h"

typedef uint32_t id_boton_t;
typedef enum {
    BOTON_NO_PULSADO  = 0,
    BOTON_PULSADO = 1,
} boton_status_t;

/*Devuelve el numero de botones iniciados*/
uint32_t drv_botones_iniciar(void (*cb_a_llamar)(), EVENTO_T ev_PULSA_BOTON);

/*Devuelve el estado del boton*/
boton_status_t drv_botones_pulsado(id_boton_t boton);

/*Funcion interna para usar con el rt_GE*/
void drv_botones_actualizar(EVENTO_T evento, id_boton_t botonPulsado);

void drv_botones_preparar_despertar(void);

#endif
