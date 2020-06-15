#include "Broker.h"

int main(){

	cache_mensajes = (t_cache_colas*)malloc(sizeof(t_cache_colas));
	cache_mensajes->mensajes = list_create();
	cache_mensajes->colas = list_create();

	t_cola_mensajes* aux_crear_cola_mensajes; 

	pthread_t hilo_sender[COLA_LOCALIZED_POKEMON];

	for(int i = COLA_APPEARED_POKEMON; i <= COLA_LOCALIZED_POKEMON; i++){

		aux_crear_cola_mensajes = crear_cola_mensajes(i);

		list_add(cache_mensajes->colas,aux_crear_cola_mensajes);

		pthread_create(&hilo_sender[i],NULL,sender_suscriptores,aux_crear_cola_mensajes);
		
	}

	/*for(int i = COLA_APPEARED_POKEMON; i <= COLA_LOCALIZED_POKEMON; i++){

		pthread_join(&hilo_sender[i],NULL);
		
	}*/

	inicializar_archivo_de_configuracion();
	
	configurar_signals();

	iniciar_servidor();

	/* TODO: Obtener este valor desde la config */
	int tamanio_memoria_inicial = 8000;

	//primer_bloque = AsignarMemoriaInicial(tamanio_memoria_inicial,lista_memoria);
	while(true){
	
	}
}

t_cola_mensajes* crear_cola_mensajes(int cola_mensajes){

	t_cola_mensajes* cola_mensaje; 
	cola_mensaje = (t_cola_mensajes*)malloc(sizeof(t_cola_mensajes));

	sem_t* producciones;
	producciones = (sem_t*)malloc(sizeof(sem_t));

	sem_init(producciones, 0, 0); 	

	cola_mensaje->cola_de_mensajes = cola_mensajes;
	cola_mensaje->envios_pendientes = list_create();
	cola_mensaje->suscriptores = list_create();
	cola_mensaje->producciones = producciones;

	//list_add(cola_mensaje->suscriptores,123);
	//list_add(cola_mensaje->suscriptores,234);

	return cola_mensaje;

}

void* sender_suscriptores(t_cola_mensajes* cola){

	t_mensaje_cola* mensaje;

	t_envio_pendiente* envio_pendiente;

	while(1){

		sem_wait(cola->producciones);	

		pthread_mutex_lock(&mutex_queue_mensajes);	

		printf("\n\nLos pendientes tienen %d mensajes",(cola->envios_pendientes)->elements_count);
		
		envio_pendiente = list_get(cola->envios_pendientes,0);

		mensaje = list_get(cache_mensajes->mensajes,envio_pendiente->id);

		int envio = enviar_mensaje_a_suscriptor(envio_pendiente->id,
												mensaje->id_correlacional, 
												cola->cola_de_mensajes, 
												envio_pendiente->cliente, 
												mensaje->mensaje);

		if(envio != -1){			
			printf("\nEnviado correctamente mensaje de id %d de cola %d al cliente %d!!!",envio_pendiente->id,cola->cola_de_mensajes,envio_pendiente->cliente);
			printf("\nLos pendientes quedaron con %d mensajes",(cola->envios_pendientes)->elements_count);
		}else{
			//QUE HAGO SI NO PUDE ENVIAR EL MENSAJE CORRECTAMENTE AL SUSCRIPTOR????
			printf("\nFallo el envio del mensaje de id %d de cola %d al cliente %d!!! Se perdió :c",envio_pendiente->id,cola->cola_de_mensajes,envio_pendiente->cliente);
		}

		list_remove_and_destroy_element(cola->envios_pendientes,0,eliminar_envio_pendiente);

		pthread_mutex_unlock(&mutex_queue_mensajes);
	
	}

	return NULL;
}

void eliminar_envio_pendiente(t_envio_pendiente* pendiente){
	free(pendiente);
	return NULL;
}

int enviar_mensaje_a_suscriptor(int id_mensaje,
								int id_correlacional, 
								enum COLA_DE_MENSAJES cola_de_mensajes, 
								int cliente, 
								void* mensaje){
	int send_status = -1;

	switch(cola_de_mensajes){

		case COLA_CATCH_POKEMON:
			send_status = distribuir_catch_pokemon(cliente,id_mensaje,id_correlacional,(t_catch_pokemon*)mensaje);
			break;
 
		case COLA_APPEARED_POKEMON:
			send_status = distribuir_appeared_pokemon(cliente,id_mensaje,id_correlacional,(t_appeared_pokemon*)mensaje);
			break;

		case COLA_NEW_POKEMON:
			send_status = distribuir_new_pokemon(cliente,id_mensaje,id_correlacional,(t_new_pokemon*)mensaje);
			break;
		
		case COLA_CAUGHT_POKEMON:
			send_status = distribuir_caught_pokemon(cliente,id_mensaje,id_correlacional,(t_caught_pokemon*)mensaje);
			break;
		
		case COLA_GET_POKEMON:
			send_status = distribuir_get_pokemon(cliente,id_mensaje,id_correlacional,(t_get_pokemon*)mensaje);
			break;

		case COLA_LOCALIZED_POKEMON:
			//send_status = enviar_localized_pokemon(cliente,id_mensaje,id_correlacional,(t_localized_pokemon*)mensaje);
			break;			

		default:
			printf("Error, cola de mensajes desconocida: %d\n",cola_de_mensajes);
			break;
	}

	return send_status;

}