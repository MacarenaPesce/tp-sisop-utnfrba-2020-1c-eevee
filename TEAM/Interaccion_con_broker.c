/*
 * Interaccion_con_broker.c
 *
 *  Created on: 11 jun. 2020
 *      Author: utnso
 */

#include "Interaccion_con_broker.h"

void enviar_get(){
	int broker_socket = conectar_a_server(ip_broker, "6009");
	if(broker_socket < 0){// || broker_socket == 0){
		log_info(team_logger_oficial, "Falló la conexión con Broker; inicia la operación default");
		int h = 0;
		t_objetivo* objetivo = malloc(sizeof(t_objetivo));
		while(lista_objetivos != NULL){
			objetivo = list_get(lista_objetivos, h);
			if(objetivo == NULL){
				break;
			}
			log_info(team_logger, "No existen locaciones para esta especie: %s, cant %i", objetivo->especie, objetivo->cantidad_necesitada);
			h++;


		}free(objetivo); //si pones este free adentro del ciclo, rompe con gameboy.
	}else{
		enviar_mensaje_por_cada_pokemon_requerido(broker_socket);
		close(broker_socket);
	}
}

void enviar_mensaje_por_cada_pokemon_requerido(int broker_socket){

	int h = 0;
	t_objetivo * objetivo;
	while(lista_objetivos != NULL){
		objetivo = list_get(lista_objetivos, h);
		if(objetivo == NULL){
			break;
		}

		t_get_pokemon* get_pokemon = malloc(sizeof(t_get_pokemon));
		get_pokemon->pokemon = objetivo->especie;

		enviar_get_pokemon(broker_socket, -1, -1, get_pokemon);

		log_info(team_logger, "Enviado pedido de get pokemon para esta especie: %s", objetivo->especie);
		free(get_pokemon);
		h++;

		//Recibo ACK
		t_packed * paquete1 = recibir_mensaje(broker_socket);

		if(paquete1->operacion == ACK){
			log_info(team_logger, "Confirmada recepcion del pedido get para el pokemon: %s\n", objetivo->especie);
		}


		//TODO GUARDAR EL ID
	}

}

void convertirse_en_suscriptor_global_del_broker(){
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)enviar_mensaje_de_suscripcion_a_cola_appeared_pokemon,NULL);
	pthread_detach(hilo);

	pthread_t hilo1;
	pthread_create(&hilo1,NULL,(void*)enviar_mensaje_de_suscripcion_a_cola_localized_pokemon,NULL);
	pthread_detach(hilo1);

	pthread_t hilo2;
	pthread_create(&hilo2,NULL,(void*)enviar_mensaje_de_suscripcion_a_cola_caught_pokemon,NULL);
	pthread_detach(hilo2);
}

void hacer_intento_de_reconexion(){
	log_info(team_logger, "Haciendo intento de reconexión");
	sleep(tiempo_reconexion);
}

void * enviar_mensaje_de_suscripcion_a_cola_appeared_pokemon(){
	int broker_socket = conectar_a_server(ip_broker, "6009");

	t_suscripcion * suscripcion = malloc(sizeof(t_suscripcion));
	suscripcion->minutos_suscripcion = -1;
	suscripcion->tipo_suscripcion = SUSCRIPCION_GLOBAL;

	while(broker_socket > 0){
		enviar_solicitud_suscripcion(broker_socket,COLA_APPEARED_POKEMON,suscripcion);
		log_info(team_logger, "Solicitud de suscripcion a COLA_APPEARED_POKEMON enviada al broker");

		//Recibo ACK
		t_packed * paquete1 = recibir_mensaje(broker_socket);

		if(paquete1->operacion == ACK){
			log_info(team_logger, "Confirmada recepcion del mensaje de suscripcion a la cola de appeared_pokemon\n");
		}

		//Quedo a la espera de recibir notificaciones
		t_packed * paquete2 = recibir_mensaje(broker_socket);
		if(paquete2->operacion == ENVIAR_MENSAJE){
			if(paquete2->cola_de_mensajes == COLA_APPEARED_POKEMON){
				operar_con_appeared_pokemon(paquete2->mensaje);
			}
		}
	}

	free(suscripcion);
	if(broker_socket <= 0){
		log_info(team_logger, "No se pudo mandar al broker la solicitud para la cola appeared");
		hacer_intento_de_reconexion();
		enviar_mensaje_de_suscripcion_a_cola_appeared_pokemon();
	}

	return NULL;
}

