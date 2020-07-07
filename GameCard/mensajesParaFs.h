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

pthread_mutex_t mutexSemPokemones=PTHREAD_MUTEX_INITIALIZER;
t_dictionary* semaforosPokemones;;

void operar_con_new_pokemon(t_new_pokemon* poke);
void operar_con_catch_pokemon(t_catch_pokemon* poke);
void operar_con_get_pokemon(t_get_pokemon* poke);

#endif /* MENSAJESPARAFS_H_ */
