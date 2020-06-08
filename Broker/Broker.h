#ifndef BROKER_H_
#define BROKER_H_

#include "Memoria.h"
#include "Herramientas_broker.h"
#include "Contexto_broker.h"
//#include "Broker.h"
#include "utils.h"
#include <Pokebola.h>


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

typedef struct{
    uint32_t id;
    uint32_t tipo_mensaje;
    uint32_t id_correlacion;
    t_list* lista_suscriptores_enviados;
    t_list* lista_suscriptores_ack;
}t_queue_mensaje;


#endif
