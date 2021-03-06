/*
 * Entrenador.h
 *
 *  Created on: 23 jun. 2020
 *      Author: utnso
 */

#ifndef ENTRENADOR_H_
#define ENTRENADOR_H_

#include "Contexto_team.h"
#include "Herramientas_team.h"
#include "Team.h"

void * jugar_con_el_entrenador(t_entrenador *);
void llegar_a_el_pokemon(t_entrenador *);
void atrapar(t_entrenador *);

/*Deadlock*/
void mover_entrenador_a_otra_posicion(t_entrenador*);
bool tiene_mas_de_un_innecesario(t_entrenador *);
t_objetivo_entrenador* elegir_pokemon_innecesario(t_entrenador*);
t_objetivo_entrenador * elegir_pokemon_innecesario_util(t_entrenador *, t_entrenador *);
void realizar_intercambio(t_entrenador*);
void realizar_las_operaciones_de_deadlock(t_entrenador *);


#endif /* ENTRENADOR_H_ */
