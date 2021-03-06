/*
 * Contexto_team.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Contexto_team.h"

/* Global variables*/
char** posiciones_entrenadores;
char** pokemon_entrenadores;
char** objetivos_entrenadores;
int tiempo_reconexion;
int retardo_ciclo_cpu;
char* algoritmo_planificacion;
int quantum;
float alpha;
char* ip_broker;
int estimacion_inicial;
char* puerto_broker;
char* puerto_team;
char* log_file;
int id;

int GLOBAL_SEGUIR = 1;
int CONTADOR_DE_MENSAJES;
int ciclos_de_cpu = 0;
uint32_t deadlocks_resueltos = 0;
uint32_t deadlocks_producidos = 0;
uint32_t cambios_de_contexto = 0;
int MAXIMO_ENTRENADORES;
int CANTIDAD_EN_DEADLOCK;
bool desalojo_en_ejecucion = false;
bool me_desalojaron = false;

bool hayPokeNuevo = false;
uint32_t quantum_actual = 0;

char** pokes;

bool hayDeadlock = false;
uint32_t es_el_primer_deadlock = true;
uint32_t es_la_primera_vez_localized = true;
uint32_t es_la_primera_vez_caught = true;
uint32_t es_la_primera_vez_appeared = true;
uint32_t es_el_primer_pokemon = true;
uint32_t espera_circular = 0;

t_log* team_logger;
t_log* team_logger_oficial;
t_config* config;

t_list* lista_entrenadores; //lista de entrenadores cargada.
t_list* lista_objetivos; //una lista de objetivos para cada entrenador, esto falta.
t_list* lista_global_objetivos;
t_list* lista_global_pokemones;
t_list* lista_objetivos_de_entrenador;
t_list* lista_pokemones_de_entrenador;
t_list* lista_pokemones_objetivos;
t_list* lista_listos;
t_list* lista_finalizar;
t_list* lista_bloqueados;
t_list* lista_config; //lista auxiliar para cargar la info del archivo de configuracion
t_list* pokemones_ordenada; //lista auxiliar para calcular el objetivo global
t_list* lista_mapa;
t_list* lista_pokemon_atrapado;
t_list* mensajes;
t_list* lista_bloqueados_esperando;
t_list* lista_bloqueados_cant_max_alcanzada;
t_list* lista_bloqueados_deadlock;
t_list* mensajes_que_llegan_nuevos;
t_list* mensajes_para_chequear_id;
t_list* lista_bloqueados_esperando_caught;
t_list * lista_pokemones_objetivos_aux;
t_list * lista_asignados;
t_list * lista_historico_appeared_pokemon;
t_list * mensajes_para_chequear_id_catch;