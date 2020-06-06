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

void operar_con_appeared_pokemon(t_appeared_pokemon *);
void operar_con_localized_pokemon(t_localized_pokemon *);
void operar_con_caught_pokemon(t_caught_pokemon *);
void agregar_pokemon_a_mapa(t_pokemon * pokemon);
void recibir_appeared_pokemon_desde_gameboy(t_appeared_pokemon*);
void agregar_entrenador(uint32_t, uint32_t, uint32_t, t_list*, t_list*);
void localizar_entrenadores_en_el_mapa();
t_list* obtener_objetivos_de_entrenador(t_list*, uint32_t);
t_list* obtener_pokemones_de_entrenador(t_list*, uint32_t);
void agregar_objetivo(char*, uint32_t);
void definir_objetivo_global();
void crear_hilo_entrenador(t_entrenador*, void*funcion_a_ejecutar(t_entrenador*));
void * jugar_con_el_entrenador(t_entrenador*);
void seleccionar_el_entrenador_mas_cercano_al_pokemon();
void meter_entrenadores_en_ready();
void chequear_si_hay_pokemones_nuevos();
void planificar();
void enviar_get();
t_objetivo * sacar_objetivo_de_la_lista(t_list*, char*);
void enviar_mensaje_por_cada_pokemon_requerido();
void convertirse_en_suscriptor_global_del_broker();
int recibir_mensaje_broker();
int recibir_mensaje_gameboy(t_conexion_gameboy);

#endif /* TEAM_H_ */
