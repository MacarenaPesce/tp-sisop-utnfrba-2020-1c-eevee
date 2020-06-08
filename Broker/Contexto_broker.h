#ifndef CONTEXTO_BROKER_H_
#define CONTEXTO_BROKER_H_

#include "Memoria.h"
#include "Herramientas_broker.h"
//#include "Contexto_broker.h"
#include "Broker.h"
#include "utils.h"
#include <Pokebola.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <stdarg.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>

#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

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

extern int tamanio_memoria;
extern int tamanio_minimo_particion;
extern char* algoritmo_memoria;
extern char* algoritmo_reemplazo;
extern char* algoritmo_particion_libre;
extern char* ip_broker;
extern int frecuencia_compactacion;
extern int puerto_broker;
extern char* log_file;

extern t_log* broker_logger;
extern t_config* config;

#endif
