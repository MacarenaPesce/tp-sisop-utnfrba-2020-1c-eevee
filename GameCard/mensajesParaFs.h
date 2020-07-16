/*
 * mensajesParaFs.h
 *
 *  Created on: 4 jul. 2020
 *      Author: utnso
 */

#ifndef MENSAJESPARAFS_H_
#define MENSAJESPARAFS_H_

#include "FileSystem.h"
#include <stdbool.h>
#include "Pokebola.h"
#include <pthread.h>
#include "commons/collections/dictionary.h"

void operar_con_new_pokemon(t_new_pokemon* poke);
uint32_t operar_con_catch_pokemon(t_catch_pokemon* poke);
t_list* operar_con_get_pokemon(t_get_pokemon* poke);
#endif /* MENSAJESPARAFS_H_ */
