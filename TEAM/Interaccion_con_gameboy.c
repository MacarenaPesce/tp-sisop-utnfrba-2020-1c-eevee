/*
 * Interaccion_con_gameboy.c
 *
 *  Created on: 11 jun. 2020
 *      Author: utnso
 */

#include "Interaccion_con_gameboy.h"

void escuchar_mensajes_entrantes(int new_client_sock){

	log_info(team_logger, "Gameboy conectado, esperando mensajes...");
	t_packed * paquete = recibir_mensaje(new_client_sock);

	recibir_appeared_pokemon_desde_gameboy(paquete->mensaje);

	sem_post(&hay_un_pokemon_nuevo);

	close(new_client_sock);
	free(paquete);
}

void atender_a_gameboy(int serv_socket){
	while(GLOBAL_SEGUIR){
		struct sockaddr_in client_addr;

		//Setea la direccion en 0
		memset(&client_addr, 0, sizeof(client_addr));
		socklen_t client_len = sizeof(client_addr);

		//Acepta la nueva conexion
		int new_client_sock = accept(serv_socket, (struct sockaddr *)&client_addr, &client_len);

		log_info(team_logger, "Se aceptó un nuevo gameboy");

		escuchar_mensajes_entrantes(new_client_sock);
	}
}

void crear_hilo_de_escucha_para_gameboy(int serv_socket){
	pthread_t hilo1;
	pthread_create(&hilo1,NULL,(void*)atender_a_gameboy,serv_socket);
	pthread_join(hilo1, NULL);
}

void recibir_appeared_pokemon_desde_gameboy(t_appeared_pokemon * mensaje){
	log_info(team_logger,"Voy a recibir un pokemon y coordenadas desde gameboy");
	log_info(team_logger,"Me llego este pokemon: %s, coordenada x %d, coordenada y %d", mensaje->pokemon, mensaje->coordenadas.posx, mensaje->coordenadas.posy);

	t_pokemon * pokemon = malloc(sizeof(t_pokemon));
	pokemon->especie = mensaje->pokemon;
	pokemon->posx = mensaje->coordenadas.posx;
	pokemon->posy = mensaje->coordenadas.posy;

	operar_con_appeared_pokemon(pokemon);

}
