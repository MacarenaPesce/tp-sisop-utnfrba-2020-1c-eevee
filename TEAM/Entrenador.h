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
void mover_entrenador_a_otra_posicion(t_entrenador*);
void realizar_intercambio(t_entrenador*);
t_objetivo_entrenador* elegir_pokemon_innecesario(t_entrenador*);

#endif /* ENTRENADOR_H_ */
