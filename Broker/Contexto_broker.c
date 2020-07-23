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
pthread_t hilo_transacciones;
t_list* paquetes_pendientes;
sem_t transaccionar_paquetes_pendientes; 
t_log* broker_logger;
t_config* config;
t_cache_colas* cache_mensajes;
bool debug_broker;
bool warn_broker;
void* memoria_inicial;