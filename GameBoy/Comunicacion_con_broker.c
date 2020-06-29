/*
 * Comunicacion_con_broker.c
 *
 *  Created on: 19 jun. 2020
 *      Author: utnso
 */

#include "Comunicacion_con_broker.h"

void broker_new_pokemon(char * pokemon, char * posx, char * posy, char * cantidad){
	validar_parametros_cuatro_argumentos(pokemon, posx, posy, cantidad);
	log_info(gameboy_logger,"Le voy a mandar a broker los parametros para un nuevo pokemon %s",pokemon);

	t_new_pokemon* new_pokemon = malloc(sizeof(t_new_pokemon));
	new_pokemon->cantidad = (uint32_t)atoi(cantidad);
	new_pokemon->coordenadas.posx = (uint32_t)atoi(posx);
	new_pokemon->coordenadas.posy = (uint32_t)atoi(posy);
	new_pokemon->pokemon = pokemon;

	int socket =  conectar_a_server(ip_broker,puerto_broker);

	if(socket < 0){
		log_info(gameboy_logger,"Broker caído");
	}else{
		int status = distribuir_new_pokemon(socket, -1, -1, (uint32_t)id, new_pokemon);
		if(status < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de new pokemon enviado correctamente");
			close(socket);
		}
	}

	free(new_pokemon);
	terminar_gameboy_correctamente();
}

void broker_appeared_pokemon(char* pokemon, char * posx, char * posy, char * id_mensaje_correlativo){
	validar_parametros_cuatro_argumentos(pokemon, posx, posy, id_mensaje_correlativo);
	log_info(gameboy_logger,"Le voy a mandar a broker los parametros de un appeared pokemon %s",pokemon);

	t_appeared_pokemon* appeared_pokemon = malloc(sizeof(t_appeared_pokemon));
	appeared_pokemon->pokemon = pokemon;
	appeared_pokemon->coordenadas.posx = (uint32_t)atoi(posx);
	appeared_pokemon->coordenadas.posy = (uint32_t)atoi(posy);

	int socket =  conectar_a_server(ip_broker,puerto_broker);

	if(socket < 0){
		log_info(gameboy_logger,"Broker caído");
	}else{
		int status = distribuir_appeared_pokemon(socket, -1, (uint32_t)atoi(id_mensaje_correlativo), (uint32_t)id, appeared_pokemon);
		if(status < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de appeared pokemon enviado correctamente");
			close(socket);
		}
	}

	free(appeared_pokemon);
	terminar_gameboy_correctamente();
}

void broker_catch_pokemon(char * pokemon, char* posx, char* posy){
	validar_parametros_tres_argumentos(pokemon, posx, posy);
	log_info(gameboy_logger,"Le voy a mandar a broker los parametros para un catch pokemon %s",pokemon);

	t_catch_pokemon* appeared_pokemon = malloc(sizeof(t_catch_pokemon));
	appeared_pokemon->pokemon = pokemon;
	appeared_pokemon->coordenadas.posx = (uint32_t)atoi(posx);
	appeared_pokemon->coordenadas.posy = (uint32_t)atoi(posy);

	int socket =  conectar_a_server(ip_broker,puerto_broker);

	if(socket < 0){
		log_info(gameboy_logger,"Broker caído");
	}else{
		int status = distribuir_catch_pokemon(socket, -1, -1, (uint32_t)id, appeared_pokemon);
		if(status < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de catch pokemon enviado correctamente");
			close(socket);
		}
	}

	free(appeared_pokemon);
	terminar_gameboy_correctamente();
}

void broker_caught_pokemon(char * id_mensaje_correlativo, char *ok_or_fail){
	validar_parametros_dos_argumentos(id_mensaje_correlativo, ok_or_fail);
	log_info(gameboy_logger,"Le voy a mandar a broker los parametros para un caught pokemon id mensaje %s, status %s", id_mensaje_correlativo, ok_or_fail);

	t_caught_pokemon* caught_pokemon = malloc(sizeof(t_caught_pokemon));
	caught_pokemon->status = (uint32_t)atoi(ok_or_fail);

	int socket =  conectar_a_server(ip_broker,puerto_broker);

	if(socket < 0){
		log_info(gameboy_logger,"Broker caído");
	}else{
		int status = distribuir_caught_pokemon(socket, -1, (uint32_t)atoi(id_mensaje_correlativo), (uint32_t)id, caught_pokemon);
		if(status < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de caught pokemon enviado correctamente");
			close(socket);
		}
	}

	free(caught_pokemon);
	terminar_gameboy_correctamente();
}

