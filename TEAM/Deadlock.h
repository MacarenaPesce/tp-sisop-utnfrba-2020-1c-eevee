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
#include "Entrenador.h"

void crear_hilo_para_deadlock();
void chequear_cantidad_de_deadlocks_producidos();
void * chequear_deadlock();
void ver_entre_quienes_hay_deadlock_y_resolverlo(t_entrenador *);
void planificar_para_deadlock(t_entrenador*, t_entrenador*, t_objetivo_entrenador*);
void verificar_si_entrenador_sigue_bloqueado(t_entrenador*);
void verificar_si_sigue_habiendo_deadlock_luego_del_intercambio();




#endif /* DEADLOCK_H_ */
