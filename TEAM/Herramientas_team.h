/*
 * Herramientas_team.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef HERRAMIENTAS_TEAM_H_
#define HERRAMIENTAS_TEAM_H_

#include "Contexto_team.h"

/*SETEOS INICIALES*/
void inicializar_logger();
void inicializar_semaforos();
void inicializar_archivo_de_configuracion();
void configurar_signals(void);
void capturar_signal(int signo);


/*PROPIAS DEL ARCHIVO DE CONFIGURACION*/
void obtener_valor_config(char* key, t_config* file, void(*obtener)(void));
void obtener_las_posiciones_de_entrenadores();
void obtener_los_pokemon_de_entrenadores();
void obtener_los_objetivos_de_entrenadores();
void obtener_el_tiempo_de_reconexion();
void obtener_el_alpha();
void obtener_el_retardo_de_ciclo_de_cpu();
void obtener_el_algoritmo_de_planificacion();
void obtener_el_quantum();
void obtener_la_estimacion_inicial();
void obtener_la_ip_del_broker();
void obtener_el_puerto_del_broker();
void obtener_el_log_file();


/*PARA FINALIZAR EL PROCESO CORRECTAMENTE*/
void terminar_team_correctamente();
int destruir_entrenador(t_entrenador *);
int destruir_objetivo(t_objetivo *);


/*BUSQUEDA*/
t_objetivo * buscar_pokemon_por_especie(t_list*, char*);
t_entrenador * buscar_entrenador_por_id(t_list*, int);
t_entrenador * buscar_entrenador_por_objetivo_actual(t_catch_pokemon*);
t_mensaje_guardado * buscar_mensaje(uint32_t id);


/*CHEQUEOS CON BOOL*/
bool objetivo_personal_cumplido(t_entrenador* entrenador);
bool objetivo_global_cumplido();


#endif /* HERRAMIENTAS_TEAM_H_ */
