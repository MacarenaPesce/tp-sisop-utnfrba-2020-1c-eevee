/*
 * Herramientas_team.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef HERRAMIENTAS_GAMECARD_H_
#define HERRAMIENTAS_GAMECARD_H_

#include "Contexto_gameCard.h"

void inicializar_logger();
void inicializar_archivo_de_configuracion();
void obtener_valor_config(char* key, t_config* file, void(*obtener)(void));

#endif /* HERRAMIENTAS_GAMECARD_H_ */
