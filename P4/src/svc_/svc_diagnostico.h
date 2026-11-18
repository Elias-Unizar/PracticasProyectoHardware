#ifndef SVC_DGNSTC_H
#define SVC_DGNSTC_H
#include <stdint.h>
#include <stddef.h>

typedef uint32_t CANAL_T;
typedef int32_t err_t;

#define ERROR_SINERROR 0//Para tener el err_t de todas formas haria falta incluir este err_t global

#define ERROR_DIAGN_0_N_MONITORES (-1)
#define ERROR_DIAGN_CANAL_INVALIDO (-2)

err_t svc_DIAGNOSTICO_iniciar(void);
 
CANAL_T svc_DIAGNOSTICO_reservar(void);
 
err_t svc_DIAGNOSTICO_senyalar(CANAL_T canal);
 
err_t svc_DIAGNOSTICO_limpiar(CANAL_T canal);
	
void svc_DIAGNOSTICO_error(void);

#endif
