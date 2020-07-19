#ifndef BROKER_H_
#define BROKER_H_


#include "Contexto_broker.h"
#include "Herramientas_broker.h"
#include <Pokebola.h>
#include <semaphore.h>
#include <pthread.h>
#include "ColaMensajes.h"
#include "Conexion.h"
#include "Memoria.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <stdarg.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include<stdint.h>

#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>
int espacio_total;
int espacio_vacio;
int q_bloques_ocupados;
int q_bloques_vacios;
float* primer_bloque;
t_list* lista_memoria; 
int socket_servidor;
enum SERVER_STATUS server_status;
bool* debug_broker;

static pthread_mutex_t mutex_queue_mensajes = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_recibir_mensajes = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_server_status = PTHREAD_MUTEX_INITIALIZER;

t_cache_colas* cache_mensajes;

#endif
