/*
 * Comunicacion_con_broker.h
 *
 *  Created on: 19 jun. 2020
 *      Author: utnso
 */

#ifndef COMUNICACION_CON_BROKER_H_
#define COMUNICACION_CON_BROKER_H_

#include "Contexto_gameboy.h"
#include "GameBoy.h"

void broker_new_pokemon(char *, char *, char *, char *);
void broker_appeared_pokemon(char* pokemon, char *, char *, char *);
void broker_catch_pokemon(char *, char*, char*);
void broker_caught_pokemon(char *, char *);
void broker_get_pokemon(char *);
void mostrar_contenido_del_mensaje(int);
void mostrar_appeared_pokemon(t_appeared_pokemon *);
void mostrar_caught_pokemon(t_caught_pokemon *);
void mostrar_localized_pokemon(t_localized_pokemon *);
void mostrar_get_pokemon(t_get_pokemon *);
void mostrar_new_pokemon(t_new_pokemon *);
void mostrar_catch_pokemon(t_catch_pokemon *);
//void * tiempo_suscripto(t_suscripcion_gameboy *);
void consola_suscriptor(char*, char*);

#endif /* COMUNICACION_CON_BROKER_H_ */
