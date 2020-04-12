/*
 * Contexto_team.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef CONTEXTO_TEAM_H_
#define CONTEXTO_TEAM_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include <string.h>
#include<sys/socket.h>
#include<commons/log.h>
#include<signal.h>
#include<stdarg.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<commons/config.h>
#include <commons/bitarray.h>
#include<string.h>
#include <fcntl.h>
#include <errno.h>


/* Keys config file*/
#define KEY_CONFIG_POSICIONES_ENTRENADORES "POSICIONES_ENTRENADORES"
#define KEY_CONFIG_POKEMON_ENTRENADORES "POKEMON_ENTRENADORES"
#define KEY_CONFIG_OBJETIVOS_ENTRENADORES "OBJETIVOS_ENTRENADORES"
#define KEY_CONFIG_TIEMPO_RECONEXION "TIEMPO_RECONEXION"
#define KEY_CONFIG_RETARDO_CICLO_CPU "RETARDO_CICLO_CPU"
#define KEY_CONFIG_ALGORITMO_PLANIFICACION "ALGORITMO_PLANIFICACION"
#define KEY_CONFIG_QUANTUM "QUANTUM"
#define KEY_CONFIG_ESTIMACION_INICIAL "ESTIMACION_INICIAL"
#define KEY_CONFIG_IP_BROKER "IP_BROKER"
#define KEY_CONFIG_PUERTO_BROKER "PUERTO_BROKER"
#define KEY_CONFIG_LOG_FILE "LOG_FILE"

extern char** posiciones_entrenadores;
extern char** pokemon_entrenadores;
extern char** objetivos_entrenadores;
extern int tiempo_reconexion;
extern int retardo_ciclo_cpu;
extern char* algoritmo_planificacion;
extern int quantum;
extern char* ip_broker;
extern int estimacion_inicial;
extern int puerto_broker;
extern char* log_file;

extern t_log* team_logger;
extern t_config* config;

/*** Enums log***/
enum type_log {
	escribir,
	loguear,
	escribir_loguear,
	l_trace,
	l_debug,
	l_info,
	l_warning,
	l_error
};


#endif /* CONTEXTO_TEAM_H_ */
