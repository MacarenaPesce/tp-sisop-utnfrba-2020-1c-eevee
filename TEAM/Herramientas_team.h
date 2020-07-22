/*
 * Herramientas_team.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef HERRAMIENTAS_TEAM_H_
#define HERRAMIENTAS_TEAM_H_

#include "Contexto_team.h"

/*Destruccion de listas*/
int destruir_pokemon(t_pokemon *);
int destruir_entrenador(t_entrenador *);
int destruir_objetivo(t_objetivo *);
int destruir_objetivo_entrenador(t_objetivo_entrenador *);
int destruir_mensaje(t_mensaje_guardado *);
int destruir_mensaje_catch(t_mensaje_guardado_catch *);
void liberar_lista_char(char**);

/*SETEOS INICIALES*/
void inicializar_logger();
void inicializar_semaforos();
void inicializar_semaforos_deadlock();
void inicializar_archivo_de_configuracion();
void configurar_signals(void);
void capturar_signal(int signo);


/*PROPIAS DEL ARCHIVO DE CONFIGURACION*/
void obtener_valor_config(char* key, t_config* file, void(*obtener)(void));
void obtener_el_id();
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
void obtener_el_puerto_de_team();

/*PARA FINALIZAR EL PROCESO CORRECTAMENTE*/
void terminar_team_correctamente();


/*BUSQUEDA*/
t_objetivo * buscar_pokemon_por_especie(t_list*, char*);
t_objetivo_entrenador * buscar_pokemon_objetivo_por_especie(t_list*, char*);
t_entrenador * buscar_entrenador_por_id(t_list*, int);
t_entrenador * buscar_entrenador_por_ubicacion(t_list*, uint32_t, uint32_t);
t_pokemon * buscar_pokemon_por_especie_y_ubicacion(t_list *, t_pokemon *);
t_entrenador * buscar_entrenador_por_objetivo_actual(t_catch_pokemon*);
t_mensaje_guardado * buscar_mensaje(uint32_t);
uint32_t * buscar_mensaje_por_id(uint32_t, t_list*);
t_mensaje_guardado_catch * buscar_mensaje_por_id_catch(uint32_t, t_list*);
t_mensaje_guardado * buscar_mensaje_appeared_por_especie(char*, t_list*);


/*CHEQUEOS CON BOOL*/
bool objetivo_personal_cumplido(t_entrenador* entrenador);
bool objetivo_global_cumplido();
bool todos_bloqueados_por_cantidad_maxima();

/* Agregar a listas */
void agregar_a_historico_appeared(char* pokemon);

/*MOSTRAR*/
void mostrar_lo_que_hay_en_la_lista_de_objetivos_del_entrenador(t_list *);

#endif /* HERRAMIENTAS_TEAM_H_ */
