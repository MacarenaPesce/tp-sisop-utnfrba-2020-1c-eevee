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

void * recibir_appeared_pokemon_desde_broker(t_appeared_pokemon *);
void * recibir_localized_pokemon_desde_broker(t_localized_pokemon *);
void * recibir_caught_pokemon_desde_broker(t_caught_pokemon *);
void enviar_get();
void convertirse_en_suscriptor_global_del_broker();
void hacer_intento_de_reconexion();
void * suscribirse_a_cola(t_suscripcion_a_broker*);


#endif /* INTERACCION_CON_BROKER_H_ */
