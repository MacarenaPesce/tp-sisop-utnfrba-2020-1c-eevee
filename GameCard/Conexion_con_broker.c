/*
 * Conexion_con_broker.c
 *
 *  Created on: 2 jul. 2020
 *      Author: utnso
 */

#include "Conexion_con_broker.h"
#include "Pokebola.h"

void * atender_get_pokemon(t_packed * paquete){

	t_servidor * servidor = malloc(sizeof(t_servidor));
	servidor->ip = ip_broker;
	servidor->puerto = puerto_broker;
	servidor->id_cliente = id;

	t_get_pokemon * get_pokemon = (t_get_pokemon*)paquete->mensaje;
	log_debug(gameCard_logger, "Recibi el pedido de get pokemon para esta especie: %s.", get_pokemon->pokemon);

	//t_list* listaPosicionesParaLocalized;
	listaPosicionesParaLocalized=operar_con_get_pokemon(get_pokemon);

	if (!list_is_empty(listaPosicionesParaLocalized)){

		t_localized_pokemon* localized_pokemon= generar_localized(get_pokemon->pokemon);
		
		t_list* lista_coordenadas = listaPosicionesParaLocalized;

		void cargar_localized(void* _coordenadas){

			t_coordenadas* coordenadas = (t_coordenadas*) _coordenadas;

			agregar_coordenadas_a_localized(localized_pokemon,coordenadas);

		}

		list_iterate(lista_coordenadas,cargar_localized);

		t_packed * ack = enviar_localized_pokemon(servidor,paquete->id_mensaje,localized_pokemon);

		eliminar_localized_pokemon(localized_pokemon);
		if(ack != (t_packed*) -1){
			eliminar_mensaje(ack);
		}
		list_destroy(lista_coordenadas);
	}

	free(servidor);

	return NULL;
}

void * atender_catch_pokemon(t_packed * paquete){
	t_servidor * servidor = malloc(sizeof(t_servidor));
	servidor->ip = ip_broker;
	servidor->puerto = puerto_broker;
	servidor->id_cliente = id;

	t_catch_pokemon * catch_pokemon = paquete->mensaje;
	log_debug(gameCard_logger, "Recibi el pedido de catch pokemon para la especie %s en la posicion (%d, %d).", catch_pokemon->pokemon, catch_pokemon->coordenadas.posx, catch_pokemon->coordenadas.posy);

	t_caught_pokemon * caught_pokemon = malloc(sizeof(t_caught_pokemon));
	caught_pokemon->status = operar_con_catch_pokemon(catch_pokemon);

	t_packed * ack = enviar_caught_pokemon(servidor, paquete->id_mensaje, caught_pokemon);

	if(ack == (t_packed*) -1){
		log_info(gameCard_logger, "El broker esta caído, no se pudo enviar el caught pokemon para la especie %s", catch_pokemon->pokemon);
	}else{
		log_info(gameCard_logger, "Enviado el status del caught pokemon para la especie %s", catch_pokemon->pokemon);
	eliminar_mensaje(ack);
	}

	free(servidor);
	//eliminar_mensaje(paquete);
	free(caught_pokemon);

	return NULL;
}

void * atender_new_pokemon(t_packed * paquete){
	t_servidor * servidor = malloc(sizeof(t_servidor));
	servidor->ip = ip_broker;
	servidor->puerto = puerto_broker;
	servidor->id_cliente = id;

	t_new_pokemon * new_pokemon = paquete->mensaje;

	log_debug(gameCard_logger, "Recibi el pedido de new pokemon para la especie %s en (%d, %d). Cantidad: %d. ", new_pokemon->pokemon, new_pokemon->coordenadas.posx,
			new_pokemon->coordenadas.posy, new_pokemon->cantidad);

	operar_con_new_pokemon(new_pokemon);

	t_appeared_pokemon * appeared_pokemon = malloc(sizeof(t_appeared_pokemon));
	appeared_pokemon->pokemon = new_pokemon->pokemon;
	appeared_pokemon->coordenadas.posx = new_pokemon->coordenadas.posx;
	appeared_pokemon->coordenadas.posy = new_pokemon->coordenadas.posy;

	t_packed * ack = enviar_appeared_pokemon(servidor, paquete->id_correlacional, appeared_pokemon);

	if(ack == (t_packed*) -1){
		log_info(gameCard_logger, "El broker esta caído, no se pudo enviar el appeared pokemon para la especie %s, en la posicion ( %d, %d).", appeared_pokemon->pokemon,
				appeared_pokemon->coordenadas.posx, appeared_pokemon->coordenadas.posy);
	}else{
		log_info(gameCard_logger, "Enviado appeared pokemon para la especie %s, en la posicion ( %d, %d).",
				appeared_pokemon->pokemon, appeared_pokemon->coordenadas.posx, appeared_pokemon->coordenadas.posy);
		eliminar_mensaje(ack);
	}

	free(servidor);
	/*free(new_pokemon->pokemon);
	free(new_pokemon);
	eliminar_mensaje(ack);*/
	free(appeared_pokemon);

	return NULL;
}

