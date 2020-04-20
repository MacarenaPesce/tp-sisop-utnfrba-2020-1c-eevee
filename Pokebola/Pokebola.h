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

enum MENSAJES{
	CHAR_MESSAGE,
};

typedef struct{
	int tamanio;
	enum MENSAJES tipo_de_mensaje;
}__attribute__((packed)) t_header;

typedef struct{
	void* stream;
} t_buffer;

typedef struct{
	t_header header;
	t_buffer* buffer;
} t_paquete;

typedef struct stru_mensaje_char{
	char * mensaje;
} t_mensaje_char;
typedef t_mensaje_char* tp_mensaje_char;

typedef struct stru_new_pokemon{
	char * pokemon;
	int posx;
	int posy;
	int cantidad;
} t_new_pokemon;
typedef t_new_pokemon* tp_new_pokemon;



/**************************************************************************************/
void cerrar_conexion(int);
int enviar_mensaje(int sock, void *mensaje, int tamanio);
int recibir_mensaje(int sock, void *mensaje, int tamanio);
int enviar_header(int, enum MENSAJES tipo_de_mensaje,int);
t_header recibir_header(int);
t_paquete* crear_paquete(enum MENSAJES tipo_de_mensaje);
void crear_buffer(t_paquete*);
void enviar_paquete(t_paquete*, int);
void* serializar_paquete(t_paquete*, int);
void agregar_string_a_paquete(t_paquete*, void*, int);
void agregar_int_a_paquete(t_paquete*, int);
void eliminar_paquete(t_paquete*);
void enviar_mensaje_char(char*, int);
tp_mensaje_char recibir_mensaje_char(int, int);
void escribir_en_pantalla(int tipo_esc, int tipo_log, char* console_buffer,char* log_colors[8], char* msj_salida);
void definir_nivel_y_loguear(int, int, char*);
void logger(int, int, const char*, ...);
void agregar_uint32_t_a_paquete(t_paquete*, uint32_t);

#endif /* POKEBOLA_H_ */
