/*
 * Algoritmos_de_planificacion.h
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#ifndef ALGORITMOS_DE_PLANIFICACION_H_
#define ALGORITMOS_DE_PLANIFICACION_H_

#include "Contexto_team.h"
#include "Herramientas_team.h"

void * planificar();
void crear_hilo_para_planificar();
bool esta_en_ready(t_entrenador *);
t_entrenador * sacar_entrenador_de_lista_pid(t_list*,int);
void obtener_proximo_ejecucion(void);

/*Entrenador mas cercano*/
void seleccionar_el_entrenador_mas_cercano_al_pokemon(t_pokemon*);
bool esta_mas_cerca(t_entrenador*, t_entrenador*, t_pokemon*);
int distancia_a_pokemon(t_entrenador*, t_pokemon*);

/*SJF*/
void ordenar_lista_estimacion(t_list *);
void desalojar_ejecucion(void);
int estimar_entrenador(t_entrenador *);



#endif /* ALGORITMOS_DE_PLANIFICACION_H_ */
