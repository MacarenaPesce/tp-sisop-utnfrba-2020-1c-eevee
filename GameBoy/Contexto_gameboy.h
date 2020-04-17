/*
 * Contexto_gameboy.h
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#ifndef CONTEXTO_GAMEBOY_H_
#define CONTEXTO_GAMEBOY_H_

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
#define KEY_CONFIG_IP_BROKER "IP_BROKER"
#define KEY_CONFIG_IP_TEAM "IP_TEAM"
#define KEY_CONFIG_IP_GAMECARD "IP_GAMECARD"
#define KEY_CONFIG_PUERTO_BROKER "PUERTO_BROKER"
#define KEY_CONFIG_PUERTO_TEAM "PUERTO_TEAM"
#define KEY_CONFIG_PUERTO_GAMECARD "PUERTO_GAMECARD"

extern char* ip_broker;
extern char* ip_team;
extern char* ip_gamecard;
extern char* puerto_broker;
extern char* puerto_team;
extern char* puerto_gamecard;

extern t_log* gameboy_logger;
extern t_config* config;


#endif /* CONTEXTO_GAMEBOY_H_ */
