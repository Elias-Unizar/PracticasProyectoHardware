#include "env.h"

/* Todo el fichero depende de ACTIVAR_TEST (en env.h). Si no esta definido
 * este fichero queda vacio y no choca con el main real del proyecto. */
#ifdef ACTIVAR_TEST

#include <stdint.h>

#ifdef TEST_LEDS
#include "test_led.h"
#endif
#ifdef TEST_TIEMPO
#include "test_tiempo.h"
#endif
#ifdef TEST_CONSUMO
#include "test_consumo.h"
#endif
#ifdef TEST_RT_FIFO
#include "test_fifo.h"
#endif
#ifdef TEST_SVC_DIAGNOSTICO
#include "test_svc_diagnostico.h"
#endif
#ifdef TEST_SVC_GE
#include "test_svc_GE.h"
#endif
#ifdef TEST_SVC_ALARMA
#include "test_svc_alarma.h"
#endif
#ifdef TEST_BOTONES
#include "test_botones.h"
#endif
/* Un bit por modulo; 0 = todo bien. Se puede mirar en el debugger. */

volatile uint32_t tests_fallos = 0;

int main(void){
#ifdef TEST_LEDS
	if (!test_leds_todos())    tests_fallos = 1;
	while(tests_fallos != 0){;}
#endif
		
#ifdef TEST_TIEMPO
	if (!test_tiempo_todos())  tests_fallos = 1;
	while(tests_fallos != 0){;}
#endif

#ifdef TEST_CONSUMO
	if (!test_consumo_todos()) tests_fallos = 1;
	while(tests_fallos != 0){;}
#endif

	/* rt_fifo el ultimo: su test de desbordamiento puede no retornar */
#ifdef TEST_RT_FIFO
	if (!test_fifo_todos_no_bloq()) tests_fallos = 1;
	while(tests_fallos != 0){;}
	//test_fifo_desbordamiento();
#endif
	
#ifdef TEST_SVC_DIAGNOSTICO
	if (!test_svc_diagnostico_todos_no_bloq()) tests_fallos = 1;
	while(tests_fallos != 0){;}
	//test_svc_DIAGNOSTICO_error();
#endif

#ifdef TEST_SVC_GE
	if (!test_svc_GE_todos_no_bloq()) tests_fallos = 1;
	while(tests_fallos != 0){;}
	//test_svc_GE_reservar_ID_desborde();
	//test_svc_GE_suscribir_desborde();
#endif

#ifdef TEST_SVC_ALARMA
	if (!test_svc_alarma_todos()) tests_fallos = 1;
	while(tests_fallos != 0){;}
#endif

#ifdef TEST_BOTONES
	if (!test_drv_botones_todos()) tests_fallos = 1;
	while(tests_fallos != 0){;}
#endif

	/* En sistemas empotrados no se retorna */
	while (1) { /* idle */ }
}

#endif /* ACTIVAR_TEST */
