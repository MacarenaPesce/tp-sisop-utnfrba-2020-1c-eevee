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


void operar_con_appeared_pokemon(t_pokemon *);
void operar_con_localized_pokemon(t_localized_pokemon *);
void operar_con_caught_pokemon(t_caught_pokemon *, uint32_t);
void agregar_pokemon_a_mapa(t_pokemon *);
void definir_objetivo_global();
void localizar_entrenadores_en_mapa();
void agregar_entrenador(uint32_t, uint32_t, uint32_t, t_list*, t_list*);
void * jugar_con_el_entrenador(t_entrenador *);
void actualizar_mapa_y_entrenador(t_catch_pokemon*, t_entrenador*);
void chequear_si_fue_cumplido_el_objetivo_global();
void hacer_procedimiento_para_atrapar_default(t_catch_pokemon*, t_entrenador *);
void hacer_procedimiento_para_atrapar_pokemon_con_broker(t_packed * ack, t_entrenador *);
void atrapar(t_entrenador *);
void mostrar_lo_que_hay_en_la_lista_de_objetivos_del_entrenador(t_list *);
void bloquear_entrenador(t_entrenador*);
void consumir_un_ciclo_de_cpu();
void llegar_a_el_pokemon(t_entrenador *);
void * planificar();
void crear_hilo_para_planificar();
bool todos_bloqueados_por_cantidad_maxima();

#endif /* TEAM_H_ */
