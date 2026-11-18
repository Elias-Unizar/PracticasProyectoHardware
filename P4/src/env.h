#ifndef ENV_H
#define ENV_H

/*
* PARA EL LPC: VICVECTCADDR cogidos: 
* 0 = Watchdog
* 1 = Timer 1; monotonico
* 3 = Timer 0; periodico
* 4,5,6 = EINT0, EINT1, EINT2
*/

#define ACTIVAR_TEST //Descomentar esta linea si se desea ejecutar tests
#define TEST_LEDS     //Tests basicos de leds y monitores
#define TEST_TIEMPO   //Tests muy basicos de temporizadores
#define TEST_CONSUMO  //Tests manuales, en los que comprobar
#define TEST_RT_FIFO 
#define TEST_SVC_DIAGNOSTICO
#define TEST_SVC_GE
#define TEST_SVC_ALARMA
#define TEST_BOTONES
#endif
