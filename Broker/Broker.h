#ifndef BROKER_H_
#define BROKER_H_

#include "Contexto_broker.h"
#include "Herramientas_broker.h"
#include <Pokebola.h>
#include <semaphore.h>
#include <pthread.h>
#include "ColaMensajes.h"
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

typedef struct{
    uint32_t id_mensaje __attribute__((packed));
    uint32_t id_correlacional __attribute__((packed));
    enum COLA_DE_MENSAJES cola_de_mensajes __attribute__((packed));
    t_list* lista_suscriptores_enviados;
    t_list* lista_suscriptores_ack;
    void* mensaje;
}t_mensaje_cola;

typedef struct{
 t_list* mensajes;
 t_list* colas;
 int proximo_id_mensaje;
}t_cache_colas;

typedef struct{
    int id;
    int cliente;
}t_envio_pendiente;

typedef struct{
    enum COLA_DE_MENSAJES cola_de_mensajes __attribute__((packed));
    t_list* envios_pendientes;
    t_list* suscriptores;
    sem_t* producciones; 
}t_cola_mensajes;

t_cache_colas* cache_mensajes;

t_cola_mensajes* crear_cola_mensajes(int cola_mensajes);
void* sender_suscriptores(void* cola_mensajes);
void eliminar_envio_pendiente(void* pendiente);
void enviar_mensajes_cacheados_a_nuevo_suscriptor(t_cola_mensajes* cola,int cliente);
int enviar_mensaje_a_suscriptor(int id_mensaje,
								int id_correlacional, 
								enum COLA_DE_MENSAJES cola_de_mensajes, 
								int cliente, 
								void* mensaje);
#endif
