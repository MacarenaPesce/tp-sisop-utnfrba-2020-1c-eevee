/*
 * Contexto_team.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Contexto_team.h"

/* Global variables*/
char** posiciones_entrenadores;
char** pokemon_entrenadores;
char** objetivos_entrenadores;
int tiempo_reconexion;
int retardo_ciclo_cpu;
char* algoritmo_planificacion;
int quantum;
int alpha;
char* ip_broker;
int estimacion_inicial;
char* puerto_broker;
char* log_file;

int GLOBAL_SEGUIR = 1;
int ciclos_de_cpu = 0;
int MAXIMO_ENTRENADORES;
int CANTIDAD_EN_DEADLOCK;

bool hayDeadlock;

t_log* team_logger;
t_log* team_logger_oficial;
t_config* config;

t_list* lista_entrenadores; //lista de entrenadores cargada.
t_list* lista_objetivos; //una lista de objetivos para cada entrenador, esto falta.
t_list* lista_global_objetivos;
t_list* lista_global_pokemones;
t_list* lista_objetivos_de_entrenador;
t_list* lista_pokemones_de_entrenador;
t_list* lista_pokemones_objetivos;
t_list* lista_listos;
t_list* lista_finalizar;
t_list* lista_bloqueados;
t_list* lista_config; //lista auxiliar para cargar la info del archivo de configuracion
t_list* pokemones_ordenada; //lista auxiliar para calcular el objetivo global
t_list* lista_mapa;
t_list* lista_pokemon_atrapado;
t_list* mensajes;
t_list* lista_bloqueados_esperando;
t_list* lista_bloqueados_cant_max_alcanzada;
t_list* lista_bloqueados_deadlock;
