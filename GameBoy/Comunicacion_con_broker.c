/*
 * Comunicacion_con_broker.c
 *
 *  Created on: 19 jun. 2020
 *      Author: utnso
 */

#include "Comunicacion_con_broker.h"

t_servidor * servidor;

void broker_new_pokemon(char * pokemon, char * posx, char * posy, char * cantidad){
	servidor = (t_servidor*)malloc(sizeof(t_servidor));
	servidor->ip = strdup(ip_broker);
	servidor->puerto = strdup(puerto_broker);
	servidor->id_cliente = (uint32_t)id;

	validar_parametros_cuatro_argumentos(pokemon, posx, posy, cantidad);

	t_new_pokemon* new_pokemon = (t_new_pokemon*)malloc(sizeof(t_new_pokemon));
	new_pokemon->cantidad = (uint32_t)atoi(cantidad);
	new_pokemon->coordenadas.posx = (uint32_t)atoi(posx);
	new_pokemon->coordenadas.posy = (uint32_t)atoi(posy);
	new_pokemon->pokemon = pokemon;

	t_packed* ack = enviar_new_pokemon(servidor,-1,new_pokemon);

	if(ack != (t_packed*)-1){
		log_info(gameboy_logger,"Conectado a Broker");
		log_info(gameboy_logger,"Recibí ACK del mensaje con ID: %d",ack->id_mensaje);
		eliminar_mensaje(ack);
	}else{
		log_info(gameboy_logger,"No pude conectarme al Broker");
	}

	free(new_pokemon);
	terminar_gameboy_correctamente();
}

void broker_appeared_pokemon(char* pokemon, char * posx, char * posy, char * id_mensaje_correlativo){
	servidor = (t_servidor*)malloc(sizeof(t_servidor));
	servidor->ip = strdup(ip_broker);
	servidor->puerto = strdup(puerto_broker);
	servidor->id_cliente = (uint32_t)id;
	validar_parametros_cuatro_argumentos(pokemon, posx, posy, id_mensaje_correlativo);

	t_appeared_pokemon* appeared_pokemon = (t_appeared_pokemon*)malloc(sizeof(t_appeared_pokemon));
	appeared_pokemon->pokemon = pokemon;
	appeared_pokemon->coordenadas.posx = (uint32_t)atoi(posx);
	appeared_pokemon->coordenadas.posy = (uint32_t)atoi(posy);

	t_packed* ack = enviar_appeared_pokemon(servidor, -1, appeared_pokemon);

	if(ack != (t_packed*)-1){
		log_info(gameboy_logger,"Conectado a Broker");
		log_info(gameboy_logger,"Recibí ACK del mensaje con ID: %d",ack->id_mensaje);
		eliminar_mensaje(ack);
	}else{
		log_info(gameboy_logger,"No pude conectarme al Broker");
	}

	free(appeared_pokemon);
	terminar_gameboy_correctamente();
}

void broker_catch_pokemon(char * pokemon, char* posx, char* posy){
	servidor = (t_servidor*)malloc(sizeof(t_servidor));
	servidor->ip = strdup(ip_broker);
	servidor->puerto = strdup(puerto_broker);
	servidor->id_cliente = (uint32_t)id;
	validar_parametros_tres_argumentos(pokemon, posx, posy);

	t_catch_pokemon* catch_pokemon = (t_catch_pokemon*)malloc(sizeof(t_catch_pokemon));
	catch_pokemon->pokemon = pokemon;
	catch_pokemon->coordenadas.posx = (uint32_t)atoi(posx);
	catch_pokemon->coordenadas.posy = (uint32_t)atoi(posy);

	t_packed* ack = enviar_catch_pokemon(servidor, -1, catch_pokemon);

	if(ack != (t_packed*)-1){
		log_info(gameboy_logger,"Conectado a Broker");
		log_info(gameboy_logger,"Recibí ACK del mensaje con ID: %d",ack->id_mensaje);
		eliminar_mensaje(ack);
	}else{
		log_info(gameboy_logger,"No pude conectarme al Broker");
	}

	free(catch_pokemon);
	terminar_gameboy_correctamente();
}

void broker_caught_pokemon(char * id_mensaje_correlativo, char *ok_or_fail){
	servidor = (t_servidor*)malloc(sizeof(t_servidor));
	servidor->ip = strdup(ip_broker);
	servidor->puerto = strdup(puerto_broker);
	servidor->id_cliente = (uint32_t)id;
	validar_parametros_dos_argumentos(id_mensaje_correlativo, ok_or_fail);

	t_caught_pokemon* caught_pokemon = (t_caught_pokemon*)malloc(sizeof(t_caught_pokemon));
	caught_pokemon->status = (uint32_t)atoi(ok_or_fail);

	t_packed* ack = enviar_caught_pokemon(servidor, -1, caught_pokemon);

	if(ack != (t_packed*)-1){
		log_info(gameboy_logger,"Conectado a Broker");
		log_info(gameboy_logger,"Recibí ACK del mensaje con ID: %d",ack->id_mensaje);
		eliminar_mensaje(ack);
	}else{
		log_info(gameboy_logger,"No pude conectarme al Broker");
	}

	free(caught_pokemon);
	terminar_gameboy_correctamente();
}

