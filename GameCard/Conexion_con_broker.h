/*
 * Conexion_con_broker.h
 *
 *  Created on: 2 jul. 2020
 *      Author: utnso
 */

#ifndef CONEXION_CON_BROKER_H_
#define CONEXION_CON_BROKER_H_

#include "Contexto_gameCard.h"
#include "Herramientas_gameCard.h"
#include "mensajesParaFs.h"
#include <pthread.h>

t_packed * paquete;
void * atender_get_pokemon(t_packed *);
void * atender_catch_pokemon(t_packed *);
void * atender_new_pokemon(t_packed *);
void * recibir_new_pokemon_desde_broker(t_packed *);
void * recibir_catch_pokemon_desde_broker(t_packed *);
void * recibir_get_pokemon_desde_broker(t_packed *);
void convertirse_en_suscriptor_global_del_broker();
void hacer_intento_de_reconexion();
void * suscribirse_a_cola(t_suscripcion_a_broker *);

#endif /* CONEXION_CON_BROKER_H_ */
