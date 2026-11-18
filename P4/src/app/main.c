/* *****************************************************************************
 * P.H.2025: main.c
 *
 * Programa principal de la pr�ctica 2 de Proyecto Hardware.
 *
 * - Inicializa la HAL de GPIO y el driver de LEDs.
 * - Invoca a las funciones de parpadeo (blink_v1 o blink_v2) seg�n la sesi�n.
 *
 * Versi�n de pr�cticas:
 *   - Sesi�n 1: blink_v1 -> parpadeo con retardo por bucle de instrucciones.
 *   - Sesi�n 2: blink_v2 -> parpadeo controlado por temporizador.
 *
 * Notas:
 *   - El identificador de LED comienza en 1 (primer LED definido en board.h).
 *   - El bucle principal no retorna (sistema embebido).
 *
 * Autores: Enrique Torrs
 * Universidad de Zaragoza
 * ****************************************************************************/
#include "env.h"
#ifndef ACTIVAR_TEST

#define VERSION_BLINK_EJECUTAR 4

#include <stdint.h>
#include <stdbool.h>

#include "hal_gpio.h"
#include "drv_leds.h"
#include "drv_tiempo.h"
#include "drv_consumo.h"
#include "drv_monitor.h"
#include "rt_fifo.h"
#include "drv_botones.h"
#include "rt_GE.h"
#include "funcionesCallbackJuego.h"

/* Constante de retardo en milisegundos */
#define RETARDO_MS 500u 		 /* ejemplo: medio segundo */


#if VERSION_BLINK_EJECUTAR == 1
/* *****************************************************************************
 * BLINK v1: parpadeo de un LED conmutando on/off.
 * Retardo por bucle de instrucciones (busy-wait). Solo usa el driver de LEDs.
 * para realizar la primera sesi�n de la practica
 */
void blink_v1(LED_id_t id) {

	drv_led_establecer(id, LED_ON);
    while (1) {
        volatile uint32_t tmo = 3333333u;/* ajustad para vuestro reloj */
        while (tmo--) { /* nop */ }
        (void)drv_led_conmutar(id);
    }
}
#endif

#if VERSION_BLINK_EJECUTAR == 2
/* *****************************************************************************
 * BLINK v2, parpadeo de un led conmutando on/off 
 * activacion por tiempo, usa tanto manejador del led como el del tiempo
 * para realizar en la segunda sesion de la practica, version a entregar
 */
void blink_v2(LED_id_t id) {
		drv_tiempo_iniciar();
    Tiempo_ms_t siguiente_activacion;	

    /* Encender LED inicial */
    drv_led_establecer(id, LED_ON);

    /* Tomar tiempo de referencia */
    siguiente_activacion = drv_tiempo_actual_ms();
	
    /* Bucle principal: conmutar con temporizador */
    while (true) {
        siguiente_activacion += RETARDO_MS;     /* calcular la siguiente activaci�n */
        siguiente_activacion = drv_tiempo_esperar_hasta_ms(siguiente_activacion);
        (void)drv_led_conmutar(id);

        /* Aqu� podr�an a�adirse otras tareas peri�dicas */
    }
}
#endif

#if VERSION_BLINK_EJECUTAR == 3
void leds_c(uint32_t id){
	drv_led_conmutar(id);
}

void blink_v3(LED_id_t id){
  /* Encender LED inicial */
  drv_led_establecer(id, LED_ON);
	drv_tiempo_periodico_ms(RETARDO_MS, &leds_c, id);
	while(true){
		drv_consumo_esperar();
	}
}
#endif

#if VERSION_BLINK_EJECUTAR == 4
void blink_v4(LED_id_t id){
	drv_led_establecer(id, LED_ON);
	drv_tiempo_periodico_ms(500, rt_FIFO_encolar, ev_T_PERIODICO);
	
	uint32_t EV_ID_evento;
	uint32_t EV_auxData;
	Tiempo_us_t EV_TS;
	while(1){
		if(rt_FIFO_extraer(&EV_ID_evento, &EV_auxData, &EV_TS)){
			if(ev_T_PERIODICO) drv_led_conmutar(id);
		}else{
			drv_consumo_esperar();
		}
	}
}
#endif

