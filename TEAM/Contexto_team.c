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
char* ip_broker;
int estimacion_inicial;
char* puerto_broker;
char* log_file;

t_log* team_logger;
t_log* team_logger_oficial;
t_config* config;

t_list* lista_entrenadores; //lista de entrenadores cargada.
t_list* lista_objetivos; //una lista de objetivos para cada entrenador, esto falta.
t_queue* cola_listos;
t_queue* cola_finalizar;
t_queue* cola_bloqueados;
t_list* lista_config; //lista auxiliar para cargar la info del archivo de configuracion
t_list* pokemones_ordenada; //lista auxiliar para calcular el objetivo global
