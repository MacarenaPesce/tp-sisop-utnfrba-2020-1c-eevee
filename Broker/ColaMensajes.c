#include "ColaMensajes.h"

extern t_cache_colas* cache_mensajes;

/* Flujo de mensajes */
int agregar_mensaje_a_cola(t_packed* paquete){	

	t_mensaje_cola* mensaje = crear_mensaje(paquete->cola_de_mensajes,
                                            paquete->id_correlacional,
                                            paquete->mensaje);

	pthread_mutex_lock(&mutex_queue_mensajes);

    agregar_mensaje_a_cache(mensaje);

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
void agregar_suscriptor_a_cola(int cola_de_mensajes, uint32_t id_cliente, int socket_cliente){

	if(debug_broker) log_debug(broker_logger, "Recibida suscripcion de cliente %d en socket %d", id_cliente,socket_cliente);
	
	pthread_mutex_lock(&mutex_queue_mensajes);

	t_cola_mensajes* cola = obtener_cola_mensajes(cola_de_mensajes);

	t_cliente* cliente = crear_o_actualizar_cliente(id_cliente,socket_cliente);

	agregar_cliente_a_suscriptores(cola,cliente);

	enviar_mensajes_cacheados_a_suscriptor(cola,cliente->id);

	pthread_mutex_unlock(&mutex_queue_mensajes);

	return;
}

void enviar_mensajes_cacheados_a_suscriptor(t_cola_mensajes* cola,int cliente){

    t_list* mensajes = obtener_mensajes_de_cola(cola);

    void agregar_mensaje_pendiente(void* mensaje){

		if(!ack_recibido_de(mensaje,cliente)){
        	agregar_pendiente_de_envio(cola,((t_mensaje_cola*)mensaje)->id_mensaje,cliente);		
		}

	}	

	list_iterate(mensajes,agregar_mensaje_pendiente);	

	list_destroy(mensajes);

}

/* Flujo de ACK */

void agregar_ack_a_mensaje(uint32_t id_mensaje, uint32_t id_cliente, int socket_cliente){

	log_info(broker_logger, "El cliente %d envió confirmación del mensaje %d a través del socket %d ",id_cliente, id_mensaje, socket_cliente);

	pthread_mutex_lock(&mutex_queue_mensajes);

	t_mensaje_cola* mensaje = obtener_mensaje_por_id(id_mensaje);

	int* id_cliente_ptr = (int*)malloc(sizeof(int));

	*id_cliente_ptr = id_cliente;

	list_add(mensaje->suscriptores_ack,id_cliente_ptr);

	pthread_mutex_unlock(&mutex_queue_mensajes);

}

/* Flujo de sender */

void* sender_suscriptores(void* cola_mensajes){

	t_cola_mensajes* cola = (t_cola_mensajes*)cola_mensajes;

	t_envio_pendiente* envio_pendiente;

	while(server_status != ENDING){

		sem_wait(cola->producciones);	

		pthread_mutex_lock(&mutex_queue_mensajes);	

		if(debug_broker) log_debug(broker_logger,"Los pendientes tienen %d mensajes",(cola->envios_pendientes)->elements_count);
		
		envio_pendiente = list_get(cola->envios_pendientes,0);

		t_mensaje_cola * mensaje = obtener_mensaje_por_id(envio_pendiente->id);

		t_cliente* cliente = obtener_cliente_por_id(envio_pendiente->cliente);

		int envio = enviar_mensaje_a_suscriptor(envio_pendiente->id,
												mensaje->id_correlacional, 
												cola->cola_de_mensajes, 
												cliente->socket, 
												mensaje->mensaje);


		agregar_cliente_a_enviados(mensaje,cliente);

		if(envio != -1){			
			log_info(broker_logger, "Se envió mensaje %d perteneciente a la cola %d al cliente %d", envio_pendiente->id,cola->cola_de_mensajes,envio_pendiente->cliente);
			eliminar_mensaje_enviado(cola);
            if(debug_broker) log_debug(broker_logger,"Los pendientes quedaron con %d mensajes",(cola->envios_pendientes)->elements_count);
        }else{
			log_error(broker_logger,"Falló el envío del mensaje %d perteneciente a la cola %d al cliente %d. Se reintentará cuando se vuelva a suscribir",envio_pendiente->id,cola->cola_de_mensajes,envio_pendiente->cliente);
            eliminar_mensaje_enviado(cola);
		}

		pthread_mutex_unlock(&mutex_queue_mensajes);
	
	}

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
			send_status = distribuir_catch_pokemon(cliente,id_mensaje,id_correlacional,-1,(t_catch_pokemon*)mensaje);
			break;
 
		case COLA_APPEARED_POKEMON:
			send_status = distribuir_appeared_pokemon(cliente,id_mensaje,id_correlacional,-1,(t_appeared_pokemon*)mensaje);
			break;

		case COLA_NEW_POKEMON:
			send_status = distribuir_new_pokemon(cliente,id_mensaje,id_correlacional,-1,(t_new_pokemon*)mensaje);
			break;
		
		case COLA_CAUGHT_POKEMON:
			send_status = distribuir_caught_pokemon(cliente,id_mensaje,id_correlacional,-1,(t_caught_pokemon*)mensaje);
			break;
		
		case COLA_GET_POKEMON:
			send_status = distribuir_get_pokemon(cliente,id_mensaje,id_correlacional,-1,(t_get_pokemon*)mensaje);
			break;

		case COLA_LOCALIZED_POKEMON:
			//send_status = enviar_localized_pokemon(cliente,id_mensaje,id_correlacional,-1,(t_localized_pokemon*)mensaje);
			break;			

		default:
			log_error(broker_logger,"Error, cola de mensajes desconocida: %d\n",cola_de_mensajes);
			break;
	}

	return send_status;

}

