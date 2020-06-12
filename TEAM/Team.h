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


void operar_con_appeared_pokemon(t_appeared_pokemon *);
void operar_con_localized_pokemon(t_localized_pokemon *);
void operar_con_caught_pokemon(t_caught_pokemon *);
void agregar_pokemon_a_mapa(t_pokemon *);
void mostrar_lo_que_hay_en_la_lista_de_objetivos();
void agregar_entrenador(uint32_t, uint32_t, uint32_t, t_list*, t_list*);
t_list* obtener_pokemones(t_list*,t_list*, uint32_t);
void definir_objetivo_global();
void cargar_objetivos(t_list*, t_list*);
void agregar_objetivo(char*, uint32_t, t_list*);
void mostrar_lo_que_hay_en_lista_entrenadores();
void localizar_entrenadores_en_mapa();
void crear_hilo_entrenador(t_entrenador *, void*funcion_a_ejecutar(t_entrenador*));
void * jugar_con_el_entrenador(t_entrenador *);
void seleccionar_el_entrenador_mas_cercano_al_pokemon();
void meter_entrenadores_en_ready();
void chequear_si_hay_pokemones_nuevos();
void planificar();
bool esta_mas_cerca(t_entrenador*, t_entrenador*, t_pokemon*);
int distancia_a_pokemon(t_entrenador*, t_pokemon*);
t_objetivo * buscar_pokemon_por_especie(t_list*, char*);
void mostrar_pokemones_de_entrenador(t_list*, t_list* , int);
bool objetivo_personal_cumplido(t_entrenador*);
bool objetivo_global_cumplido();

#endif /* TEAM_H_ */
