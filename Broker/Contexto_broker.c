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

t_log* broker_logger;
t_config* config;
