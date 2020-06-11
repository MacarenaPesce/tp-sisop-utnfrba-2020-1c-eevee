/*
 * Interaccion_con_gameboy.h
 *
 *  Created on: 11 jun. 2020
 *      Author: utnso
 */

#ifndef INTERACCION_CON_GAMEBOY_H_
#define INTERACCION_CON_GAMEBOY_H_

#include "Contexto_team.h"

void * escuchar_mensajes_entrantes(int);
void crear_hilo_de_escucha_para_gameboy(int, void*funcion_a_ejecutar(int));
void recibir_appeared_pokemon_desde_gameboy(t_appeared_pokemon *);


#endif /* INTERACCION_CON_GAMEBOY_H_ */
