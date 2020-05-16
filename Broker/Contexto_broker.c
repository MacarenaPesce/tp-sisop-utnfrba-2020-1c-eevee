#include "Contexto_broker.h"

/* Global variables*/
char** posiciones_entrenadores;
char** pokemon_entrenadores;
char** objetivos_entrenadores;
int tiempo_reconexion;
int retardo_ciclo_cpu;
char* algoritmo_planificacion;
int quantum;
char* ip_broker;
int estimacion_inicial;
char* puerto_broker;
char* log_file;

t_log* broker_logger;
t_config* config;