void * enviar_mensaje_de_suscripcion_a_cola_localized_pokemon(){
	int broker_socket = conectar_a_server(ip_broker, "6009");

	t_suscripcion * suscripcion = malloc(sizeof(t_suscripcion));
	suscripcion->minutos_suscripcion = -1;
	suscripcion->tipo_suscripcion = SUSCRIPCION_GLOBAL;

	while(broker_socket > 0){
		enviar_solicitud_suscripcion(broker_socket,COLA_LOCALIZED_POKEMON,suscripcion);
		log_info(team_logger, "Solicitud de suscripcion a COLA_LOCALIZED_POKEMON enviada al broker");

		//Recibo ACK
		t_packed * paquete1 = recibir_mensaje(broker_socket);

		if(paquete1->operacion == ACK){
			log_info(team_logger, "Confirmada recepcion del mensaje de suscripcion a la cola localized_pokemon\n");
		}

		//Quedo a la espera de recibir notificaciones
		t_packed * paquete2 = recibir_mensaje(broker_socket);
		if(paquete2->operacion == ENVIAR_MENSAJE){
			if(paquete2->cola_de_mensajes == COLA_LOCALIZED_POKEMON){
				operar_con_localized_pokemon(paquete2->mensaje);
			}
		}
	}

	free(suscripcion);
	if(broker_socket <= 0){
		log_info(team_logger_oficial, "Falló la conexión con Broker; inicia la operación default");
		log_info(team_logger, "No se pudo mandar al broker la solicitud para la cola localized");
		log_info(team_logger_oficial, "Se inicia el intento de reconexión con Broker");
		hacer_intento_de_reconexion();
		enviar_mensaje_de_suscripcion_a_cola_localized_pokemon();
	}

	return NULL;
}

void * enviar_mensaje_de_suscripcion_a_cola_caught_pokemon(){
	int broker_socket = conectar_a_server(ip_broker, "6009");

	t_suscripcion * suscripcion = malloc(sizeof(t_suscripcion));
	suscripcion->minutos_suscripcion = -1;
	suscripcion->tipo_suscripcion = SUSCRIPCION_GLOBAL;

	while(broker_socket > 0){
		enviar_solicitud_suscripcion(broker_socket,COLA_CAUGHT_POKEMON,suscripcion);
		log_info(team_logger, "Solicitud de suscripcion a COLA_CAUGHT_POKEMON enviada al broker");

		//Recibo ACK
		t_packed * paquete1 = recibir_mensaje(broker_socket);

		if(paquete1->operacion == ACK){
			log_info(team_logger, "Confirmada recepcion del mensaje de suscripcion a la cola caught_pokemon\n");
		}

		//Quedo a la espera de recibir notificaciones
		t_packed * paquete2 = recibir_mensaje(broker_socket);
		if(paquete2->operacion == ENVIAR_MENSAJE){
			if(paquete2->cola_de_mensajes == COLA_CAUGHT_POKEMON){
				//operar_con_caught_pokemon(paquete2->mensaje);
			}
		}
	}

	free(suscripcion);
	if(broker_socket <= 0){
		log_info(team_logger, "No se pudo mandar al broker la solicitud para la cola caught");
		hacer_intento_de_reconexion();
		enviar_mensaje_de_suscripcion_a_cola_caught_pokemon();
	}

	return NULL;
}
