#include "env.h"
#include <stdint.h>
#include <stdbool.h>

#include "board.h"
#include "drv_leds.h"
#include "rt_fifo.h"
#include "rt_GE.h"
#include "svc_alarma.h"
#include "funcionesCallbackJuego.h"

#define T_ENCENDIDO_INICIAL_MS    1500u
#define T_ENCENDIDO_MINIMO_MS      300u
#define T_REDUCCION_POR_PUNTO_MS    50u
#define T_PAUSA_ENTRE_LEDS_MS      500u
#define FALLOS_MAXIMOS               3u

typedef enum{ /* Estados del juego */
    estado_inicial,
    led_encendido,
    led_apagado,
    estado_fin
} estado_t;

static EVENTO_T ev_JUEGO_TIMEOUT, ev_PULSA;

static estado_t estado = estado_inicial;
/* LEDs de juego: minimo entre LEDs y botones (deberia ser 4 en la placa) */
static uint32_t ledsJuego = (LEDS_NUMBER < BUTTONS_NUMBER) ? LEDS_NUMBER : BUTTONS_NUMBER;
static uint32_t ledActual = 1;   /* LED actual, 1..ledsJuego */
static uint32_t puntos = 0;
static uint32_t fallos = 0;

static void establecerLedsJuego(LED_status_t st){
    for(LED_id_t i = 1; i <= (LED_id_t)ledsJuego; i++){
        (void)drv_led_establecer(i, st);
    }
}

/* Tiempo que permanece encendido el LED: baja con los puntos hasta un minimo */
static uint32_t tiempoEncendido_ms(void){
    uint32_t reduccion = puntos * T_REDUCCION_POR_PUNTO_MS;
    if(reduccion >= (T_ENCENDIDO_INICIAL_MS - T_ENCENDIDO_MINIMO_MS)){
        return T_ENCENDIDO_MINIMO_MS;
    }
    return T_ENCENDIDO_INICIAL_MS - reduccion;
}

/* Programa (o reprograma, mismo evento y aux) la alarma del juego. 0 = cancela */
static void programarTimeout(uint32_t ms){
    (void)svc_alarma_activar(svc_alarma_codificar(false, ms, 0), ev_JUEGO_TIMEOUT, 0);
}

static void reiniciarPartida(void){
    establecerLedsJuego(LED_OFF);
    puntos = 0;
    fallos = 0;
    ledActual = 1;
    programarTimeout(T_PAUSA_ENTRE_LEDS_MS);
    estado = led_apagado;
}

static void terminarRonda(bool acierto){
    (void)drv_led_establecer((LED_id_t)ledActual, LED_OFF);
    if(acierto){
        puntos++;
    }else{
        if(puntos > 0){ puntos--; }
        fallos++;
    }
    ledActual = (ledActual % ledsJuego) + 1;

    if(fallos >= FALLOS_MAXIMOS){
        programarTimeout(0);
        establecerLedsJuego(LED_ON);
        estado = estado_fin;
    }else{
        programarTimeout(T_PAUSA_ENTRE_LEDS_MS);
        estado = led_apagado;
    }
}

static void actualizarJuego(EVENTO_T evento, uint32_t aux){
    switch(estado){
        case estado_inicial:
            break;  /* iniciarJuego lo deja en led_apagado */

        case led_apagado:
            if(evento == ev_JUEGO_TIMEOUT){
                (void)drv_led_establecer((LED_id_t)ledActual, LED_ON);
                programarTimeout(tiempoEncendido_ms());
                estado = led_encendido;
            }
            break;

        case led_encendido:
            if(evento == ev_JUEGO_TIMEOUT){
                terminarRonda(false);
            }else if(evento == ev_PULSA){
                terminarRonda((aux + 1u) == ledActual);
            }
            break;

        case estado_fin:
            if(evento == ev_PULSA){
                reiniciarPartida();
            }
            break;
    }
}

void iniciarJuego(EVENTO_T ev_pulsar_boton){
    ev_PULSA = ev_pulsar_boton;
    ev_JUEGO_TIMEOUT = svc_GE_reservar_ID();
    (void)svc_GE_suscribir(ev_JUEGO_TIMEOUT, 1, actualizarJuego);
    (void)svc_GE_suscribir(ev_PULSA, 1, actualizarJuego);
    reiniciarPartida();
}