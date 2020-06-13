/*
 * Team.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_H_
#define TEAM_H_

#include "Contexto_team.h"
#include "Herramientas_team.h"
#include "Funciones_para_listas.h"
#include "Algoritmos_de_planificacion.h"
#include "Funciones_de_conexion.h"
#include "Interaccion_con_gameboy.h"
#include "Interaccion_con_broker.h"

void operar_con_appeared_pokemon(t_pokemon *);
void operar_con_localized_pokemon(t_localized_pokemon *);
void operar_con_caught_pokemon(t_caught_pokemon *, uint32_t);
void agregar_pokemon_a_mapa(t_pokemon *);
void definir_objetivo_global();
void * jugar_con_el_entrenador(t_entrenador * entrenador);
void actualizar_mapa_y_entrenador(t_catch_pokemon* catch_pokemon, t_entrenador* entrenador);
void atrapar(t_entrenador * entrenador);
void bloquear_entrenador(t_entrenador*);
void consumir_un_ciclo_de_cpu();
void llegar_a_el_pokemon(t_entrenador *);
void * planificar();
void localizar_entrenadores_en_mapa();
void agregar_entrenador(uint32_t, uint32_t, uint32_t, t_list*, t_list*);

#endif /* TEAM_H_ */
