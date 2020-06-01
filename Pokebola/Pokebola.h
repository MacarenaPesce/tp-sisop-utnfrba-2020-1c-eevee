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
	SUSCRIBIRSE_A_COLA,
};

enum TIPOS_SUSCRIPCION{
	SUSCRIPCION_GLOBAL,
	SUSCRIPCION_TEMPORAL
};

enum STATUS_CAUGHT{
	OK,
	FAIL
};

typedef struct{
	uint32_t posx __attribute__((packed));
	uint32_t posy __attribute__((packed));
} t_coordenadas;

typedef struct{	

	uint32_t id_mensaje __attribute__((packed));

	uint32_t id_correlacional __attribute__((packed));

	uint32_t tamanio_payload __attribute__((packed));

	enum COLA_DE_MENSAJES cola_de_mensajes __attribute__((packed));	 

	enum OPERACIONES operacion __attribute__((packed));

	void* mensaje;

} t_packed;

typedef struct{
	t_coordenadas coordenadas;
	uint32_t _tamanio_string_pokemon __attribute__((packed));
	char * pokemon;
} t_appeared_pokemon;

typedef t_appeared_pokemon t_catch_pokemon;

typedef struct{
	uint32_t status __attribute__((packed)); //OK o FAIL
} t_caught_pokemon;

typedef struct{
	enum TIPOS_SUSCRIPCION tipo_suscripcion __attribute__((packed));	 
	uint32_t minutos_suscripcion __attribute__((packed)); //OK o FAIL
} t_suscripcion;

typedef struct{
	t_coordenadas coordenadas;
	uint32_t cantidad __attribute__((packed));
	uint32_t _tamanio_string_pokemon __attribute__((packed));
	char * pokemon;
} t_new_pokemon;

typedef struct{
	uint32_t _tamanio_string_pokemon __attribute__((packed));
	char * pokemon;
} t_get_pokemon;

typedef struct{
	uint32_t cantidad_coordenadas __attribute__((packed));
	uint32_t _tamanio_string_pokemon __attribute__((packed));
	char * pokemon;
	t_list lista_coordenadas;
} t_localized_pokemon;


t_packed* recibir_mensaje(int sock);
void _agregar_string_a_paquete(t_packed* paquete, char* string_value);

/*
 * HAY VARIAS COSAS QUE SE PUEDEN REUTILIZAR PARA NO DUPLICAR CODIGO
 * PERO ESTA HECHO DE ESTA MANERA PARA RESPETAR EL PROTOCOLO
 */

/**************************************************************************************/
int conectar_a_server(char*, char*);
void cerrar_conexion(int);
t_packed* recibir_mensaje(int sock);
void eliminar_mensaje(t_packed* paquete);
void enviar_new_pokemon(int socket, uint32_t id_mensaje, uint32_t id_correlacional, t_new_pokemon* new_pokemon);
void logger(int tipo_esc, int tipo_log, const char* mensaje, ...);

/*
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