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
#define KEY_CONFIG_ID "ID"

#define NO_SOCKET -1
#define IP "127.0.0.2"
#define PUERTO "5002"

extern char** posiciones_entrenadores;
extern char** pokemon_entrenadores;
extern char** objetivos_entrenadores;
extern int tiempo_reconexion;
extern int retardo_ciclo_cpu;
extern char* algoritmo_planificacion;
extern int quantum;
extern float alpha;
extern char* ip_broker;
extern int estimacion_inicial;
extern char* puerto_broker;
extern char* log_file;
extern int id;

extern int GLOBAL_SEGUIR;
extern int CONTADOR_DE_MENSAJES;
extern int ciclos_de_cpu;
extern int hilos_entrenadores_total;
extern int MAXIMO_ENTRENADORES;
extern int CANTIDAD_EN_DEADLOCK;
extern bool hayPokeNuevo;
extern uint32_t quantum_actual;
extern bool desalojo_en_ejecucion;

extern uint32_t cambios_de_contexto;
extern uint32_t deadlocks_producidos;
extern uint32_t deadlocks_resueltos;

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
extern t_list* mensajes;
extern t_list* pokemones_bloqueados;
extern t_list* lista_bloqueados_esperando;
extern t_list* lista_bloqueados_cant_max_alcanzada;
extern t_list* lista_bloqueados_deadlock;
extern t_list* mensajes_que_llegan_nuevos;
extern t_list* mensajes_para_chequear_id;
extern t_list* lista_bloqueados_esperando_caught;

sem_t * array_semaforos;
sem_t * array_semaforos_rr;
sem_t * array_semaforos_finalizar;
sem_t * array_semaforos_deadlock;
sem_t * array_semaforos_caught;
sem_t entrenadores_ubicados;
pthread_mutex_t mapa_mutex;
pthread_mutex_t llego_gameboy;
pthread_mutex_t lista_bloq_max_mutex;
pthread_mutex_t lista_entrenadores_mutex;
pthread_mutex_t lista_listos_mutex;
pthread_mutex_t mensaje_chequear_id_mutex;
pthread_mutex_t moverse;
pthread_mutex_t mutex_para_colas;
pthread_mutex_t tocando_pokemones_objetivos;
pthread_mutex_t tocando_semaforos_deadlock;
pthread_mutex_t lista_comun_deadlock;
pthread_mutex_t mutex_ciclos_cpu;

pthread_mutex_t mensaje_nuevo_mutex;
sem_t mensaje_nuevo_disponible;

t_list * semaforos_deadlock;

sem_t semaforos_listos;
sem_t hay_interbloqueo;
sem_t hay_interbloqueo_avisar_a_entrenador;
pthread_mutex_t mutex_deadlock;

sem_t ejecucion;
sem_t operar_con_catch;
sem_t operar_con_appeared;
sem_t operar_con_localized;
sem_t operar_con_caught;
sem_t orden_para_planificar;
sem_t aviso_entrenador_hizo_intercambio;
sem_t se_hizo_el_intercambio;
sem_t chequeo_de_deadlock;
sem_t todos_los_entrenadores_finalizaron;
sem_t me_bloquee;
sem_t puedo_volver_a_ejecutar; 

extern bool hayDeadlock;
extern bool me_desalojaron;

//Estructura de un entrenador
enum ESTADO{
	NUEVO,
	LISTO,
	EJECUTANDO,
	BLOQUEADO,
	FINALIZANDO
};

enum OPERACION{
	LOCALIZED,
	APPEARED,
	CAUGHT,
	GET,
	CATCH
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
	bool desalojado;
	uint32_t quantum_restante;
	bool agoto_quantum;
	uint32_t ciclos_de_cpu;
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

typedef struct {
	uint32_t id;
	uint32_t id_correlacional;
	void * contenido;
	enum OPERACION operacion;
}t_mensaje_guardado;

typedef struct {
	uint32_t id;
	uint32_t id_correlacional;
	void * contenido;
	enum OPERACION operacion;
}t_mensaje_ver_id;

typedef struct {
	int cola;
	void *(*operacion)(t_packed*);
}t_suscripcion_a_broker;

typedef struct {
	int id_entrenador;
	sem_t * semaforo;
}t_semaforo_deadlock;

t_entrenador * entrenador_en_ejecucion;
t_entrenador * entrenador_por_desalojar;
t_entrenador * nuevo_entrenador;

#endif /* CONTEXTO_TEAM_H_ */