void broker_get_pokemon(char * pokemon){
	servidor = (t_servidor*)malloc(sizeof(t_servidor));
	servidor->ip = strdup(ip_broker);
	servidor->puerto = strdup(puerto_broker);
	servidor->id_cliente = (uint32_t)id;
	if(pokemon==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}else{
		t_get_pokemon * get_pokemon = (t_get_pokemon*)malloc(sizeof(t_get_pokemon));
		get_pokemon->pokemon = pokemon;

		t_packed* ack = enviar_get_pokemon(servidor, -1, get_pokemon);

		if(ack != (t_packed*)-1){
			log_info(gameboy_logger,"Conectado a Broker");
			log_info(gameboy_logger,"Recibí ACK del mensaje con ID: %d",ack->id_mensaje);
			eliminar_mensaje(ack);
		}else{
			log_info(gameboy_logger,"No pude conectarme al Broker");
		}

		free(get_pokemon);
		terminar_gameboy_correctamente();
	}
}

void* mostrar_contenido_del_mensaje(void* _socket){

	int socket = *((int*)_socket);
	bool socket_finalizado = false;

	while(1){
	//Recibo ACK
		t_packed * paquete = recibir_mensaje(socket);

		if(paquete == (t_packed*)-2){
			socket_finalizado = true;
			//El socket finalizó, esta suscripcion no sirve mas
			break;
		}

		if(paquete != (t_packed*)-1){ 
			//Quedo a la espera de recibir notificaciones
			switch(paquete->operacion){

				case ENVIAR_MENSAJE:
					log_info(gameboy_logger,"Recibido mensaje de id %d",paquete->id_mensaje);
					recibir_mensaje_de_broker(paquete);
					enviar_ack(servidor,paquete->id_mensaje);
					eliminar_mensaje(paquete);
					break;

				case ACK:

					if(paquete->id_mensaje == -1){
						log_info(gameboy_logger,"Recibido ack de suscripcion");
						eliminar_mensaje(paquete);
						break;
					}

					log_info(gameboy_logger,"Recibido ack de mensaje de id %d",paquete->id_mensaje);
					eliminar_mensaje(paquete);

					break;
			}
		}
	}

	free(_socket);

	return NULL;
}

void recibir_mensaje_de_broker(t_packed* paquete){

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

void * tiempo_suscripto(t_suscripcion_gameboy * info){
	sleep(atoi(info->tiempo));
	free(info);
	log_info(gameboy_logger,"Se acabo el tiempo de suscripcion");
	terminar_gameboy_correctamente();
	return NULL;
}

void consola_suscriptor(char* cola_de_mensajes, char* tiempo){

	servidor = (t_servidor*)malloc(sizeof(t_servidor));
	servidor->ip = strdup(ip_broker);
	servidor->puerto = strdup(puerto_broker);
	servidor->id_cliente = (uint32_t)id;

	t_suscripcion_gameboy * info_para_hilo = malloc(sizeof(t_suscripcion_gameboy));
	info_para_hilo->servidor = servidor;
	info_para_hilo->tiempo = tiempo;

	validar_parametros_dos_argumentos(cola_de_mensajes, tiempo);
	log_info(gameboy_logger," Envio a broker la solicitud de suscripcion para la cola %s y estare suscripto por %s segundos",cola_de_mensajes, tiempo);

	suscribirse_a_cola(cola_de_mensajes);

	tiempo_suscripto(info_para_hilo);

}

void* suscribirse_a_cola(char* cola_de_mensajes){

	pthread_t hilo;

	int socket = 0;

	if(string_equals_ignore_case(cola_de_mensajes, "NEW_POKEMON")){
		socket = enviar_solicitud_suscripcion(servidor,COLA_NEW_POKEMON);	
	}

	if(string_equals_ignore_case(cola_de_mensajes, "APPEARED_POKEMON")){
		socket = enviar_solicitud_suscripcion(servidor,COLA_APPEARED_POKEMON);
	}

	if(string_equals_ignore_case(cola_de_mensajes, "CATCH_POKEMON")){
		socket = enviar_solicitud_suscripcion(servidor,COLA_CATCH_POKEMON);
	}

	if(string_equals_ignore_case(cola_de_mensajes, "CAUGHT_POKEMON")){
		socket = enviar_solicitud_suscripcion(servidor,COLA_CAUGHT_POKEMON);
	}

	if(string_equals_ignore_case(cola_de_mensajes, "GET_POKEMON")){
		socket = enviar_solicitud_suscripcion(servidor,COLA_GET_POKEMON);
	}

	if(string_equals_ignore_case(cola_de_mensajes, "LOCALIZED_POKEMON")){
		socket = enviar_solicitud_suscripcion(servidor,COLA_LOCALIZED_POKEMON);
	}

	if(socket <= 0){
		log_info(gameboy_logger,"Broker caído");
		return;
	}

	void* socket_server = (int*)malloc(sizeof(int));
	memcpy(socket_server,&socket,sizeof(int));

	pthread_create(&hilo, NULL, mostrar_contenido_del_mensaje, socket_server);

	pthread_detach(hilo);

}