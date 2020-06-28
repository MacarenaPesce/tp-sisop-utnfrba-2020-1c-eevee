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
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <sys/ioctl.h>
#include <semaphore.h>

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

enum SERVER_STATUS{
	STARTING,
	RUNNING,
	ENDING
};

enum COLA_DE_MENSAJES{
	COLA_APPEARED_POKEMON,
	COLA_NEW_POKEMON,
	COLA_CATCH_POKEMON,
	COLA_GET_POKEMON,
	COLA_CAUGHT_POKEMON,
	COLA_LOCALIZED_POKEMON,
	MENSAJE_TEXTO
};

enum OPERACIONES{
	ENVIAR_MENSAJE,
	SUSCRIBIRSE_A_COLA,
	ACK
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
	uint32_t id_cliente __attribute__((packed));
	char* ip;
	char* puerto;
} t_servidor;

typedef struct{	

	uint32_t id_mensaje __attribute__((packed));

	uint32_t id_correlacional __attribute__((packed));
	
	uint32_t id_cliente __attribute__((packed));

	uint32_t tamanio_payload __attribute__((packed));

	enum COLA_DE_MENSAJES cola_de_mensajes __attribute__((packed));	 

	enum OPERACIONES operacion __attribute__((packed));

	void* mensaje;

} t_packed;

typedef struct{
	t_coordenadas coordenadas;
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
	char * pokemon;
} t_new_pokemon;

typedef struct{
	char * pokemon;
} t_get_pokemon;

typedef struct{
	uint32_t cantidad_coordenadas __attribute__((packed));
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
/* Typedef Broker */

typedef struct{
	uint32_t id_mensaje __attribute__((packed));
    uint32_t id_correlacional __attribute__((packed));
	uint32_t tamanio_mensaje __attribute__((packed));
    enum COLA_DE_MENSAJES cola_de_mensajes __attribute__((packed));
    t_list* suscriptores_enviados;
    t_list* suscriptores_ack;
    void* mensaje;
}t_mensaje_cola;

typedef struct{    
    int tamanio_particion __attribute__((packed));
    bool esta_vacio;
    t_mensaje_cola* estructura_mensaje; //EN CASO QUE esta_vacio SEA TRUE, ESTO APUNTA AL PAYLOAD DIRECTO
    uint64_t timestamp;
    uint64_t last_time;    
}t_bloque_memoria;

typedef struct{
 t_list* memoria;
 t_list* colas; 
 t_list* clientes;
 int proximo_id_mensaje;
}t_cache_colas;

typedef struct{
    int id;
    int cliente;
}t_envio_pendiente;

typedef struct{
    enum COLA_DE_MENSAJES cola_de_mensajes __attribute__((packed));
    t_list* envios_pendientes;
    t_list* suscriptores;
    sem_t* producciones; 
}t_cola_mensajes;



typedef struct{
	uint32_t id __attribute__((packed));
	int socket __attribute__((packed));
}t_cliente;

/*  */


int conectar_a_server(char*, char*);
void cerrar_conexion(int);
t_packed* recibir_mensaje(int sock);
t_packed* recibir_mensaje_serealizado(int sock);
void deserializar_paquete(t_packed* paquete);

void eliminar_mensaje(t_packed* paquete);
void logger(int tipo_esc, int tipo_log, const char* mensaje, ...);


t_packed* enviar_mensaje_string(t_servidor* servidor, char* mensaje);
t_packed* enviar_appeared_pokemon(t_servidor* servidor, uint32_t id_correlacional, t_appeared_pokemon* appeared_pokemon );
t_packed* enviar_catch_pokemon(t_servidor* servidor, uint32_t id_correlacional, t_catch_pokemon* catch_pokemon);
t_packed* enviar_new_pokemon(t_servidor* servidor, uint32_t id_correlacional, t_new_pokemon* new_pokemon);
t_packed* enviar_caught_pokemon(t_servidor* servidor, uint32_t id_correlacional, t_caught_pokemon* caught_pokemon);
t_packed* enviar_get_pokemon(t_servidor* servidor, uint32_t id_correlacional, t_get_pokemon* get_pokemon);
t_packed* enviar_localized_pokemon(t_servidor* servidor, uint32_t id_correlacional, t_localized_pokemon* localized_pokemon);
t_packed* distribuir_mensaje_string(int socket, char* mensaje);
int enviar_solicitud_suscripcion(t_servidor* servidor,uint32_t cola_de_mensajes, t_suscripcion* suscripcion);
int enviar_ack(t_servidor* servidor, uint32_t id_mensaje);
/**************************************************************************************/

int distribuir_appeared_pokemon(int socket, uint32_t id_mensaje, uint32_t id_correlacional,uint32_t id_cliente, t_appeared_pokemon* appeared_pokemon );
int distribuir_catch_pokemon(int socket, uint32_t id_mensaje, uint32_t id_correlacional,uint32_t id_cliente, t_catch_pokemon* catch_pokemon);
int distribuir_new_pokemon(int socket, uint32_t id_mensaje, uint32_t id_correlacional,uint32_t id_cliente, t_new_pokemon* new_pokemon);
int distribuir_caught_pokemon(int socket, uint32_t id_mensaje, uint32_t id_correlacional,uint32_t id_cliente, t_caught_pokemon* caught_pokemon);
int distribuir_get_pokemon(int socket, uint32_t id_mensaje, uint32_t id_correlacional,uint32_t id_cliente, t_get_pokemon* get_pokemon);
int distribuir_localized_pokemon(int socket, uint32_t id_mensaje, uint32_t id_correlacional,uint32_t id_cliente, t_localized_pokemon* localized_pokemon);
int distribuir_ack(int socket,uint32_t id_mensaje, uint32_t id_cliente);
t_packed* _esperar_ack(int socket);

/**************************************************************************************/

void _recuperar_mensaje(t_packed *paquete);
void _recibir_solicitud_suscripcion(t_packed *paquete);
void _recibir_localized_pokemon(t_packed *paquete);
void _recibir_get_pokemon(t_packed *paquete);
void _recibir_mensaje_string(t_packed *paquete);
void _recibir_catch_o_appeared_pokemon(t_packed *paquete);
void _recibir_new_pokemon(t_packed *paquete);
void _recibir_caught_pokemon(t_packed *paquete);

#endif /* POKEBOLA_H_ */