void broker_get_pokemon(char * pokemon){
	if(pokemon==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}else{
		log_info(gameboy_logger," Envio a broker el pokemon %s", pokemon);

		t_get_pokemon * get_pokemon = malloc(sizeof(t_get_pokemon));
		get_pokemon->pokemon = pokemon;

		int socket =  conectar_a_server(ip_broker,puerto_broker);

		if(socket < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			int status = distribuir_get_pokemon(socket, -1, -1, (uint32_t)id, get_pokemon);
			if(status < 0){
				log_info(gameboy_logger,"Broker caído");
			}else{
				log_info(gameboy_logger,"Pedido de get pokemon enviado correctamente");
				close(socket);
			}
		}
		free(get_pokemon);
		terminar_gameboy_correctamente();
	}
}

void mostrar_contenido_del_mensaje(int socket){
	while(1){
	//Recibo ACK
		t_packed * paquete = recibir_mensaje(socket);

		if(paquete != (t_packed*)-1){
			//Quedo a la espera de recibir notificaciones
			if(paquete->operacion == ENVIAR_MENSAJE){
				switch(paquete->cola_de_mensajes){
					case COLA_APPEARED_POKEMON:
						mostrar_appeared_pokemon(paquete->mensaje);
						break;
					case COLA_CAUGHT_POKEMON:
						mostrar_caught_pokemon(paquete->mensaje);
						break;
					case COLA_LOCALIZED_POKEMON:
						mostrar_localized_pokemon(paquete->mensaje);
						break;
					case COLA_GET_POKEMON:
						mostrar_get_pokemon(paquete->mensaje);
						break;
					case COLA_NEW_POKEMON:
						mostrar_new_pokemon(paquete->mensaje);
						break;
					case COLA_CATCH_POKEMON:
						mostrar_catch_pokemon(paquete->mensaje);
						break;
					default:
						break;
				}
			}
			free(paquete);
		}
	}
}

void mostrar_appeared_pokemon(t_appeared_pokemon * pokemon){
	log_info(gameboy_logger,"Me llego un mensaje de appeared pokemon");
	log_info(gameboy_logger,"Pokemon: %s", pokemon->pokemon);
	log_info(gameboy_logger,"Coordenada x: %d", pokemon->coordenadas.posx);
	log_info(gameboy_logger,"Coordenada y: %d", pokemon->coordenadas.posy);
}

void mostrar_caught_pokemon(t_caught_pokemon * pokemon){
	log_info(gameboy_logger,"Me llego un mensaje de caught pokemon");
	log_info(gameboy_logger,"Status: %d", pokemon->status);
}

void mostrar_localized_pokemon(t_localized_pokemon * pokemon){
	log_info(gameboy_logger,"Me llego un mensaje de localized pokemon");
	log_info(gameboy_logger,"Pokemon: %s", pokemon->pokemon);
	log_info(gameboy_logger,"Cantidad de coordenadas: %d", pokemon->cantidad_coordenadas);

	/*t_coordenadas * coord = malloc(sizeof(t_coordenadas));
	for(int i = 0; i < list_size(pokemon->lista_coordenadas); i++){
		coord = list_get(pokemon->lista_coordenadas, i);
		log_info(gameboy_logger,"Par de coordenadas nro %d:", i);
		log_info(gameboy_logger,"Coordenada x: %d", coord->posx);
		log_info(gameboy_logger,"Coordenada y: %d", coord->posy);
	}

	free(coord);*/
}

void mostrar_get_pokemon(t_get_pokemon * pokemon){
	log_info(gameboy_logger,"Me llego un mensaje de get pokemon");
	log_info(gameboy_logger,"Pokemon: %s", pokemon->pokemon);
}

void mostrar_new_pokemon(t_new_pokemon * pokemon){
	log_info(gameboy_logger,"Me llego un mensaje de new pokemon");
	log_info(gameboy_logger,"Pokemon: %s", pokemon->pokemon);
	log_info(gameboy_logger,"Cantidad: %d", pokemon->cantidad);
	log_info(gameboy_logger,"Coordenada x: %d", pokemon->coordenadas.posx);
	log_info(gameboy_logger,"Coordenada y: %d", pokemon->coordenadas.posy);
}

