#include "Contexto_broker.h"

/* Global variables*/

int tamanio_memoria;
int tamanio_minimo_particion;
char* algoritmo_memoria;
char* algoritmo_reemplazo;
char* algoritmo_particion_libre;
char* ip_broker;
int frecuencia_compactacion;
char* puerto_broker;
char* log_file;
enum SERVER_STATUS server_status;
pthread_t hilo_sender[COLA_LOCALIZED_POKEMON];

t_log* broker_logger;
t_config* config;
