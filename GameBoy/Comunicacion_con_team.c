/*
 * Comunicacion_con_team.c
 *
 *  Created on: 19 jun. 2020
 *      Author: utnso
 */

#include "Comunicacion_con_team.h"

void team_appeared_pokemon(char* pokemon, char* posx, char* posy){
	validar_parametros_tres_argumentos(pokemon, posx, posy);
	log_info(gameboy_logger,"Le voy a mandar a team las coordenadas del pokemon %s",pokemon);

	t_appeared_pokemon* appeared_pokemon = malloc(sizeof(t_appeared_pokemon));
	appeared_pokemon->pokemon = pokemon;
	appeared_pokemon->coordenadas.posx = (uint32_t)atoi(posx);
	appeared_pokemon->coordenadas.posy = (uint32_t)atoi(posy);

	int socket =  conectar_a_server(ip_team,puerto_team);

	if(socket < 0){
		log_info(gameboy_logger,"TEAM caído");
	}else{
		int status = distribuir_appeared_pokemon(socket, -1, -1, (uint32_t)id, appeared_pokemon);
		if(status < 0){
			log_info(gameboy_logger,"TEAM caído");
		}else{
			log_info(gameboy_logger,"Pedido de appeared pokemon enviado correctamente");
			close(socket);
		}
	}

	free(appeared_pokemon);
	terminar_gameboy_correctamente();
}
