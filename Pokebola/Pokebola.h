/*
 * Pokebola.h
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#ifndef POKEBOLA_H_
#define POKEBOLA_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include<netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>


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

/***********************PROTOCOLO DE COMUNICACION***********************************/

enum PROCESOS{
	BROKER,
	TEAM,
	GAMEBOY,
	GAMECARD
};

enum COLA_DE_MENSAJES{
	COLA_APPEARED_POKEMON,
	COLA_NEW_POKEMON,
	COLA_CATCH_POKEMON,
	COLA_GET_POKEMON,
	COLA_CAUGHT_POKEMON,
	COLA_LOCALIZED_POKEMON
};

enum OPERACIONES{
	ENVIAR_MENSAJE,
	SUSCRIPTOR_GLOBAL,
	SUSCRIPTOR_TEMPORAL
};

enum STATUS_CAUGHT{
	OK,
	FAIL
};

typedef struct{	

	uint32_t id_mensaje __attribute__((packed));

	uint32_t id_correlacional __attribute__((packed));

	uint32_t tamanio_payload __attribute__((packed));

	enum COLA_DE_MENSAJES cola_de_mensajes __attribute__((packed));	 

	enum OPERACIONES operacion __attribute__((packed));

	void* mensaje;

} t_packed;

typedef struct t_payload{
	uint32_t tamanio __attribute__((packed));
	void*    buffer;
} t_payload;


typedef struct stru_appeared_pokemon{
	char * pokemon;
	uint32_t posx;
	uint32_t posy;
	uint32_t id_mensaje;
} t_appeared_pokemon;
typedef t_appeared_pokemon* tp_appeared_pokemon;


/*
 * ACLARACION, APPEARED POKEMON PARA TEAM NO TIENE ID_MENSAJE
 * ENTONCES HAGO UNA ESTRUCTURA A PARTE
 */

typedef struct stru_appeared_pokemon_team{
	char * pokemon;
	uint32_t posx;
	uint32_t posy;
} t_appeared_pokemon_team;
typedef t_appeared_pokemon_team* tp_appeared_pokemon_team;


typedef struct stru_catch_pokemon{
	char * pokemon;
	uint32_t posx;
	uint32_t posy;
} t_catch_pokemon;
typedef t_catch_pokemon* tp_catch_pokemon;

typedef struct stru_caught_pokemon{
	uint32_t id_mensaje;
	uint32_t status; //OK o FAIL
} t_caught_pokemon;
typedef t_caught_pokemon* tp_caught_pokemon;

typedef struct stru_new_pokemon{
	char * pokemon;
	uint32_t posx;
	uint32_t posy;
	uint32_t cantidad;
} t_new_pokemon;
typedef t_new_pokemon* tp_new_pokemon;

typedef struct stru_get_pokemon{
	char * pokemon;
} t_get_pokemon;
typedef t_get_pokemon* tp_get_pokemon;

typedef struct stru_modo_suscriptor{
	uint32_t cola_de_mensajes;
	uint32_t tiempo;
} t_modo_suscriptor;
typedef t_modo_suscriptor* tp_modo_suscriptor;




t_packed* recibir_paquete(int sock);
void _agregar_string_a_paquete(t_packed* paquete, char* string_value, int size);

/*
 * HAY VARIAS COSAS QUE SE PUEDEN REUTILIZAR PARA NO DUPLICAR CODIGO
 * PERO ESTA HECHO DE ESTA MANERA PARA RESPETAR EL PROTOCOLO
 */

/**************************************************************************************/
/*int conectar_a_server(char*, char*);
void cerrar_conexion(int);
int enviar_mensaje(int sock, void *mensaje, int tamanio);
int recibir_mensaje(int sock, void *mensaje, int tamanio);
int enviar_header(int, enum MENSAJES tipo_de_mensaje,int);
t_header recibir_header(int);
t_paquete* crear_paquete(enum MENSAJES tipo_de_mensaje);
void crear_buffer(t_paquete*);
void* serializar_paquete(t_paquete*, int);
void agregar_string_a_paquete(t_paquete*, void*, int);
void agregar_int_a_paquete(t_paquete*, int);
void agregar_uint32_t_a_paquete(t_paquete*, uint32_t);
void eliminar_paquete(t_paquete*);

void enviar_mensaje_char(char*, int);
tp_mensaje_char recibir_mensaje_char(int, int);

void enviar_appeared_pokemon(char*, uint32_t, uint32_t, uint32_t, int);
tp_appeared_pokemon recibir_appeared_pokemon(int paquete_size, int);

void enviar_new_pokemon(char*, uint32_t, uint32_t, uint32_t, int);
tp_new_pokemon recibir_new_pokemon(int, int);

void enviar_catch_pokemon(char*, uint32_t, uint32_t, int);
tp_catch_pokemon recibir_catch_pokemon(int, int);

void enviar_appeared_pokemon_team(char*, uint32_t, uint32_t, int);
tp_appeared_pokemon_team recibir_appeared_pokemon_team(int, int);

void enviar_get_pokemon(char*, int);
tp_get_pokemon recibir_get_pokemon(int, int);

void enviar_caught_pokemon(uint32_t, uint32_t, int);
tp_caught_pokemon recibir_caught_pokemon(int, int);

void enviar_modo_suscriptor(uint32_t, uint32_t, int);
tp_modo_suscriptor recibir_modo_suscriptor(int, int);

/**************************************************************************************/



#endif /* POKEBOLA_H_ */