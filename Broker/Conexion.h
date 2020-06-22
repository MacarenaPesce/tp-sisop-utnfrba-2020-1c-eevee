#ifndef HILOS_BROKER_H_
#define HILOS_BROKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include <Pokebola.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include "Broker.h"


/* Funciones de servidor */
void iniciar_servidor(void);
void* esperar_cliente(void* socket_servidor);
void* esperar_mensajes(void* cliente);

/* Funciones de recepcion */
void recibir_mensaje_de_colas(t_packed* paquete,int socket_cliente);
void recibir_solicitud_suscripcion(t_packed *paquete,int socket_cliente);
void recibir_ack(t_packed *paquete,int socket_cliente);

#endif /* HILOS_BROKER_H_ */
