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
int puerto_broker;
char* log_file;

t_log* team_logger;
t_config* config;
