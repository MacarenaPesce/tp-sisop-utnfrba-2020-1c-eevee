/*
 * mensajesParaFs.c
 *
 *  Created on: 4 jul. 2020
 *      Author: utnso
 */

#include "mensajesParaFs.h"

void operar_con_new_pokemon(t_new_pokemon * poke){

	agregarSemaforoPokemon(poke->pokemon);

	if (existePokemon(poke->pokemon)){

		while (estaAbiertoArchivo(poke->pokemon)){

			sleep(tiempo_reintento_operacion);
			abrirArchivo(poke->pokemon);
		}

		modificarPokemon(poke);
		sleep(tiempo_retardo_operacion);
	}

	else{
		crearPokemon(poke);
		sleep(tiempo_retardo_operacion);
	}

	cerrarArchivo(poke->pokemon);
}


uint32_t operar_con_catch_pokemon(t_catch_pokemon * poke){

	agregarSemaforoPokemon(poke->pokemon);

	if (existePokemon(poke->pokemon)){

		while (estaAbiertoArchivo(poke->pokemon)){

			sleep(tiempo_reintento_operacion);
			abrirArchivo(poke->pokemon);
		}

		capturarPokemon(poke);

	}

	else{

		log_error(gameCard_logger," No existe el pokemon %s, no se puede capturar",poke->pokemon);
		cerrarArchivo(poke->pokemon);
		sleep(tiempo_retardo_operacion);
		return FAIL;
	}
}

t_list* operar_con_get_pokemon(t_get_pokemon* poke){

	agregarSemaforoPokemon(poke->pokemon);

	if (existePokemon(poke->pokemon)){

		while (estaAbiertoArchivo(poke->pokemon)){

			sleep(tiempo_reintento_operacion);
			abrirArchivo(poke->pokemon);
		}

	return obtenerPosicionesPokemon(poke->pokemon);}

return -1;

}

void agregarSemaforoPokemon(char* poke){

	pthread_mutex_lock(&mutexSemPokemones);

	if  (!dictionary_has_key(semaforosPokemon,poke)){

		pthread_mutex_t semPoke;
			dictionary_put(semaforosPokemon,poke,&semPoke);
	}

	pthread_mutex_lock(&mutexSemPokemones);

}

void eliminarSemaforoPokemon(char* poke){

	pthread_mutex_lock(&mutexSemPokemones);

	dictionary_remove(semaforosPokemon,poke);

	 pthread_mutex_unlock(&mutexSemPokemones);
}