void * recibir_new_pokemon_desde_broker(t_packed * paquete){

	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)atender_new_pokemon, (void*)paquete);
	pthread_join(hilo, NULL);

	return NULL;
}

void * recibir_catch_pokemon_desde_broker(t_packed * paquete){

	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)atender_catch_pokemon, (void*)paquete);
	pthread_join(hilo, NULL);

	return NULL;
}

void * recibir_get_pokemon_desde_broker(t_packed * paquete){

	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)atender_get_pokemon, (void*)paquete);
	pthread_join(hilo, NULL);

	return NULL;
}

void convertirse_en_suscriptor_global_del_broker(){

	int colas_a_suscribirse[] = {COLA_NEW_POKEMON, COLA_CATCH_POKEMON, COLA_GET_POKEMON};
	void * operacion[] = {recibir_new_pokemon_desde_broker, recibir_catch_pokemon_desde_broker, recibir_get_pokemon_desde_broker};

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
	log_info(gameCard_logger, "Haciendo intento de reconexión con broker");
	sleep(tiempo_reintento_conexion);
}

void * suscribirse_a_cola(t_suscripcion_a_broker * paquete_suscripcion){

	t_servidor * servidor = malloc(sizeof(t_servidor));
	servidor->ip = ip_broker;
	servidor->puerto = puerto_broker;
	servidor->id_cliente = id;

	int broker_socket = enviar_solicitud_suscripcion(servidor,paquete_suscripcion->cola);

	while(1){

		if(broker_socket <= 0){
			log_info(gameCard_logger, "No se pudo mandar al broker la solicitud de suscripcion para la cola %s", obtener_nombre_cola(paquete_suscripcion->cola));
			hacer_intento_de_reconexion();
			suscribirse_a_cola(paquete_suscripcion);
			break;
		}

		//Recibo ACK
		 t_packed * paquete = recibir_mensaje(broker_socket);

		        if(paquete == (t_packed*)-2){
		            //El socket finalizó, esta suscripcion no sirve mas
		            break;
		        }

		//}else{
			//Recibo ACK
			//t_packed * paquete = recibir_mensaje(broker_socket);

			if(paquete != (t_packed*)-1){

				//Quedo a la espera de recibir notificaciones
				if(paquete->operacion == ENVIAR_MENSAJE){
					switch(paquete->cola_de_mensajes){
						case COLA_NEW_POKEMON:
							//se envia ack al broker que llego bien
							enviar_ack(servidor,paquete->id_mensaje);
							log_info(gameCard_logger,"se informó la recepcion del mensaje al broker");
							recibir_new_pokemon_desde_broker(paquete);
							eliminar_mensaje(paquete);
							break;
						case COLA_CATCH_POKEMON:
							//se envia ack al broker que llego bien
							enviar_ack(servidor,paquete->id_mensaje);
							log_info(gameCard_logger,"se informó la recepcion del mensaje al broker");
							recibir_catch_pokemon_desde_broker(paquete);
							eliminar_mensaje(paquete);
							break;
						case COLA_GET_POKEMON:
							//se envia ack al broker que llego bien
							enviar_ack(servidor,paquete->id_mensaje);
							log_info(gameCard_logger,"se informó la recepcion del mensaje al broker");
							recibir_get_pokemon_desde_broker(paquete);
							eliminar_mensaje(paquete);
							break;
						default:
							log_error(gameCard_logger, "RECIBI COLA INVALIDA");
							log_error(gameCard_logger, "COLA DE MENSAJES:%d", paquete->cola_de_mensajes);
							eliminar_mensaje(paquete);
							break;
					}
				}
			} else {
				if(paquete->operacion == ACK){
					eliminar_mensaje(paquete);
				}
			}
			
		}



	free(servidor);
	free(paquete_suscripcion);

	return NULL;
}
