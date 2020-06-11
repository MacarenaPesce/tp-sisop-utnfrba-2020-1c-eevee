/*
 * Contexto_team.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef CONTEXTO_TEAM_H_
#define CONTEXTO_TEAM_H_

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
#include <commons/collections/queue.h>
#include <pthread.h>
#include <sys/select.h>		// Select
#include <sys/time.h>
#include <math.h>
#include <semaphore.h>


/* Keys config file*/
#define KEY_CONFIG_POSICIONES_ENTRENADORES "POSICIONES_ENTRENADORES"
#define KEY_CONFIG_POKEMON_ENTRENADORES "POKEMON_ENTRENADORES"
#define KEY_CONFIG_OBJETIVOS_ENTRENADORES "OBJETIVOS_ENTRENADORES"
#define KEY_CONFIG_TIEMPO_RECONEXION "TIEMPO_RECONEXION"
#define KEY_CONFIG_RETARDO_CICLO_CPU "RETARDO_CICLO_CPU"
#define KEY_CONFIG_ALGORITMO_PLANIFICACION "ALGORITMO_PLANIFICACION"
#define KEY_CONFIG_QUANTUM "QUANTUM"
#define KEY_CONFIG_ESTIMACION_INICIAL "ESTIMACION_INICIAL"
#define KEY_CONFIG_IP_BROKER "IP_BROKER"
#define KEY_CONFIG_PUERTO_BROKER "PUERTO_BROKER"
#define KEY_CONFIG_LOG_FILE "LOG_FILE"
#define KEY_CONFIG_ALPHA "ALPHA"

#define NO_SOCKET -1
#define IP "127.0.0.2"
#define PUERTO "5002"
#define MAX_CLIENTES 20

extern char** posiciones_entrenadores;
extern char** pokemon_entrenadores;
extern char** objetivos_entrenadores;
extern int tiempo_reconexion;
extern int retardo_ciclo_cpu;
extern char* algoritmo_planificacion;
extern int quantum;
extern int alpha;
extern char* ip_broker;
extern int estimacion_inicial;
extern char* puerto_broker;
extern char* log_file;

extern int GLOBAL_SEGUIR;

extern t_log* team_logger;
extern t_log* team_logger_oficial;
extern t_config* config;

extern t_list* lista_entrenadores; //lista de entrenadores cargada.
extern t_list* lista_objetivos;
extern t_list* lista_global_objetivos;
extern t_list* lista_global_pokemones;
extern t_list* lista_objetivos_de_entrenador;
extern t_list* lista_pokemones_de_entrenador;
extern t_list* lista_pokemones_objetivos;
extern t_list* lista_listos;
extern t_list* lista_finalizar;
extern t_list* lista_bloqueados;
extern t_list* lista_config; //lista auxiliar para cargar la info del archivo de configuracion
extern t_list* pokemones_ordenada; //lista auxiliar para calcular el objetivo global
extern t_list* lista_mapa;
extern t_list* lista_pokemon_atrapado;

pthread_mutex_t entrenador_exec;
sem_t hilo_entrenador;

//Estructura de un entrenador
enum ESTADO{
	NUEVO,
	LISTO,
	EJECUTANDO,
	BLOQUEADO,
	FINALIZANDO
};

enum RAZON_BLOQUEO{
	ESPERANDO_MENSAJE_CAUGHT,
	ESPERANDO_DEADLOCK,
	CANTIDAD_MAXIMA_ALCANZADA,
	ESPERANDO_POKEMON
};

typedef struct { //estructura del objetivo global
	char* especie;
	uint32_t posx;
	uint32_t posy;
} t_pokemon;

typedef struct {
	int id;
	uint32_t posx;
	uint32_t posy;
	enum ESTADO estado;
	enum RAZON_BLOQUEO razon_de_bloqueo;
	t_list *objetivo;
	t_list *pokemones;
	uint32_t cant_maxima_objetivos;
	t_pokemon* objetivo_actual;
	float estimacion_real;//sjf
	float estimacion_actual;//sjf
	float estimacion_anterior;//sjf
	int instruccion_actual;//sjf
	int ejec_anterior;//sjf
} t_entrenador;

typedef struct { //estructura del objetivo global
	char* especie;
	uint32_t cantidad_atrapada;
	uint32_t cantidad_necesitada;
} t_objetivo;

typedef struct { //estructura del objetivo de un entrenador
	char* especie;
	uint32_t cantidad;
} t_objetivo_entrenador;


t_entrenador * entrenador_en_ejecucion;

#endif /* CONTEXTO_TEAM_H_ */
