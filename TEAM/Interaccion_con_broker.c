/*
 * Interaccion_con_broker.c
 *
 *  Created on: 11 jun. 2020
 *      Author: utnso
 */

#include "Interaccion_con_broker.h"

void * recibir_appeared_pokemon_desde_broker(t_packed * paquete){

	t_appeared_pokemon* appeared = (t_appeared_pokemon*)paquete->mensaje;

	log_debug(team_logger, "Llego el sgte mensaje: APPEARED_POKEMON, de la especie %s en las coordenadas (%d, %d)", appeared->pokemon, appeared->coordenadas.posx, appeared->coordenadas.posy);
	log_info(team_logger_oficial, "Llego el sgte mensaje: APPEARED_POKEMON, de la especie %s en las coordenadas (%d, %d)", appeared->pokemon, appeared->coordenadas.posx, appeared->coordenadas.posy);

	pthread_mutex_lock(&mensaje_nuevo_mutex);
	list_add(mensajes_que_llegan_nuevos, (void*)paquete);
	pthread_mutex_unlock(&mensaje_nuevo_mutex);

	sem_post(&mensaje_nuevo_disponible);

	sem_wait(&paquete_usado);

	return NULL;
}

void * recibir_localized_pokemon_desde_broker(t_packed * paquete){

	t_localized_pokemon * localized = (t_localized_pokemon*)paquete->mensaje;

	log_debug(team_logger, "Llego el sgte mensaje: LOCALIZED_POKEMON de la especie %s", localized->pokemon);
	log_debug(team_logger_oficial, "Llego el sgte mensaje: LOCALIZED_POKEMON de la especie %s", localized->pokemon);

	pthread_mutex_lock(&mensaje_nuevo_mutex);
	list_add(mensajes_que_llegan_nuevos, (void*)paquete);
	pthread_mutex_unlock(&mensaje_nuevo_mutex);

	sem_post(&mensaje_nuevo_disponible);

	sem_wait(&paquete_usado);

	return NULL;
}

void * recibir_caught_pokemon_desde_broker(t_packed * paquete){

	t_caught_pokemon * caught = paquete->mensaje;

	log_debug(team_logger, "Llego el sgte mensaje: CAUGHT_POKEMON, id correlativo --> %d y status %d", paquete->id_correlacional, caught->status);
	log_debug(team_logger_oficial, "Llego el sgte mensaje: CAUGHT_POKEMON, id correlativo --> %d y status %d", paquete->id_correlacional, caught->status);

	pthread_mutex_lock(&mensaje_nuevo_mutex);
	list_add(mensajes_que_llegan_nuevos, (void*)paquete);
	pthread_mutex_unlock(&mensaje_nuevo_mutex);

	sem_post(&mensaje_nuevo_disponible);

	sem_wait(&paquete_usado);

	return NULL;
}

void enviar_get(){
	sem_wait(&objetivos_listos);
	
	t_servidor * servidor = (t_servidor*)malloc(sizeof(t_servidor));
	servidor->ip = ip_broker;
	servidor->puerto = puerto_broker;
	servidor->id_cliente = id;

	void _enviar_get_pokemon(void* _objetivo){

		t_objetivo * objetivo = (t_objetivo*) _objetivo;

		if(objetivo == NULL){
			return;
		}

		t_get_pokemon * get_pokemon = malloc(sizeof(t_get_pokemon));
		get_pokemon->pokemon = objetivo->especie;

		t_packed * ack = enviar_get_pokemon(servidor, -1, get_pokemon);
		log_info(team_logger, "Enviado pedido de get pokemon para esta especie: %s", objetivo->especie);

		//Recibo ACK
		if(ack->operacion == ACK){

			log_info(team_logger, "Confirmada recepcion del pedido get para el pokemon: %s\n", objetivo->especie);
			uint32_t* id_esperando_respuesta = (uint32_t*)malloc(sizeof(uint32_t));
			memcpy(id_esperando_respuesta,&ack->id_mensaje,sizeof(uint32_t));

			pthread_mutex_lock(&mensaje_chequear_id_mutex);
			list_add(mensajes_para_chequear_id, (void*)id_esperando_respuesta);
			pthread_mutex_unlock(&mensaje_chequear_id_mutex);

			eliminar_mensaje(ack);

		}else{
			log_info(team_logger_oficial, "Falló la conexión con Broker; inicia la operación default");
			log_info(team_logger, "No existen locaciones para esta especie: %s, cant %i", objetivo->especie, objetivo->cantidad_necesitada);
		}

		free(get_pokemon);
	}

	list_iterate(lista_objetivos,_enviar_get_pokemon);
	free(servidor);
	
}

