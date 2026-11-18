#ifndef FUNCIONES_CALLBACK_H
#define FUNCIONES_CALLBACK_H

#include "rt_GE.h"

/* Inicia el juego. Recibe el evento de pulsacion de boton que ya usa
 * drv_botones_iniciar (el juego se suscribe a el). Debe llamarse despues de
 * rt_GE_iniciar() y drv_botones_iniciar(), y antes de rt_GE_lanzador(). */
void iniciarJuego(EVENTO_T ev_pulsar_boton);

#endif