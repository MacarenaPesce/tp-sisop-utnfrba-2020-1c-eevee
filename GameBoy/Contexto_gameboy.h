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
#include <pthread.h>
#include<string.h>
#include <fcntl.h>
#include <errno.h>
#include <Pokebola.h>
#include <readline/readline.h>

/* Keys config file*/
#define KEY_CONFIG_IP_BROKER "IP_BROKER"
#define KEY_CONFIG_IP_TEAM "IP_TEAM"
#define KEY_CONFIG_IP_GAMECARD "IP_GAMECARD"
#define KEY_CONFIG_PUERTO_BROKER "PUERTO_BROKER"
#define KEY_CONFIG_PUERTO_TEAM "PUERTO_TEAM"
#define KEY_CONFIG_PUERTO_GAMECARD "PUERTO_GAMECARD"
#define KEY_CONFIG_ID "ID"

extern char* ip_broker;
extern char* ip_team;
extern char* ip_gamecard;
extern char* puerto_broker;
extern char* puerto_team;
extern char* puerto_gamecard;
extern int id;

extern int server_broker;
extern int server_team;
extern int server_gamecard;

extern t_log* gameboy_logger;
extern t_config* config;

//Enumeracion de los comandos de la consola
enum comandos { broker, team, gamecard, suscriptor};
enum tipo_mensaje { new_pokemon, appeared_pokemon, catch_pokemon, caught_pokemon, get_pokemon, localized_pokemon};
/*
typedef struct {
	char* tiempo;
	t_suscripcion * suscripcion;
	t_servidor * servidor;
} t_suscripcion_gameboy;*/

#endif /* CONTEXTO_GAMEBOY_H_ */
