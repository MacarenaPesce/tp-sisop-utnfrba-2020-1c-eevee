/*
 * mensajesParaFs.c
 *
 *  Created on: 4 jul. 2020
 *      Author: utnso
 */

#include "mensajesParaFs.h"

void operar_con_new_pokemon(t_new_pokemon * poke){


	if (existePokemon(poke->pokemon)){

		agregarSemaforoPokemon(poke->pokemon);

		log_info(gameCard_logger,"existe el pokemon %s", poke->pokemon);

		while (estaAbiertoArchivo(poke->pokemon)){

			log_info(gameCard_logger,"el archivo se encuentra abierto por otro hilo o proceso");
			log_info(gameCard_logger,"no se lo puede abrir");
			log_info(gameCard_logger,"se reintenta abrir "
					"el archivo en %d segundos",tiempo_reintento_operacion);
			sleep(tiempo_reintento_operacion);
			abrirArchivo(poke->pokemon);
		}

		if(estadoArchivo!=NULL){ free(estadoArchivo);}
		modificarPokemon(poke);
		log_info(gameCard_logger,"el tiempo de acceso a disco "
				"es : %d", tiempo_retardo_operacion);
		sleep(tiempo_retardo_operacion);
		cerrarArchivo(poke->pokemon);
	}

	else{

		log_info(gameCard_logger,"el pokemon %s no existe, "
				"hay que crearlo",poke->pokemon);
		crearPokemon(poke);
		agregarSemaforoPokemon(poke->pokemon);
		log_info(gameCard_logger,"el tiempo de acceso a disco "
				"es : %d", tiempo_retardo_operacion);
		sleep(tiempo_retardo_operacion);
	}

}


uint32_t operar_con_catch_pokemon(t_catch_pokemon * poke){

	if (existePokemon(poke->pokemon)){

		agregarSemaforoPokemon(poke->pokemon);

		log_info(gameCard_logger,"existe el pokemon %s", poke->pokemon);

		while (estaAbiertoArchivo(poke->pokemon)){

			log_info(gameCard_logger,"el archivo se encuentra abierto "
					"por otro hilo o proceso");
			log_info(gameCard_logger,"no se lo puede abrir");
			log_info(gameCard_logger,"se reintenta abrir "
								"el archivo en %d segundos",tiempo_reintento_operacion);

			sleep(tiempo_reintento_operacion);
			abrirArchivo(poke->pokemon);
		}

		if(estadoArchivo!=NULL){ free(estadoArchivo);}
		return capturarPokemon(poke);

	}

	else{

		log_error(gameCard_logger," No existe el pokemon %s, "
				"no se puede capturar",poke->pokemon);
		sleep(tiempo_retardo_operacion);
		return FAIL;
	}
}

t_list* operar_con_get_pokemon(t_get_pokemon* poke){


	if (existePokemon(poke->pokemon)){

		log_info(gameCard_logger,"existe el pokemon %s", poke->pokemon);

		agregarSemaforoPokemon(poke->pokemon);

		while (estaAbiertoArchivo(poke->pokemon)){

			log_info(gameCard_logger,"el archivo se encuentra abierto "
								"por otro hilo o proceso");
			log_info(gameCard_logger,"no se lo puede abrir");
			log_info(gameCard_logger,"se reintenta abrir "
											"el archivo en %d segundos",tiempo_reintento_operacion);

			sleep(tiempo_reintento_operacion);
			abrirArchivo(poke->pokemon);
		}

		if(estadoArchivo!=NULL){
			free(estadoArchivo);
		}
		return obtenerPosicionesPokemon(poke->pokemon);

	}

else{
	log_error(gameCard_logger," No existe el pokemon %s, no posee posiciones",poke->pokemon);

log_info(gameCard_logger, "te voy a mandar una lista vacia");

t_list* listaVacia;
listaVacia=list_create();

	return listaVacia;	}


}


