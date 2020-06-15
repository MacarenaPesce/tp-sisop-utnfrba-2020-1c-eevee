#ifndef HILOS_BROKER_H_
#define HILOS_BROKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include <Pokebola.h>
#include <semaphore.h>
#include "ColaMensajes.h"

void recibir_mensaje_de_texto(int, int);
void iniciar_servidor(void);
void* esperar_cliente(void* socket_servidor);
void* ObtenerMensaje(void* cliente);
void* esperar_mensajes(void* cliente);

void recibir_mensaje_de_colas(t_packed* paquete,int socket_cliente);
void recibir_solicitud_suscripcion(t_packed *paquete,int socket_cliente);
void recibir_ack(t_packed *paquete,int socket_cliente);
int agregar_mensaje_a_cola(t_packed* paquete);
void agregar_suscriptor_a_cola(int cola_de_mensajes, int cliente);


#endif /* HILOS_BROKER_H_ */