#if VERSION_BLINK_EJECUTAR == 5
#include "board.h"
#include "drv_botones.h"

#define PALPITACIONES_MAX 10u
static volatile uint32_t palpitaciones = 0;

/* Callback del timer periódico (contexto IRQ): lo mínimo posible */
static void blink_bis_cb(uint32_t id){
	drv_led_conmutar((LED_id_t)id);
	palpitaciones++;
}

static EVENTO_T ev_PULSARBOTON;
void blink_v3_bis(LED_id_t id){
	while(true){                       /* en LPC, si "continuar" tras despertar, reinicia el ciclo */
		palpitaciones = 0;
		drv_led_establecer(id, LED_ON);
		drv_tiempo_periodico_ms(RETARDO_MS, blink_bis_cb, id);

		while(palpitaciones < PALPITACIONES_MAX){
			drv_consumo_esperar();
		}

		drv_tiempo_parar_periodico();

		for(LED_id_t i = 1; i <= (LED_id_t)LEDS_NUMBER; i++){
			drv_led_establecer(i, LED_OFF);
		}

		drv_botones_preparar_despertar();
		drv_consumo_dormir();
	}
}
#endif

#if VERSION_BLINK_EJECUTAR == 6
void whackAMole(void){
	EVENTO_T ev_pulsar_boton;
	rt_GE_iniciar();                          //iniciamos FIFO, alarmas, GE y consumo 
	ev_pulsar_boton = svc_GE_reservar_ID();
	drv_botones_iniciar(rt_FIFO_encolar, ev_pulsar_boton);
	rt_GE_asignar_evento_usuario(ev_pulsar_boton);  
	iniciarJuego(ev_pulsar_boton);
	rt_GE_lanzador();                         
}
#endif

/* *****************************************************************************
 * MAIN, Programa principal.
 * para la primera sesion se debe usar la funcion de blink_v1 sin temporizadores
 * para la entrega final se debe incocar blink_v2
 */
int main(void){
	uint32_t Num_Leds;

  /* Init tiempo, es un reloj que indica el tiempo desde que comenzo la ejecuci�n */
//	bool error = drv_tiempo_iniciar(); // para la sesion 2 de practica 2
	
	hal_gpio_iniciar();	// llamamos a iniciar gpio antesde que lo hagan los drivers
	/* Configurar LEDs (IDs v�lidos: 1..num_leds) */
	Num_Leds = drv_leds_iniciar();
	/*Inicializa el timer monotono, los structs de datos internos de ambos relojes, periodico y no periodico*/
	drv_tiempo_iniciar();
	/*Inicializa primero los monitores, son como LEDS*/
	drv_monitor_iniciar();
	/*Inicializa los registtos de manera adecuada para que ademas del drv_esperar funcione el drv_dormir*/
	drv_consumo_iniciar();//Para debug le a�adimos el monitor 3
	
	//rt_FIFO_inicializar((Monitor_id_t)4); De momento lo comento para no inicializar 2 veces la cola en el juego, en el juego se inicializa con rt_GE_iniciar en el juego
	if (Num_Leds > 0){
			#if VERSION_BLINK_EJECUTAR == 1
			/* Sesi�n 1: parpadeo por lazo ocupado */
			blink_v1((LED_id_t)1);
			#endif
			#if VERSION_BLINK_EJECUTAR == 2
			// Sesi�n 2: temporizador (cuando est� disponible)
			blink_v2((LED_id_t)2);
			#endif
			
			#if VERSION_BLINK_EJECUTAR == 3
			// Sesi�n 1: Practica 3
			blink_v3((LED_id_t)3);
			#endif
		
			#if VERSION_BLINK_EJECUTAR == 4
			// Sesi�n 2: Practica 3
			blink_v4((LED_id_t)4);
			#endif
			
			#if VERSION_BLINK_EJECUTAR == 5
			blink_v3_bis((LED_id_t)3);
			#endif
			
			#if VERSION_BLINK_EJECUTAR == 6
			whackAMole();
			#endif
    }
    /* En sistemas empotrados normalmente no se retorna */
    while (1) { /* idle */ }
    /* return 0; */
}
#endif