void mostrar_catch_pokemon(t_catch_pokemon * pokemon){
	log_info(gameboy_logger,"Me llego un mensaje de catch pokemon");
	log_info(gameboy_logger,"Pokemon: %s", pokemon->pokemon);
	log_info(gameboy_logger,"Coordenada x: %d", pokemon->coordenadas.posx);
	log_info(gameboy_logger,"Coordenada y: %d", pokemon->coordenadas.posy);
}
/*
void * tiempo_suscripto(t_suscripcion_gameboy * info){
	sleep(atoi(info->tiempo));
	free(info);
	log_info(gameboy_logger,"Se acabo el tiempo de suscripcion");
	terminar_gameboy_correctamente();
	return NULL;
}*/

void consola_suscriptor(char* cola_de_mensajes, char* tiempo){
	/*pthread_t hilo;

	t_servidor * servidor = malloc(sizeof(t_servidor));
	servidor->ip = ip_broker;
	servidor->puerto = puerto_broker;
	servidor->id_cliente = (uint32_t)id;

	t_suscripcion_gameboy * info_para_hilo = malloc(sizeof(t_suscripcion_gameboy));
	info_para_hilo->servidor = servidor;
	info_para_hilo->suscripcion = suscripcion;
	info_para_hilo->tiempo = tiempo;

	pthread_create(&hilo,NULL,(void*)tiempo_suscripto, (void*)info_para_hilo);

	validar_parametros_dos_argumentos(cola_de_mensajes, tiempo);
	log_info(gameboy_logger," Envio a broker la solicitud de suscripcion para la cola %s y estare suscripto por %s segundos",cola_de_mensajes, tiempo);


	if(string_equals_ignore_case(cola_de_mensajes, "COLA_NEW_POKEMON")){
		int solicitud = enviar_solicitud_suscripcion(servidor,COLA_NEW_POKEMON,suscripcion);
		if(solicitud < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de solicitud de suscripcion a la cola NEW_POKEMON enviado correctamente");
			mostrar_contenido_del_mensaje(solicitud);
		}
	}

	if(string_equals_ignore_case(cola_de_mensajes, "COLA_APPEARED_POKEMON")){
		int solicitud = enviar_solicitud_suscripcion(servidor,COLA_APPEARED_POKEMON,suscripcion);
		if(solicitud < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de solicitud de suscripcion a la cola APPEARED_POKEMON enviado correctamente");
			mostrar_contenido_del_mensaje(solicitud);
		}
	}

	if(string_equals_ignore_case(cola_de_mensajes, "COLA_CATCH_POKEMON")){
		int solicitud = enviar_solicitud_suscripcion(servidor,COLA_CATCH_POKEMON,suscripcion);
		if(solicitud < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de solicitud de suscripcion a la cola CATCH_POKEMON enviado correctamente");
			mostrar_contenido_del_mensaje(solicitud);
		}
	}

	if(string_equals_ignore_case(cola_de_mensajes, "COLA_CAUGHT_POKEMON")){
		int solicitud = enviar_solicitud_suscripcion(servidor,COLA_CAUGHT_POKEMON,suscripcion);
		if(solicitud < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de solicitud de suscripcion a la cola CAUGHT_POKEMON enviado correctamente");
			mostrar_contenido_del_mensaje(solicitud);
		}
	}

	if(string_equals_ignore_case(cola_de_mensajes, "COLA_GET_POKEMON")){
		int solicitud = enviar_solicitud_suscripcion(servidor,COLA_GET_POKEMON,suscripcion);
		if(solicitud < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de solicitud de suscripcion a la cola GET_POKEMON enviado correctamente");
			mostrar_contenido_del_mensaje(solicitud);
		}
	}

	if(string_equals_ignore_case(cola_de_mensajes, "COLA_LOCALIZED_POKEMON")){
		int solicitud = enviar_solicitud_suscripcion(servidor,COLA_LOCALIZED_POKEMON,suscripcion);
		if(solicitud < 0){
			log_info(gameboy_logger,"Broker caído");

		}else{
			log_info(gameboy_logger,"Pedido de solicitud de suscripcion a la cola LOCALIZED_POKEMON enviado correctamente");
			mostrar_contenido_del_mensaje(solicitud);
		}
	}*/
}

