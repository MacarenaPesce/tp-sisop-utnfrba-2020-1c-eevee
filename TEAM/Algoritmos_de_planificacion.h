/*
 * Algoritmos_de_planificacion.h
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#ifndef ALGORITMOS_DE_PLANIFICACION_H_
#define ALGORITMOS_DE_PLANIFICACION_H_

#include "Contexto_team.h"

t_entrenador * sacar_entrenador_de_lista_pid(t_list*,int);
void ordenar_lista_estimacion(t_list *);
void desalojar_ejecucion(void);
int estimar_entrenador(t_entrenador *);
void obtener_proximo_ejecucion(void);


#endif /* ALGORITMOS_DE_PLANIFICACION_H_ */
