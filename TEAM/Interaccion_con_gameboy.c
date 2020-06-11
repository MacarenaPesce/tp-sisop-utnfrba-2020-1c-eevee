/*
 * Interaccion_con_gameboy.c
 *
 *  Created on: 11 jun. 2020
 *      Author: utnso
 */

#include "Interaccion_con_gameboy.h"

void * escuchar_mensajes_entrantes(int new_client_sock){

	log_info(team_logger, "Gameboy conectado, esperando mensajes...");

	t_packed * paquete = recibir_mensaje(new_client_sock);

	if(paquete == 0) {
	   log_info(team_logger, "\nDesconectado \n");
	}

	if(paquete->cola_de_mensajes == COLA_APPEARED_POKEMON){
		recibir_appeared_pokemon_desde_gameboy(paquete->mensaje);
	}

		close(new_client_sock);
		free(paquete);
}

void crear_hilo_de_escucha_para_gameboy(int socket, void*funcion_a_ejecutar(int)){
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)funcion_a_ejecutar,(void*)socket);
	pthread_detach(hilo);
}

void recibir_appeared_pokemon_desde_gameboy(t_appeared_pokemon * mensaje){
	log_info(team_logger,"Voy a recibir un pokemon y coordenadas desde gameboy");

	log_info(team_logger,"Me llego este pokemon: %s", mensaje->pokemon);
	log_info(team_logger,"La coordenada X es: %d", mensaje->coordenadas.posx);
	log_info(team_logger,"La coordenada Y es: %d\n", mensaje->coordenadas.posy);
	operar_con_appeared_pokemon(mensaje);

	free(mensaje);
}
