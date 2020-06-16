/*
 * Interaccion_con_broker.c
 *
 *  Created on: 11 jun. 2020
 *      Author: utnso
 */

#include "Interaccion_con_broker.h"

void * recibir_appeared_pokemon_desde_broker(t_appeared_pokemon * mensaje){
	log_info(team_logger,"Voy a recibir un pokemon y coordenadas desde broker");
	log_info(team_logger,"Me llego este pokemon: %s, coordenada x %d, coordenada y %d", mensaje->pokemon, mensaje->coordenadas.posx, mensaje->coordenadas.posy);

	t_pokemon * pokemon = malloc(sizeof(t_pokemon));
	pokemon->especie = mensaje->pokemon;
	pokemon->posx = mensaje->coordenadas.posx;
	pokemon->posy = mensaje->coordenadas.posy;

	operar_con_appeared_pokemon(pokemon);

	return NULL;
}

void * recibir_localized_pokemon_desde_broker(t_localized_pokemon * mensaje){
	log_info(team_logger,"Voy a recibir un pokemon y coordenadas desde broker para localized pokemon %s", mensaje->pokemon);
	//operar_con_localized_pokemon();
	return NULL;
}

void * recibir_caught_pokemon_desde_broker(t_caught_pokemon * mensaje){
	log_info(team_logger,"Voy a recibir un status desde broker para caught pokemon %d", mensaje->status);

	//operar_con_caught_pokemon();
	return NULL;
}

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
	t_suscripcion_a_broker * paquete_suscripcion = malloc(sizeof(paquete_suscripcion));

	int colas_a_suscribirse[3] = {COLA_APPEARED_POKEMON, COLA_LOCALIZED_POKEMON, COLA_CAUGHT_POKEMON};
	void * operacion[3] = {recibir_appeared_pokemon_desde_broker, recibir_localized_pokemon_desde_broker, recibir_caught_pokemon_desde_broker};

	for(int i = 0; i < 3; i++){
		paquete_suscripcion->cola = colas_a_suscribirse[i];
		paquete_suscripcion->operacion = &operacion[i];

		pthread_t hilo;
		pthread_create(&hilo,NULL,(void*)suscribirse_a_cola, (void*)paquete_suscripcion);
		pthread_detach(hilo);
	}

}

void hacer_intento_de_reconexion(){
	log_info(team_logger, "Haciendo intento de reconexión");
	sleep(tiempo_reconexion);
}

void * suscribirse_a_cola(t_suscripcion_a_broker * paquete_suscripcion){
	t_servidor * servidor = malloc(sizeof(t_servidor));
	servidor->ip = ip_broker;
	servidor->puerto = "6009";

	t_suscripcion * suscripcion = malloc(sizeof(t_suscripcion));
	suscripcion->minutos_suscripcion = -1;
	suscripcion->tipo_suscripcion = SUSCRIPCION_GLOBAL;

	int broker_socket = enviar_solicitud_suscripcion(servidor,paquete_suscripcion->cola, suscripcion);
	log_info(team_logger, "Solicitud de suscripcion a COLA_APPEARED_POKEMON enviada al broker");

	while(1){
		//Recibo ACK
		t_packed * paquete = recibir_mensaje(broker_socket);

		if(paquete != (t_packed*)-1){
			//Quedo a la espera de recibir notificaciones
			if(paquete->operacion == ENVIAR_MENSAJE){
				(paquete_suscripcion->operacion)(paquete->mensaje);
			}
		}
	}

	free(servidor);
	free(suscripcion);

	if(broker_socket <= 0){
		log_info(team_logger, "No se pudo mandar al broker la solicitud para la cola");
		hacer_intento_de_reconexion();
		suscribirse_a_cola(paquete_suscripcion);
	}

	return NULL;
}
