/*
 * Interaccion_con_broker.h
 *
 *  Created on: 11 jun. 2020
 *      Author: utnso
 */

#ifndef INTERACCION_CON_BROKER_H_
#define INTERACCION_CON_BROKER_H_

#include "Contexto_team.h"
#include "Team.h"

void enviar_get();
void enviar_mensaje_por_cada_pokemon_requerido(int);
void convertirse_en_suscriptor_global_del_broker();
void hacer_intento_de_reconexion();
void * enviar_mensaje_de_suscripcion_a_cola_localized_pokemon();
void * enviar_mensaje_de_suscripcion_a_cola_appeared_pokemon();
void * enviar_mensaje_de_suscripcion_a_cola_caught_pokemon();


#endif /* INTERACCION_CON_BROKER_H_ */
