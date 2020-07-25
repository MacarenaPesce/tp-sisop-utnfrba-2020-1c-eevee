/*
 * Conexion_con_gameboy.c
 *
 *  Created on: 2 jul. 2020
 *      Author: utnso
 */

#include "Conexion_con_gameboy.h"

void escuchar_mensajes_entrantes(int new_client_sock){
	/* Gameboy le enviará mensajes al Game Card como si los mismos vinieran desde las colas del Broker.
	 * Por lo que desde acá llamo a las funciones que se usan en la conexión con el Broker*/

	log_info(gameCard_logger, "Gameboy conectado, esperando mensajes...");

	while(1){
		paquete = recibir_mensaje(new_client_sock);
		if(paquete != (t_packed*)-1){

			switch(paquete->cola_de_mensajes){
				case COLA_NEW_POKEMON:
					log_info(gameCard_logger,"recibo mensaje de la cola NEW_POKEMON");
					recibir_new_pokemon_desde_broker(paquete);
					eliminar_mensaje(paquete);
					break;
				case COLA_CATCH_POKEMON:
					log_info(gameCard_logger,"recibo mensaje de la cola CATCH_POKEMON");
					recibir_catch_pokemon_desde_broker(paquete);
					eliminar_mensaje(paquete);
					break;
				case COLA_GET_POKEMON:
					log_info(gameCard_logger,"recibo mensaje de la cola GET_POKEMON");
					recibir_get_pokemon_desde_broker(paquete);
					eliminar_mensaje(paquete);
					break;
				default:
					log_error(gameCard_logger, "RECIBI UN MENSAJE DESDE UNA COLA INVALIDA");
					log_error(gameCard_logger, "COLA DE MENSAJES:%d", paquete->cola_de_mensajes);
					eliminar_mensaje(paquete);
					break;
			}
		}
			else {

				if(paquete->operacion == ACK){
					eliminar_mensaje(paquete);
							}
						}

			close(new_client_sock);
			break;
		}


	}


void * atender_a_gameboy(void * serv_socket){

	while(1){
		struct sockaddr_in client_addr;

		//Setea la direccion en 0
		memset(&client_addr, 0, sizeof(client_addr));
		socklen_t client_len = sizeof(client_addr);

		//Acepta la nueva conexion
		int new_client_sock = accept(*(int*)serv_socket, (struct sockaddr *)&client_addr, &client_len);
		printf("\n");
		log_debug(gameCard_logger, "Se aceptó un nuevo gameboy");

		escuchar_mensajes_entrantes(new_client_sock);
	}
	return NULL;
}

void crear_hilo_de_escucha_para_gameboy(int serv_socket){
	pthread_t hilo1;
	pthread_create(&hilo1,NULL, atender_a_gameboy, (void*)&serv_socket);
	pthread_join(hilo1, NULL);
}
