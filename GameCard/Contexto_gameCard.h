/*
 * Contexto_team.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef CONTEXTO_GAMECARD_H_
#define CONTEXTO_GAMECARD_H_

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
#define KEY_TIEMPO_DE_REINTENTO_CONEXION "TIEMPO_DE_REINTENTO_CONEXION"
#define KEY_TIEMPO_DE_REINTENTO_OPERACION "TIEMPO_DE_REINTENTO_DE_OPERACION"
#define KEY_PUNTO_MONTAJE_TALLGRASS "PUNTO_DE_MONTAJE_TALLGRASS"
#define KEY_IP_BROKER "IP_BROKER"
#define KEY_PUERTO_BROKER "PUERTO_BROKER"

#define IP "127.0.0.2"
#define PUERTO "5002"
/* adaptar a mi modulo*/

extern int tiempo_reintento_conexion;
extern int tiempo_reintento_operacion;
extern char* punto_montaje_fs;
extern char* ip_broker;
extern char* puerto_broker;

//estas dos arreglarlas para cada proceso ojo
extern t_log* gameCard_logger;
extern t_config* config_game_card;

#endif /* CONTEXTO_GAMECARD_H_ */
