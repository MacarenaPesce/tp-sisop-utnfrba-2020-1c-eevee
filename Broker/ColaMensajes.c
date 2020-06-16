#include "ColaMensajes.h";

extern t_cache_colas* cache_mensajes;

/* Flujo de mensajes */
int agregar_mensaje_a_cola(t_packed* paquete){	

	t_mensaje_cola* mensaje;
	mensaje = (t_mensaje_cola*)malloc(sizeof(t_mensaje_cola));

	mensaje->id_mensaje = cache_mensajes->proximo_id_mensaje;
	mensaje->cola_de_mensajes = paquete->cola_de_mensajes;
	mensaje->id_correlacional = paquete->id_correlacional;
	mensaje->mensaje = paquete->mensaje;

	cache_mensajes->proximo_id_mensaje++;

	pthread_mutex_lock(&mutex_queue_mensajes);

	list_add(cache_mensajes->mensajes,(void*)mensaje);

	encolar_envio_a_suscriptores(mensaje->cola_de_mensajes, mensaje->id_mensaje);

	pthread_mutex_unlock(&mutex_queue_mensajes);

	return mensaje->id_mensaje;

}

void encolar_envio_a_suscriptores(int cola_de_mensajes,int id_mensaje){

	t_cola_mensajes* cola = obtener_cola_mensajes(cola_de_mensajes);

    void enviar_mensaje_a_suscriptores(void* suscriptor){

        agregar_pendiente_de_envio(cola,id_mensaje,*((int*)suscriptor));

    }

	list_iterate(cola->suscriptores,enviar_mensaje_a_suscriptores);

}

/* Flujo de suscripciones */
void agregar_suscriptor_a_cola(int cola_de_mensajes, int cliente){
	
	int* id_cliente = (int*)malloc(sizeof(int));
	*id_cliente = cliente;

	pthread_mutex_lock(&mutex_queue_mensajes);

	t_cola_mensajes* cola = obtener_cola_mensajes(cola_de_mensajes);

	list_add(cola->suscriptores,(void*)id_cliente);

	enviar_mensajes_cacheados_a_nuevo_suscriptor(cola,cliente);

	pthread_mutex_unlock(&mutex_queue_mensajes);

	return;
}

void enviar_mensajes_cacheados_a_nuevo_suscriptor(t_cola_mensajes* cola,int cliente){

    t_list* mensajes = obtener_mensajes_de_cola(cola);

    void agregar_mensaje_pendiente(void* mensaje){

        agregar_pendiente_de_envio(cola,((t_mensaje_cola*)mensaje)->id_mensaje,cliente);		

	}	

	list_iterate(mensajes,agregar_mensaje_pendiente);	

	list_destroy(mensajes);

}

/* Flujo de ACK */


/* Genericas */
t_cola_mensajes* obtener_cola_mensajes(int cola_de_mensajes){

    bool filtro_cola(void* cola){
		return ((t_cola_mensajes*)cola)->cola_de_mensajes == cola_de_mensajes;
	}

	t_cola_mensajes* cola = list_find(cache_mensajes->colas, filtro_cola);

    return cola;
}

void agregar_pendiente_de_envio(t_cola_mensajes* cola, int id_mensaje, int cliente){
		
    t_envio_pendiente* envio_pendiente = (t_envio_pendiente*)malloc(sizeof(t_envio_pendiente));
    
    envio_pendiente->id = id_mensaje;
    envio_pendiente->cliente = cliente;

    printf("\n\n Agregado mensaje %d pendiente de envio a %d ",envio_pendiente->id,envio_pendiente->cliente);
    
    list_add(cola->envios_pendientes,(void*)envio_pendiente);

    sem_post(cola->producciones);

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

	return cola_mensaje;

}

t_list* obtener_mensajes_de_cola(t_cola_mensajes* cola){

   	bool filtro_mensajes(void* mensaje){
		return ((t_mensaje_cola*)mensaje)->cola_de_mensajes == cola->cola_de_mensajes;
	}

	t_list* mensajes = list_filter(cache_mensajes->mensajes, filtro_mensajes);

    return mensajes;

}

/* Validar */

void* print_operacion(void* mensaje){

	printf("\n\n Mensaje: ");
	printf("\n\n Cola: %d",((t_mensaje_cola*)mensaje)->cola_de_mensajes);

	return NULL;
}

void* sender_suscriptores(void* cola_mensajes){

	t_cola_mensajes* cola = (t_cola_mensajes*)cola_mensajes;

	t_mensaje_cola* mensaje;

	t_envio_pendiente* envio_pendiente;

	while(server_status != ENDING){

		sem_wait(cola->producciones);	

		pthread_mutex_lock(&mutex_queue_mensajes);	

		printf("\n\nLos pendientes tienen %d mensajes",(cola->envios_pendientes)->elements_count);
		
		envio_pendiente = list_get(cola->envios_pendientes,0);

		bool filtro_cola(void* mensaje){
			return ((t_mensaje_cola*)mensaje)->id_mensaje == envio_pendiente->id;
		}

		mensaje = list_find(cache_mensajes->mensajes,filtro_cola);

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

void eliminar_envio_pendiente(void* pendiente){
	free(pendiente);
	return;
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

