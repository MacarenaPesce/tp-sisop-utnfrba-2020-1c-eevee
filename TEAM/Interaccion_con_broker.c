/*
 * Interaccion_con_broker.c
 *
 *  Created on: 11 jun. 2020
 *      Author: utnso
 */

#include "Interaccion_con_broker.h"

void * recibir_appeared_pokemon_desde_broker(t_packed * paquete){

	t_appeared_pokemon * appeared = paquete->mensaje;

	t_mensaje_guardado * mensaje = malloc(sizeof(t_mensaje_guardado));
	mensaje->id = paquete->id_mensaje;
	mensaje->id_correlacional = paquete->id_correlacional;
	mensaje->operacion = APPEARED;
	mensaje->contenido = appeared;

	log_debug(team_logger, "Llego el sgte mensaje: APPEARED_POKEMON, de la especie %s en las coordenadas (%d, %d)", appeared->pokemon, appeared->coordenadas.posx, appeared->coordenadas.posy);

	pthread_mutex_lock(&mensaje_nuevo_mutex);
	list_add(mensajes_que_llegan_nuevos, mensaje);
	pthread_mutex_unlock(&mensaje_nuevo_mutex);

	sem_post(&mensaje_nuevo_disponible);

	free(paquete);

	return NULL;
}

void * recibir_localized_pokemon_desde_broker(t_packed * paquete){

	t_localized_pokemon * localized = paquete->mensaje;

	t_mensaje_guardado * mensaje = malloc(sizeof(t_mensaje_guardado));
	mensaje->id = paquete->id_mensaje;
	mensaje->id_correlacional = paquete->id_correlacional;
	mensaje->operacion = LOCALIZED;
	mensaje->contenido = localized;

	log_debug(team_logger, "Llego el sgte mensaje: LOCALIZED_POKEMON");

	pthread_mutex_lock(&mensaje_nuevo_mutex);
	list_add(mensajes_que_llegan_nuevos, mensaje);
	pthread_mutex_unlock(&mensaje_nuevo_mutex);

	sem_post(&mensaje_nuevo_disponible);

	free(paquete);

	return NULL;
}

void * recibir_caught_pokemon_desde_broker(t_packed * paquete){

	t_caught_pokemon * caught = paquete->mensaje;

	t_mensaje_guardado * mensaje = malloc(sizeof(t_mensaje_guardado));
	mensaje->id = paquete->id_mensaje;
	mensaje->id_correlacional = paquete->id_correlacional;
	mensaje->operacion = CAUGHT;
	mensaje->contenido = caught;

	log_debug(team_logger, "Llego el sgte mensaje: CAUGHT_POKEMON, id correlativo --> %d y status %d", paquete->id_correlacional, caught->status);

	pthread_mutex_lock(&mensaje_nuevo_mutex);
	list_add(mensajes_que_llegan_nuevos, mensaje);
	pthread_mutex_unlock(&mensaje_nuevo_mutex);

	sem_post(&mensaje_nuevo_disponible);

	free(paquete);

	return NULL;
}

void enviar_get(){
	t_servidor * servidor = malloc(sizeof(t_servidor));
	servidor->ip = ip_broker;
	servidor->puerto = "6009";
	servidor->id_cliente = id;

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

				t_mensaje_guardado * mensaje = malloc(sizeof(t_mensaje_guardado));
				mensaje->id = ack->id_mensaje;
				mensaje->operacion = GET;
				mensaje->contenido = get_pokemon;

				pthread_mutex_lock(&mensaje_chequear_id_mutex);
				list_add(mensajes_para_chequear_id, mensaje);
				pthread_mutex_unlock(&mensaje_chequear_id_mutex);

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
	t_suscripcion_a_broker * paquete_suscripcion = malloc(sizeof(t_suscripcion_a_broker));

	int colas_a_suscribirse[] = {COLA_APPEARED_POKEMON, COLA_LOCALIZED_POKEMON, COLA_CAUGHT_POKEMON};
	void * operacion[] = {recibir_appeared_pokemon_desde_broker, recibir_localized_pokemon_desde_broker, recibir_caught_pokemon_desde_broker};

	for(int i = 0; i < 3; i++){
		paquete_suscripcion->cola = colas_a_suscribirse[i];
		paquete_suscripcion->operacion = (void*)&operacion[i];

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
	servidor->id_cliente = id;

	t_suscripcion * suscripcion = malloc(sizeof(t_suscripcion));
	suscripcion->minutos_suscripcion = -1;
	suscripcion->tipo_suscripcion = SUSCRIPCION_GLOBAL;

	int broker_socket = enviar_solicitud_suscripcion(servidor,paquete_suscripcion->cola, suscripcion);

	while(GLOBAL_SEGUIR){

		if(broker_socket <= 0){
			log_info(team_logger, "No se pudo mandar al broker la solicitud de suscripcion para la cola");
			hacer_intento_de_reconexion();
			suscribirse_a_cola(paquete_suscripcion);

		}else{

			//Recibo ACK
			t_packed * paquete = recibir_mensaje(broker_socket);

			if(paquete != (t_packed*)-1){
				//Quedo a la espera de recibir notificaciones
				if(paquete->operacion == ENVIAR_MENSAJE){
					(paquete_suscripcion->operacion)(paquete);
				}
			}
		}
	}

	free(servidor);
	free(suscripcion);

	return NULL;
}
