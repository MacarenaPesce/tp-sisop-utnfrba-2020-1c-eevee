#ifndef COLA_MENSAJES_BROKER_H_
#define COLA_MENSAJES_BROKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include <Pokebola.h>
#include <pthread.h>
#include <semaphore.h>
#include "Broker.h"

/* Flujos de mensajes */
int agregar_mensaje_a_cola(t_packed* paquete);
void agregar_suscriptor_a_cola(int cola_de_mensajes, int cliente);

/* Genericas */
void distribuir_mensaje_a_suscriptores(int cola_de_mensajes,int id_mensaje);
t_cola_mensajes* obtener_cola_mensajes(int cola_de_mensajes);
void agregar_pendiente_de_envio(t_cola_mensajes* cola, int id_mensaje, int cliente);
t_list* obtener_mensajes_de_cola(t_cola_mensajes* cola);

void* print_operacion(void* mensaje);

t_cola_mensajes* crear_cola_mensajes(int cola_mensajes);
void* sender_suscriptores(void* cola_mensajes);
void eliminar_envio_pendiente(void* pendiente);
void enviar_mensajes_cacheados_a_nuevo_suscriptor(t_cola_mensajes* cola,int cliente);
int enviar_mensaje_a_suscriptor(int id_mensaje,
								int id_correlacional, 
								enum COLA_DE_MENSAJES cola_de_mensajes, 
								int cliente, 
								void* mensaje);

#endif /* COLA_MENSAJES_BROKER_H_ */
