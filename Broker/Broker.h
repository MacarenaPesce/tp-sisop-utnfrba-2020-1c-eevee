#ifndef BROKER_H_
#define BROKER_H_

#include "Contexto_broker.h"
#include "Herramientas_broker.h"
#include <Pokebola.h>
#include <semaphore.h>
#include <pthread.h>
#include "ColaMensajes.h"
#include "Conexion.h"
//#include "memoria.h"

int espacio_total;
int espacio_vacio;
int q_bloques_ocupados;
int q_bloques_vacios;
float* primer_bloque;
t_list* lista_memoria; 
int socket_servidor;
enum SERVER_STATUS server_status;

static pthread_mutex_t mutex_queue_mensajes = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t mx_serv_socket = PTHREAD_MUTEX_INITIALIZER;

t_cache_colas* cache_mensajes;


#endif
