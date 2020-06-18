/*
 * Deadlock.h
 *
 *  Created on: 16 jun. 2020
 *      Author: utnso
 */

#ifndef DEADLOCK_H_
#define DEADLOCK_H_

#include "Contexto_team.h"
#include "Herramientas_team.h"
#include "Algoritmos_de_planificacion.h"
#include "Team.h"

void crear_hilo_para_deadlock();
void chequear_deadlock();
void interbloqueo();
void * planificar_para_deadlock(t_entrenador*, t_entrenador*, t_objetivo_entrenador*);
void mover_entrenador_a_otra_posicion(t_entrenador*);
void realizar_intercambio(t_entrenador*);
t_objetivo_entrenador* elegir_pokemon_innecesario(t_entrenador*);



#endif /* DEADLOCK_H_ */
