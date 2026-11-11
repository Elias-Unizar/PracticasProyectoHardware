#ifndef SVC_DGNSTC_H
#define SVC_DGNSTC_H
#include <stdint.h>
#include <stddef.h>

typedef uint32_t CANAL_T;
typedef int32_t err_t;
 
err_t svc_DIAGNOSTICO_iniciar(void);
 
CANAL_T svc_DIAGNOSTICO_reservar(void);
 
err_t svc_DIAGNOSTICO_senyalar(CANAL_T canal);
 
err_t svc_DIAGNOSTICO_limpiar(CANAL_T canal);
	
void svc_DIAGNOSTICO_error();

#endif
