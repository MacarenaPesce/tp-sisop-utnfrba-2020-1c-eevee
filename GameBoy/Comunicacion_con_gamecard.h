/*
 * Comunicacion_con_gamecard.h
 *
 *  Created on: 19 jun. 2020
 *      Author: utnso
 */

#ifndef COMUNICACION_CON_GAMECARD_H_
#define COMUNICACION_CON_GAMECARD_H_

#include "Contexto_gameboy.h"
#include "GameBoy.h"

void gamecard_new_pokemon(char*, char*, char*, char*, char*);
void gamecard_catch_pokemon(char*, char*, char*, char*);
void gamecard_get_pokemon(char*, char*);

#endif /* COMUNICACION_CON_GAMECARD_H_ */
