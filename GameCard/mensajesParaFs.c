/*
 * mensajesParaFs.c
 *
 *  Created on: 4 jul. 2020
 *      Author: utnso
 */

void operar_con_new_pokemon(t_new_pokemon* poke){

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


void operar_con_catch_pokemon(t_catch_pokemon* poke){

	if (existePokemon(poke->pokemon)){

		while (estaAbiertoArchivo(poke->pokemon)){

			sleep(tiempo_reintento_operacion);
			abrirArchivo(poke->pokemon);
		}

		capturarPokemon(poke);
		sleep(tiempo_retardo_operacion);

	}

	cerrarArchivo(poke->pokemon);
}
