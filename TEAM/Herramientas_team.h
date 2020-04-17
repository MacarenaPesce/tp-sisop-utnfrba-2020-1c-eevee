/*
 * Herramientas_team.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef HERRAMIENTAS_TEAM_H_
#define HERRAMIENTAS_TEAM_H_

#include "Contexto_team.h"

void inicializar_logger();
void inicializar_archivo_de_configuracion();
void obtener_valor_config(char* key, t_config* file, void(*obtener)(void));
void obtener_las_posiciones_de_entrenadores();
void obtener_los_pokemon_de_entrenadores();
void obtener_los_objetivos_de_entrenadores();
void obtener_el_tiempo_de_reconexion();
void obtener_el_retardo_de_ciclo_de_cpu();
void obtener_el_algoritmo_de_planificacion();
void obtener_el_quantum();
void obtener_la_estimacion_inicial();
void obtener_la_ip_del_broker();
void obtener_el_puerto_del_broker();
void obtener_el_log_file();
void configurar_signals(void);
void capturar_signal(int signo);
void terminar_team_correctamente();

#endif /* HERRAMIENTAS_TEAM_H_ */
