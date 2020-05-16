/*
 * Team.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_H_
#define TEAM_H_

#include "Contexto_team.h"
#include "Herramientas_team.h"

void recibir_mensaje_de_texto(int, int);
void recibir_appeared_pokemon_desde_gameboy(int, int);
void iniciar_servidor(void);
void esperar_cliente(int);

#endif /* TEAM_H_ */