void convertirse_en_suscriptor_global_del_broker(){

	int colas_a_suscribirse[] = {COLA_APPEARED_POKEMON, COLA_LOCALIZED_POKEMON, COLA_CAUGHT_POKEMON};
	void * operacion[] = {recibir_appeared_pokemon_desde_broker, recibir_localized_pokemon_desde_broker, recibir_caught_pokemon_desde_broker};

	for(int i = 0; i < 3; i++){
		t_suscripcion_a_broker * paquete_suscripcion = malloc(sizeof(t_suscripcion_a_broker));
		paquete_suscripcion->cola = colas_a_suscribirse[i];
		paquete_suscripcion->operacion = (void*)&operacion[i];

		pthread_t hilo;
		pthread_create(&hilo,NULL,(void*)suscribirse_a_cola, (void*)paquete_suscripcion);
		pthread_detach(hilo);
	}

}

void hacer_intento_de_reconexion(){
	log_info(team_logger_oficial, "Haciendo intento de reconexión");
	log_info(team_logger, "Haciendo intento de reconexión");
	sleep(tiempo_reconexion);
}

void * suscribirse_a_cola(t_suscripcion_a_broker * paquete_suscripcion){

	t_servidor * servidor = malloc(sizeof(t_servidor));
	servidor->ip = ip_broker;
	servidor->puerto = puerto_broker;
	servidor->id_cliente = id;

	int broker_socket = enviar_solicitud_suscripcion(servidor,paquete_suscripcion->cola);

	while(1){
		pthread_mutex_lock(&global_seguir_mutex);
		if(GLOBAL_SEGUIR == 0){
			break;
		}
		pthread_mutex_unlock(&global_seguir_mutex);

		if(broker_socket <= 0){
			log_info(team_logger, "No se pudo mandar al broker la solicitud de suscripcion para la cola %s", obtener_nombre_cola(paquete_suscripcion->cola));
			log_info(team_logger_oficial, "No se pudo mandar al broker la solicitud de suscripcion para la cola %s", obtener_nombre_cola(paquete_suscripcion->cola));
			hacer_intento_de_reconexion();
			suscribirse_a_cola(paquete_suscripcion);

		}else{

			//Recibo ACK
			t_packed * paquete = recibir_mensaje(broker_socket);

			if(paquete != (t_packed*)-1){
				//Quedo a la espera de recibir notificaciones
				if(paquete->operacion == ENVIAR_MENSAJE){
					switch(paquete->cola_de_mensajes){
						case COLA_APPEARED_POKEMON:
							recibir_appeared_pokemon_desde_broker(paquete);
							eliminar_mensaje(paquete);
							break;
						case COLA_LOCALIZED_POKEMON:
							recibir_localized_pokemon_desde_broker(paquete);
							eliminar_mensaje(paquete);
							break;
						case COLA_CAUGHT_POKEMON:
							recibir_caught_pokemon_desde_broker(paquete);
							eliminar_mensaje(paquete);
							break;
						default:
							eliminar_mensaje(paquete);
							break;
					}
				}else{
					if(paquete->operacion == ACK){
						free(paquete);
					}
				}
			}
		}
	}

	free(servidor);
	free(paquete_suscripcion);
	free(broker_socket);

	return NULL;
}

/*if(es_la_primera_vez){
						es_la_primera_vez = false;
					}else{
						free(paquete);
					}*/
