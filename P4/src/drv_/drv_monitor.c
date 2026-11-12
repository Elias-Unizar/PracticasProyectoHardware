#include "drv_monitor.h"
#include "board.h"
#include "hal_gpio.h"

#if MONITOR_NUMBER > 0
	static const HAL_GPIO_PIN_T s_monitor_list[MONITOR_NUMBER] = MONITOR_LIST;
#endif

	

uint32_t drv_monitor_iniciar(){
	#if MONITOR_NUMBER > 0
   for (MONITOR_id_t i = 1; i <= (MONITOR_id_t)MONITOR_NUMBER; ++i) {
		 hal_gpio_sentido(s_monitor_list[i-1], HAL_GPIO_PIN_DIR_OUTPUT);
     /* Apagar por defecto respetando activo-alto/bajo */
		 uint32_t valorHW = (LEDS_ACTIVE_STATE ? 0 : 1); //Para apagar el monitor, si se enciende con 1, ponemos 0; si se enciende con 0, ponemos 1
     hal_gpio_escribir(s_monitor_list[i-1], valorHW);
   }
  #endif //MONITOR_NUMBER > 0	
	return (uint32_t)MONITOR_NUMBER;
}

void drv_monitor_marcar(MONITOR_id_t id){
  /* Apagar por defecto respetando activo-alto/bajo */
  hal_gpio_escribir(s_monitor_list[id-1], LEDS_ACTIVE_STATE);
	return;
}

void drv_monitor_desmarcar(MONITOR_id_t id){
  /* Apagar por defecto respetando activo-alto/bajo */
	uint32_t valorHW = (LEDS_ACTIVE_STATE ? 0 : 1); //Para apagar el monitor, si se enciende con 1, ponemos 0; si se enciende con 0, ponemos 1
  hal_gpio_escribir(s_monitor_list[id-1], valorHW);
	return;
}
