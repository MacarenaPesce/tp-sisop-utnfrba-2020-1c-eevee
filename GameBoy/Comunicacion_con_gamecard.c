/*
 * Comunicacion_con_gamecard.c
 *
 *  Created on: 19 jun. 2020
 *      Author: utnso
 */

#include "Comunicacion_con_gamecard.h"

void gamecard_new_pokemon(char * pokemon, char * posx, char * posy, char * cantidad, char * id_mensaje_correlativo){

	validar_parametros_cinco_argumentos(pokemon,posx,posy,cantidad,id_mensaje_correlativo);
	log_info(gameboy_logger," Envio a gamecard la posicion en x -> %s, la posicion en y -> %s, con la cantidad %s del pokemon %s a crear en el mapa y id_correlativo %s",
			posx,posy,cantidad,pokemon,id_mensaje_correlativo);

	t_new_pokemon* new_pokemon = malloc(sizeof(t_new_pokemon));
	new_pokemon->cantidad = (uint32_t)atoi(cantidad);
	new_pokemon->coordenadas.posx = (uint32_t)atoi(posx);
	new_pokemon->coordenadas.posy = (uint32_t)atoi(posy);
	new_pokemon->pokemon = pokemon;

	int socket =  conectar_a_server(ip_gamecard,puerto_gamecard);

	if(socket < 0){
		log_info(gameboy_logger,"Gamecard caído");
	}else{
		int status = distribuir_new_pokemon(socket, -1, (uint32_t)atoi(id_mensaje_correlativo), (uint32_t)id, new_pokemon);
		if(status < 0){
			log_info(gameboy_logger,"Gamecard caído");
		}else{
			log_info(gameboy_logger,"Pedido de new pokemon enviado correctamente");
			close(socket);
		}
	}

	free(new_pokemon);
	terminar_gameboy_correctamente();
}

void gamecard_catch_pokemon(char * pokemon, char* posx, char* posy, char * id_mensaje){

	validar_parametros_cuatro_argumentos(pokemon,posx,posy,id_mensaje);
	log_info(gameboy_logger," Envio a gamecard la posicion en x -> %s, la posicion en y -> %s, del pokemon %s a crear en el mapa, y id_mensaje %s",posx,posy,pokemon,id_mensaje);

	t_catch_pokemon* appeared_pokemon = malloc(sizeof(t_catch_pokemon));
	appeared_pokemon->pokemon = pokemon;
	appeared_pokemon->coordenadas.posx = (uint32_t)atoi(posx);
	appeared_pokemon->coordenadas.posy = (uint32_t)atoi(posy);

	int socket =  conectar_a_server(ip_gamecard,puerto_gamecard);

	if(socket < 0){
		log_info(gameboy_logger,"Gamecard caído");
	}else{
		int status = distribuir_catch_pokemon(socket, (uint32_t)atoi(id_mensaje), -1, (uint32_t)id, appeared_pokemon);
		if(status < 0){
			log_info(gameboy_logger,"Gamecard caído");
		}else{
			log_info(gameboy_logger,"Pedido de appeared pokemon enviado correctamente");
			close(socket);
		}
	}

	free(appeared_pokemon);;
	terminar_gameboy_correctamente();
}

void gamecard_get_pokemon(char * pokemon, char * id_mensaje){
	validar_parametros_dos_argumentos(pokemon, id_mensaje);
	log_info(gameboy_logger," Envio a gamecard el pokemon %s y id_mensaje %s", pokemon, id_mensaje);

	t_get_pokemon * get_pokemon = malloc(sizeof(t_get_pokemon));
	get_pokemon->pokemon = pokemon;

	int socket =  conectar_a_server(ip_gamecard,puerto_gamecard);

	if(socket < 0){
		log_info(gameboy_logger,"Gamecard caído");
	}else{
		int status = distribuir_get_pokemon(socket, (uint32_t)atoi(id_mensaje), -1, (uint32_t)id, get_pokemon);
		if(status < 0){
			log_info(gameboy_logger,"Gamecard caído");
		}else{
			log_info(gameboy_logger,"Pedido de get pokemon enviado correctamente");
			close(socket);
		}
	}

	free(get_pokemon);
	terminar_gameboy_correctamente();
}
