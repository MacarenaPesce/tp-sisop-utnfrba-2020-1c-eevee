#ifndef CONTEXTO_BROKER_H_
#define CONTEXTO_BROKER_H_

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
#include <Pokebola.h>


/* Keys config file*/
#define KEY_CONFIG_TAMANIO_MEMORIA "TAMANIO_MEMORIA"
#define KEY_CONFIG_TAMANIO_MINIMO_PARTICION "TAMANIO_MINIMO_PARTICION"
#define KEY_CONFIG_ALGORITMO_MEMORIA "ALGORITMO_MEMORIA"
#define KEY_CONFIG_ALGORITMO_REEMPLAZO "ALGORITMO_REEMPLAZO"
#define KEY_CONFIG_ALGORITMO_PARTICION_LIBRE "ALGORITMO_PARTICION_LIBRE"
#define KEY_CONFIG_IP_BROKER "IP_BROKER"
#define KEY_CONFIG_PUERTO_BROKER "PUERTO_BROKER"
#define KEY_CONFIG_FRECUENCIA_COMPACTACION "RECUENCIA_COMPACTACION"
#define KEY_CONFIG_LOG_FILE "LOG_FILE"

#define IP "127.0.0.2"
#define PUERTO "5002"

extern char** posiciones_entrenadores;
extern char** pokemon_entrenadores;
extern char** objetivos_entrenadores;
extern int tiempo_reconexion;
extern int retardo_ciclo_cpu;
extern char* algoritmo_planificacion;
extern int quantum;
extern char* ip_broker;
extern int estimacion_inicial;
extern char* puerto_broker;
extern char* log_file;

extern t_log* broker_logger;
extern t_config* config;

#endif
