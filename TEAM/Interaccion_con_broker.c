/*
 * Interaccion_con_broker.c
 *
 *  Created on: 11 jun. 2020
 *      Author: utnso
 */

#include "Interaccion_con_broker.h"

void enviar_get(){
	t_servidor * servidor = malloc(sizeof(t_servidor));
	servidor->ip = ip_broker;
	servidor->puerto = "6009";

	int h = 0;
	t_objetivo * objetivo;

	while(lista_objetivos != NULL){
		objetivo = list_get(lista_objetivos, h);
		if(objetivo == NULL){
			break;
		}

		t_get_pokemon * get_pokemon = malloc(sizeof(t_get_pokemon));
		get_pokemon->pokemon = objetivo->especie;

		t_packed * ack = enviar_get_pokemon(servidor, -1, get_pokemon);
		log_info(team_logger, "Enviado pedido de get pokemon para esta especie: %s", objetivo->especie);

		if(ack != (t_packed*)-1){
			h++;

			//Recibo ACK
			if(ack->operacion == ACK){
				log_info(team_logger, "Confirmada recepcion del pedido get para el pokemon: %s\n", objetivo->especie);
				log_info(team_logger, "EL ID DEL MENSAJE ES: %d\n", ack->id_mensaje);

			}

		}else{
			log_info(team_logger_oficial, "Falló la conexión con Broker; inicia la operación default");
			log_info(team_logger, "No existen locaciones para esta especie: %s, cant %i", objetivo->especie, objetivo->cantidad_necesitada);
			h++;
		}
		free(get_pokemon);
	}

	free(servidor);
}

void convertirse_en_suscriptor_global_del_broker(){
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)enviar_mensaje_de_suscripcion_a_cola_appeared_pokemon,NULL);
	pthread_detach(hilo);

	/*pthread_t hilo1;
	pthread_create(&hilo1,NULL,(void*)enviar_mensaje_de_suscripcion_a_cola_localized_pokemon,NULL);
	pthread_detach(hilo1);

	pthread_t hilo2;
	pthread_create(&hilo2,NULL,(void*)enviar_mensaje_de_suscripcion_a_cola_caught_pokemon,NULL);
	pthread_detach(hilo2);*/
}

void hacer_intento_de_reconexion(){
	log_info(team_logger, "Haciendo intento de reconexión");
	sleep(tiempo_reconexion);
}

void * enviar_mensaje_de_suscripcion_a_cola_appeared_pokemon(){
	/*t_servidor servidor;
	servidor.ip = ip_broker;
	servidor.puerto = "6009";

	t_suscripcion * suscripcion = malloc(sizeof(t_suscripcion));
	suscripcion->minutos_suscripcion = -1;
	suscripcion->tipo_suscripcion = SUSCRIPCION_GLOBAL;

	int broker_socket = enviar_solicitud_suscripcion(servidor,COLA_APPEARED_POKEMON, suscripcion);
	log_info(team_logger, "Solicitud de suscripcion a COLA_APPEARED_POKEMON enviada al broker");

	while(broker_socket > 0){
		//Recibo ACK
		t_packed * ack = recibir_mensaje(broker_socket);

		if(ack->operacion == ACK){
			log_info(team_logger, "Confirmada recepcion del mensaje de suscripcion a la cola de appeared_pokemon\n");
		}

		//Quedo a la espera de recibir notificaciones
		t_packed * paquete = recibir_mensaje(broker_socket);
		if(paquete->operacion == ENVIAR_MENSAJE){
			if(paquete->cola_de_mensajes == COLA_APPEARED_POKEMON){
				operar_con_appeared_pokemon(paquete->mensaje);
			}
		}
	}

	free(suscripcion);

	if(broker_socket <= 0){
		log_info(team_logger, "No se pudo mandar al broker la solicitud para la cola appeared");
		hacer_intento_de_reconexion();
		enviar_mensaje_de_suscripcion_a_cola_appeared_pokemon();
	}

	return NULL;*/
}
