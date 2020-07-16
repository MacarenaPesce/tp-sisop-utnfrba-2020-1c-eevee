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
#include "Deadlock.h"

bool hay_pokemones_en_el_mapa();
bool chequear_que_no_sea_un_objetivo_de_la_gente_esperando_por_caught(t_pokemon *);
bool chequear_que_no_sea_un_objetivo_de_la_gente_en_ready(t_pokemon *);
bool chequear_que_no_sea_un_objetivo_del_entrenador_en_ejecucion(t_pokemon *);
bool el_pokemon_no_es_objetivo_de_alguien(t_pokemon *);
void chequeo_si_puedo_atrapar_otro(t_entrenador*);
void operar_con_appeared_pokemon(t_pokemon *);
void operar_con_caught_pokemon(uint32_t, uint32_t);
void agregar_pokemon_a_mapa(t_pokemon *);
void definir_objetivo_global();
void localizar_entrenadores_en_mapa();
void agregar_entrenador(uint32_t, uint32_t, uint32_t, t_list*, t_list*);
void ver_razon_de_bloqueo(t_entrenador *);
void actualizar_mapa_y_entrenador(t_catch_pokemon*, t_entrenador*);
void chequear_si_fue_cumplido_el_objetivo_global();
void hacer_procedimiento_para_atrapar_default(t_catch_pokemon*, t_entrenador *);
void hacer_procedimiento_para_atrapar_pokemon_con_broker(t_entrenador *);
void bloquear_entrenador(t_entrenador*);
void consumir_un_ciclo_de_cpu(t_entrenador*);
void consumir_un_ciclo_de_cpu_mientras_planificamos(t_entrenador*);
void crear_hilo_para_tratamiento_de_mensajes();
bool chequear_si_recibi_appeared_de_especie_antes(char *);
void * tratamiento_de_mensajes();
void mostrar_lo_que_hay_en_la_lista_de_objetivos_del_entrenador(t_list *);
bool fijarme_si_debo_atraparlo_usando_el_objetivo_global(char *);
void confirmar_desalojo_en_ejecucion(void);

#endif /* TEAM_H_ */
