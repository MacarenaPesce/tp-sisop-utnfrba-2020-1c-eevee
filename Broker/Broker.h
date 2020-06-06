#ifndef BROKER_H_
#define BROKER_H_

#include "Contexto_broker.h"
#include "Herramientas_broker.h"
#include <Pokebola.h>
//#include "memoria.h"

int espacio_total;
int espacio_vacio;
int q_bloques_ocupados;
int q_bloques_vacios;
float* primer_bloque;
t_list* lista_memoria;

typedef struct{
    uint32_t id;
    uint32_t tipo_mensaje;
    uint32_t id_correlacion;
    t_list* lista_suscriptores_enviados;
    t_list* lista_suscriptores_ack;
}t_mensaje;


#endif