/* Genericas */
t_cliente* crear_o_actualizar_cliente(uint32_t id_cliente, int socket){

	t_cliente* cliente = obtener_cliente_por_id(id_cliente);

	if(cliente == NULL){
		if(debug_broker) log_debug(broker_logger,"\nCreando nuevo cliente %d en socket %d\n",id_cliente,socket);
		cliente = crear_cliente(id_cliente, socket);
		agregar_cliente_a_cache(cliente);
		return cliente;
	}

	if(debug_broker) log_debug(broker_logger,"Actualizando socket del cliente %d al socket %d",id_cliente,socket);

	cliente->socket = socket;

	return cliente;
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

t_cliente* crear_cliente(uint32_t id_cliente, int socket){

	t_cliente* cliente = (t_cliente*)malloc(sizeof(t_cliente));

	cliente->id = id_cliente;
	cliente->socket = socket;

	return cliente;

}

t_mensaje_cola* crear_mensaje(int cola_de_mensajes, int id_correlacional, void* mensaje_recibido){

    t_mensaje_cola* mensaje;
	mensaje = (t_mensaje_cola*)malloc(sizeof(t_mensaje_cola));

	mensaje->id_mensaje = cache_mensajes->proximo_id_mensaje;
    cache_mensajes->proximo_id_mensaje++;
    
	mensaje->cola_de_mensajes = cola_de_mensajes;
	mensaje->id_correlacional = id_correlacional;
	mensaje->mensaje = mensaje_recibido;
	mensaje->suscriptores_enviados = list_create();
	mensaje->suscriptores_ack = list_create();

    return mensaje;
}

t_cola_mensajes* obtener_cola_mensajes(int cola_de_mensajes){

    bool filtro_cola(void* cola){
		return ((t_cola_mensajes*)cola)->cola_de_mensajes == cola_de_mensajes;
	}

	t_cola_mensajes* cola = list_find(cache_mensajes->colas, filtro_cola);

    return cola;
}

t_mensaje_cola* obtener_mensaje_por_id(int id_mensaje){
  
    bool es_mensaje_buscado(void* mensaje){
        return ((t_mensaje_cola*)mensaje)->id_mensaje == id_mensaje;
    }

    return list_find(cache_mensajes->mensajes,es_mensaje_buscado);

}

t_cliente* obtener_cliente_por_id(int id_cliente){
  
    bool es_cliente_buscado(void* cliente){
        return ((t_cliente*)cliente)->id == id_cliente;
    }

    return list_find(cache_mensajes->clientes,es_cliente_buscado);

}

t_list* obtener_mensajes_de_cola(t_cola_mensajes* cola){

   	bool filtro_mensajes(void* mensaje){
		return ((t_mensaje_cola*)mensaje)->cola_de_mensajes == cola->cola_de_mensajes;
	}

	t_list* mensajes = list_filter(cache_mensajes->mensajes, filtro_mensajes);

    return mensajes;

}

bool es_suscriptor_de(int id_cliente, t_cola_mensajes* cola){
  
    bool es_cliente_buscado(void* cliente){
        return ((t_cliente*)cliente)->id == id_cliente;
    }

	t_cliente* cliente = list_find(cola->suscriptores,es_cliente_buscado);

    return cliente != NULL;

}

bool ack_recibido_de(t_mensaje_cola* mensaje, int id_cliente){

	bool es_cliente_buscado(void* cliente){
        return *((int*)cliente) == id_cliente;
    }

	t_cliente* cliente = list_find(mensaje->suscriptores_ack,es_cliente_buscado);

    return cliente != NULL;
}

void agregar_mensaje_a_cache(t_mensaje_cola* mensaje){
    list_add(cache_mensajes->mensajes,(void*)mensaje);
}

void agregar_cliente_a_cache(t_cliente* cliente){
	list_add(cache_mensajes->clientes,(void*)cliente);
}

void agregar_cliente_a_suscriptores(t_cola_mensajes* cola, t_cliente* cliente){

	if(es_suscriptor_de(cliente->id, cola)) return;

    int* id_cliente = (int*)malloc(sizeof(int));

	*id_cliente = cliente->id;

    list_add(cola->suscriptores,(void*)id_cliente);

}

void agregar_pendiente_de_envio(t_cola_mensajes* cola, int id_mensaje, int id_cliente){
		
    t_envio_pendiente* envio_pendiente = (t_envio_pendiente*)malloc(sizeof(t_envio_pendiente));
    
    envio_pendiente->id = id_mensaje;
    envio_pendiente->cliente = id_cliente;

    if(debug_broker) log_debug(broker_logger,"Agregado mensaje %d pendiente de envio a %d",envio_pendiente->id,envio_pendiente->cliente);
    
    list_add(cola->envios_pendientes,(void*)envio_pendiente);

    sem_post(cola->producciones);

}

void agregar_cliente_a_enviados(t_mensaje_cola* mensaje, t_cliente* cliente){

	list_add(mensaje->suscriptores_enviados,(void*)&cliente->id);

}

void eliminar_mensaje_enviado(t_cola_mensajes* cola){
    list_remove_and_destroy_element(cola->envios_pendientes,0,eliminar_envio_pendiente);
}

void eliminar_envio_pendiente(void* pendiente){
	free(pendiente);
	return;
}

void* print_operacion(void* mensaje){

	if(debug_broker) log_debug(broker_logger,"Cola: %d",((t_mensaje_cola*)mensaje)->cola_de_mensajes);

	return NULL;
}

void* print_clientes(void* suscriptor){
	
	if(debug_broker) log_debug(broker_logger,"id_cliente: %d",*((int*)suscriptor) );

	return NULL;
}