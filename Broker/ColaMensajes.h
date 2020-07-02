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
void encolar_envio_a_suscriptores(int cola_de_mensajes,int id_mensaje);

/* Flujos de suscripciones */
void agregar_suscriptor_a_cola(int cola_de_mensajes, uint32_t id_cliente, int socket_cliente);
void enviar_mensajes_cacheados_a_suscriptor(t_cola_mensajes* cola,int cliente);

/* Flujo de ACK */
void agregar_ack_a_mensaje(uint32_t id_mensaje, uint32_t id_cliente, int socket_cliente);

/* Flujo de sender */
void* sender_suscriptores(void* cola_mensajes);
int enviar_mensaje_a_suscriptor(int id_mensaje,
								int id_correlacional, 
								enum COLA_DE_MENSAJES cola_de_mensajes, 
								int cliente, 
								int tamanio_mensaje,
								void* mensaje);

/* Genericas */

// Instanciación
t_cola_mensajes* crear_cola_mensajes(int cola_mensajes);
t_mensaje_cola* crear_mensaje(int cola_de_mensajes, int id_correlacional, uint32_t tamanio_payload, void* mensaje_recibido);
t_cliente* crear_cliente(uint32_t id_cliente, int socket, int cola_de_mensajes);
t_cliente* crear_o_actualizar_cliente(uint32_t id_cliente, int socket, int cola);
t_socket_cliente* crear_socket_cliente(int socket, int cola_de_mensajes);

// Busqueda
t_cola_mensajes* obtener_cola_mensajes(int cola_de_mensajes);
t_mensaje_cola* obtener_mensaje_por_id(int id_mensaje);
t_list* obtener_mensajes_de_cola(t_cola_mensajes* cola);
t_cliente* obtener_cliente_por_id(int id_cliente);
t_list* obtener_memoria_de_cola(t_cola_mensajes* cola);
t_socket_cliente* obtener_socket_cliente_de_cola(t_cliente* cliente, int cola_de_mensajes);

// Validación
bool es_suscriptor_de(int id_cliente, t_cola_mensajes* cola);
bool ack_recibido_de(t_mensaje_cola* mensaje, int id_cliente);
bool es_memoria_de_cola(t_bloque_memoria* bloque, t_cola_mensajes* cola);

// Creación
void agregar_mensaje_a_cache(t_mensaje_cola* mensaje);
void agregar_cliente_a_cache(t_cliente* cliente);
void agregar_cliente_a_suscriptores(t_cola_mensajes* cola, t_cliente* cliente);
void agregar_pendiente_de_envio(t_cola_mensajes* cola, int id_mensaje, int cliente);
void agregar_cliente_a_enviados(t_mensaje_cola* mensaje, t_cliente* cliente);
void agregar_socket_cliente(t_cliente* cliente, int socket, int cola_de_mensajes);

// Actualización
void actualizar_socket_cliente(t_cliente* cliente, int socket, int cola);

// Eliminación
void eliminar_mensaje_enviado();
void eliminar_envio_pendiente(void* pendiente);

// Helpers
void* print_operacion(void* mensaje);
void* print_clientes(void* cliente);

#endif /* COLA_MENSAJES_BROKER_H_ */
