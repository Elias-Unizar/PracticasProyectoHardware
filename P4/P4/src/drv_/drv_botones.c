#include "drv_botones.h"
#include "hal_ext_int.h"
#include "hal_gpio.h"
#include "board.h"
#include "svc_alarma.h"
#include "rt_FIFO.h"

#define T_REBOTE_PRESION_MS 15
#define T_ENCUESTA_PERIODICA_MS 20
#define T_REBOTE_DISPERSION_MS 15

typedef enum{
	E_BOTON_REPOSO = 0,
	E_BOTON_REBOTES = 1,
	E_BOTON_MUESTREO = 2,
	E_BOTON_SALIDA = 3,
}ESTADOS_BOTON_T;

static id_boton_t listaBotones[BUTTONS_NUMBER] = BUTTONS_LIST;
static ESTADOS_BOTON_T estadosBotones[BUTTONS_NUMBER] = {E_BOTON_REPOSO};

static EVENTO_T ev_PULSA_BOTON;
static EVENTO_T ev_BOTON_RETARDO;

id_boton_t buscarBotonDePin(uint32_t pin){
	id_boton_t botonActivado = 0;
	while(listaBotones[botonActivado] != pin){
		botonActivado++;//Buscamos que boton fue el que estaba asociado al pin activado
	}
	return botonActivado;
}

static void(*drv_cb_a_llamar)();
static void drv_cb(uint32_t pin){
	hal_ext_int_deshabilitar(pin);
	//Se que una busqueda no es lo mejor posible en una ISR, pero a cambio de esto nos ahorramos esta busqueda por cada muestreo de boton en la MSF
	drv_cb_a_llamar(ev_PULSA_BOTON, buscarBotonDePin(pin));
}

uint32_t drv_botones_iniciar(void (*cb_a_llamar)(), EVENTO_T ev_1){
	hal_gpio_iniciar();
	hal_ext_int_iniciar(drv_cb);
	drv_cb_a_llamar = cb_a_llamar;

	ev_PULSA_BOTON = ev_1;
	svc_GE_suscribir(ev_PULSA_BOTON, 0, drv_botones_actualizar);
	ev_BOTON_RETARDO = svc_GE_reservar_ID();
	svc_GE_suscribir(ev_BOTON_RETARDO, 5, drv_botones_actualizar);
	
	return BUTTONS_NUMBER;
}

/*True si el boton está siendo pulsado, False si no lo está siendo*/
boton_status_t drv_botones_pulsado(id_boton_t boton){
	if(estadosBotones[boton] == E_BOTON_MUESTREO || estadosBotones[boton] == E_BOTON_REBOTES){
		return BOTON_PULSADO;
	}
	return BOTON_NO_PULSADO;
}

/*Funcion interna para usar con el rt_GE*/
void drv_botones_actualizar(EVENTO_T evento, id_boton_t botonAct){
	switch(estadosBotones[botonAct]){
		case E_BOTON_REPOSO://Nos habra despertado la ISR
			if(evento == ev_PULSA_BOTON){
				svc_alarma_activar(svc_alarma_codificar(false, (Tiempo_ms_t)T_REBOTE_PRESION_MS, 0x0), ev_BOTON_RETARDO, botonAct);
				estadosBotones[botonAct] = E_BOTON_REBOTES;
			}
			break;
		case E_BOTON_REBOTES:
			if(evento == ev_BOTON_RETARDO){
				svc_alarma_activar(svc_alarma_codificar(true, (Tiempo_ms_t)T_ENCUESTA_PERIODICA_MS, 0x0), ev_BOTON_RETARDO, botonAct);
				estadosBotones[botonAct] = E_BOTON_MUESTREO;
			}
			break;
		case E_BOTON_MUESTREO:
			if(evento == ev_BOTON_RETARDO){
				if(hal_gpio_leer(listaBotones[botonAct]) != BUTTONS_ACTIVE_STATE){
					svc_alarma_activar(svc_alarma_codificar(false, (Tiempo_ms_t)T_REBOTE_DISPERSION_MS, 0x0), ev_BOTON_RETARDO, botonAct);
					estadosBotones[botonAct] = E_BOTON_SALIDA;
				}
			}
			break;
		case E_BOTON_SALIDA:
			if(evento == ev_BOTON_RETARDO){
				hal_ext_int_habilitar(listaBotones[botonAct]);//habilitar, limpia activaciones anteriores
				estadosBotones[botonAct] = E_BOTON_REPOSO;
			}
			break;
	}
}
