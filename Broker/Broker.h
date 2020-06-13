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
t_list* cola_mensajes;
static pthread_mutex_t mutex_queue_mensajes = PTHREAD_MUTEX_INITIALIZER;

typedef struct{
    uint32_t id_correlacional __attribute__((packed));
    enum COLA_DE_MENSAJES cola_de_mensajes __attribute__((packed));
    t_list* lista_suscriptores_enviados;
    t_list* lista_suscriptores_ack;
    void* mensaje;
}t_mensaje_cola;


#endif
