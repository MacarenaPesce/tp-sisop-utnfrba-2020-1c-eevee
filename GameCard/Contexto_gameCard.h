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
#include <sys/mman.h>
#include <pthread.h>
#include <Pokebola.h>

/* Keys config file*/
#define KEY_TIEMPO_DE_REINTENTO_CONEXION "TIEMPO_DE_REINTENTO_CONEXION"
#define KEY_TIEMPO_DE_REINTENTO_OPERACION "TIEMPO_DE_REINTENTO_DE_OPERACION"
#define KEY_TIEMPO_RETARDO_OPERACION "TIEMPO_RETARDO_OPERACION​"
#define KEY_PUNTO_MONTAJE_TALLGRASS "PUNTO_DE_MONTAJE_TALLGRASS"
#define KEY_IP_BROKER "IP_BROKER"
#define KEY_PUERTO_BROKER "PUERTO_BROKER"
#define KEY_CONFIG_ID "ID"
#define IP "127.0.0.3"
#define PUERTO "5001"
#define KEY_TAM_BLOQUE "BLOCK_SIZE"
#define KEY_CANT_BLOQUES "BLOCKS"
#define KEY_NOMBRE_FS "MAGIC_NUMBER"

extern int tiempo_reintento_conexion;
extern int tiempo_reintento_operacion;
extern int tiempo_retardo_operacion;
extern char* punto_montaje_fs;
extern char* ip_broker;
extern char* puerto_broker;
extern int id;

extern char* magicNumber;
extern int tamanioDeBloque;
extern int cantidadBloques;
extern char* pokemonEnMemoria;

//estas dos arreglarlas para cada proceso ojo
extern t_log* gameCard_logger;
extern t_config* config_game_card;

typedef struct {
	int cola;
	void *(*operacion)(t_packed*);
}t_suscripcion_a_broker;

#endif /* CONTEXTO_GAMECARD_H_ */
