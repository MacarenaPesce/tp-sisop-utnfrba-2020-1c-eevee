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

void iniciar_servidor(void);
void esperar_gameboy(int);
void enviar_get();
void conectarse_a_colas_de_broker();
void recibir_appeared_pokemon_desde_gameboy(t_appeared_pokemon *);
void * jugar_con_el_entrenador(t_entrenador *);

void localizar_entrenadores_en_el_mapa();
/*Carga una lista de entrenadores con la info del archivo de configuracion, ademas de agregarles un id
  y el estado NUEVO. Me falta resolver agregarles las listas de objetivos particulares de cada uno. Y quizas
  es conveniente tener dos estructuras una para la info del hilo y el estado, y otra para la info del entrenador en si.
  Vamos a tener otra info, como la de las rafagas ejecutadas y alguna otra cosa que surja. Me hace ruido que este
  todo en la misma estructura*/

void definir_objetivo_global();
/*Cuenta todos los pokemones de cada especie y carga en una lista especie-cantidad*/

#endif /* TEAM_H_ */
