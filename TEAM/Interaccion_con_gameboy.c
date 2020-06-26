/*
 * Interaccion_con_gameboy.c
 *
 *  Created on: 11 jun. 2020
 *      Author: utnso
 */

#include "Interaccion_con_gameboy.h"

void escuchar_mensajes_entrantes(int new_client_sock){

	log_info(team_logger, "Gameboy conectado, esperando mensajes...");

	while(1){
		t_packed * paquete = recibir_mensaje(new_client_sock);
		if(paquete != (t_packed*)-1){

			pthread_mutex_lock(&llego_gameboy);
			recibir_appeared_pokemon_desde_gameboy(paquete);
			pthread_mutex_unlock(&llego_gameboy);

			close(new_client_sock);
			break;
		}
	}
}

void * atender_a_gameboy(void * serv_socket){

	while(GLOBAL_SEGUIR){
		struct sockaddr_in client_addr;

		//Setea la direccion en 0
		memset(&client_addr, 0, sizeof(client_addr));
		socklen_t client_len = sizeof(client_addr);

		//Acepta la nueva conexion
		int new_client_sock = accept(*(int*)serv_socket, (struct sockaddr *)&client_addr, &client_len);
		printf("\n");
		log_debug(team_logger, "Se aceptó un nuevo gameboy");

		escuchar_mensajes_entrantes(new_client_sock);
	}
	return NULL;
}

void crear_hilo_de_escucha_para_gameboy(int serv_socket){
	pthread_t hilo1;
	pthread_create(&hilo1,NULL, atender_a_gameboy, (void*)&serv_socket);
	pthread_join(hilo1, NULL);
}

void recibir_appeared_pokemon_desde_gameboy(t_packed * paquete){
	t_appeared_pokemon * appeared = paquete->mensaje;

	t_mensaje_guardado * mensaje = malloc(sizeof(t_mensaje_guardado));
	mensaje->id = paquete->id_mensaje;
	mensaje->id_correlacional = paquete->id_correlacional;
	mensaje->operacion = APPEARED;
	mensaje->contenido = appeared;


	pthread_mutex_lock(&mensaje_nuevo_mutex);
	list_add(mensajes_que_llegan_nuevos, mensaje);
	pthread_mutex_unlock(&mensaje_nuevo_mutex);

	sem_post(&mensaje_nuevo_disponible);

	free(paquete);
}