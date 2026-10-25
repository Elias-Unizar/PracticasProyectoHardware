/* *****************************************************************************
 * P.H.2025: main.c
 *
 * Programa principal de la práctica 2 de Proyecto Hardware.
 *
 * - Inicializa la HAL de GPIO y el driver de LEDs.
 * - Invoca a las funciones de parpadeo (blink_v1 o blink_v2) según la sesión.
 *
 * Versión de prácticas:
 *   - Sesión 1: blink_v1 -> parpadeo con retardo por bucle de instrucciones.
 *   - Sesión 2: blink_v2 -> parpadeo controlado por temporizador.
 *
 * Notas:
 *   - El identificador de LED comienza en 1 (primer LED definido en board.h).
 *   - El bucle principal no retorna (sistema embebido).
 *
 * Autores: Enrique Torrs
 * Universidad de Zaragoza
 * ****************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "hal_gpio.h"
#include "drv_leds.h"
#include "drv_tiempo.h"
#include "drv_consumo.h"

/* Constante de retardo en milisegundos */
#define RETARDO_MS 500u 		 /* ejemplo: medio segundo */

/* Prototipos */
static void blink_v1(LED_id_t id);
static void blink_v2(LED_id_t id);

/* *****************************************************************************
 * BLINK v1: parpadeo de un LED conmutando on/off.
 * Retardo por bucle de instrucciones (busy-wait). Solo usa el driver de LEDs.
 * para realizar la primera sesión de la practica
 */
static void blink_v1(LED_id_t id) {

	drv_led_establecer(id, LED_ON);
    while (1) {
        volatile uint32_t tmo = 3333333u;/* ajustad para vuestro reloj */
        while (tmo--) { /* nop */ }
        (void)drv_led_conmutar(id);
    }
}

/* *****************************************************************************
 * BLINK v2, parpadeo de un led conmutando on/off 
 * activacion por tiempo, usa tanto manejador del led como el del tiempo
 * para realizar en la segunda sesion de la practica, version a entregar
 */
static void blink_v2(LED_id_t id) {
		drv_tiempo_iniciar();
    Tiempo_ms_t siguiente_activacion;	

    /* Encender LED inicial */
    drv_led_establecer(id, LED_ON);

    /* Tomar tiempo de referencia */
    siguiente_activacion = drv_tiempo_actual_ms();
	
    /* Bucle principal: conmutar con temporizador */
    while (true) {
        siguiente_activacion += RETARDO_MS;     /* calcular la siguiente activación */
        siguiente_activacion = drv_tiempo_esperar_hasta_ms(siguiente_activacion);
        (void)drv_led_conmutar(id);

        /* Aquí podrían añadirse otras tareas periódicas */
    }
}


void myblink_v3_aux(){
	volatile static int iteracion = 0;
	drv_led_conmutar(3);
	iteracion += 1;
}

static void myblink_v3() {
		drv_tiempo_iniciar();

    /* Encender LED inicial */
    drv_led_establecer(3, LED_ON);
	
		drv_tiempo_periodico_ms(RETARDO_MS, &myblink_v3_aux, 0);
    /* Bucle principal: conmutar con temporizador */
    while (true) {}
}

void leds_c(uint32_t id, uint32_t ms){
	drv_led_conmutar(3);
}

static void blink_v3(LED_id_t id){
  /* Encender LED inicial */
  drv_led_establecer(id, LED_ON);
	drv_tiempo_periodico_ms(RETARDO_MS, &leds_c, id);
	while(true){
		drv_consumo_esperar();
	}
}


/* *****************************************************************************
 * MAIN, Programa principal.
 * para la primera sesion se debe usar la funcion de blink_v1 sin temporizadores
 * para la entrega final se debe incocar blink_v2
 */
int main(void){
	uint32_t Num_Leds;

  /* Init tiempo, es un reloj que indica el tiempo desde que comenzo la ejecución */
//	bool error = drv_tiempo_iniciar(); // para la sesion 2 de practica 2
	
	hal_gpio_iniciar();	// llamamos a iniciar gpio antesde que lo hagan los drivers
	drv_tiempo_iniciar();
	drv_consumo_iniciar(1);
	/* Configurar LEDs (IDs válidos: 1..num_leds) */
	Num_Leds = drv_leds_iniciar();
	
	if (Num_Leds > 0){
			/* Sesión 1: parpadeo por lazo ocupado */
			//blink_v1((LED_id_t)1);

			// Sesión 2: temporizador (cuando esté disponible)
			//blink_v2((LED_id_t)2);
		
			//myblink_v3();
			
			blink_v3((LED_id_t)3);
			 
    }

    /* En sistemas empotrados normalmente no se retorna */
    while (1) { /* idle */ }
    /* return 0; */
}
