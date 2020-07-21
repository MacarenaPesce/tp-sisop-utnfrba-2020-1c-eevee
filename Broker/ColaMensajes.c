#include "ColaMensajes.h"

extern t_cache_colas* cache_mensajes;

/* Flujo de mensajes */
int agregar_mensaje_a_cola(t_packed* paquete){	

	pthread_mutex_lock(&mutex_queue_mensajes);
	
	t_mensaje_cola* mensaje = crear_mensaje(paquete->cola_de_mensajes,
                                            paquete->id_correlacional,
											paquete->tamanio_payload,
                                            paquete->mensaje);

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

	t_cliente* cliente = crear_o_actualizar_cliente(id_cliente,socket_cliente,cola_de_mensajes);

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

	if(mensaje == NULL) {
		pthread_mutex_unlock(&mutex_queue_mensajes);
		return;
	};

	int* id_cliente_ptr = (int*)malloc(sizeof(int));

	memcpy(id_cliente_ptr,&id_cliente,sizeof(int));

	list_add(mensaje->suscriptores_ack,id_cliente_ptr);

	pthread_mutex_unlock(&mutex_queue_mensajes);
}


/* Flujo de sender */

void* sender_suscriptores(void* cola_mensajes){

	t_cola_mensajes* cola = (t_cola_mensajes*)cola_mensajes;

	t_envio_pendiente* envio_pendiente;

	while(1){

		sem_wait(cola->producciones);		

		pthread_mutex_lock(&mutex_server_status);

		if(server_status == ENDING){
			pthread_mutex_unlock(&mutex_server_status);
			break;
		} 

		pthread_mutex_unlock(&mutex_server_status);

		pthread_mutex_lock(&mutex_queue_mensajes);	

		if(debug_broker) log_debug(broker_logger,"Los pendientes tienen %d mensajes",(cola->envios_pendientes)->elements_count);
		
		envio_pendiente = list_get(cola->envios_pendientes,0);

		t_mensaje_cola * mensaje = obtener_mensaje_por_id(envio_pendiente->id);

		if(mensaje == NULL){
			if(debug_broker) log_debug(broker_logger,"El mensaje %d ya no se encuentra en la memoria y será descartado",envio_pendiente->id);
			eliminar_mensaje_enviado(cola);
			pthread_mutex_unlock(&mutex_queue_mensajes);
			continue;
		}

		t_cliente* cliente = obtener_cliente_por_id(envio_pendiente->cliente);
		
		t_socket_cliente* socket_cliente = obtener_socket_cliente_de_cola(cliente,cola->cola_de_mensajes);

		int envio = enviar_mensaje_a_suscriptor(envio_pendiente->id,
												mensaje->id_correlacional, 
												cola->cola_de_mensajes, 
												socket_cliente->socket, 
												mensaje->tamanio_mensaje,
												mensaje->mensaje);

		actualizar_lru_de_mensaje(mensaje);

		agregar_cliente_a_enviados(mensaje,cliente->id);

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

	if(debug_broker) log_debug(broker_logger, "Finalizando sender de la cola %d", cola->cola_de_mensajes);

	return NULL;
}

int enviar_mensaje_a_suscriptor(int id_mensaje,
								int id_correlacional, 
								enum COLA_DE_MENSAJES cola_de_mensajes, 
								int cliente, 
								int tamanio_mensaje,
								void* mensaje){
	int send_status = -1;

	t_packed* paquete = (t_packed*)malloc(sizeof(t_packed));

	paquete->id_mensaje = id_mensaje;
	paquete->id_correlacional = id_correlacional;
	paquete->id_cliente = -1;
	paquete->tamanio_payload = tamanio_mensaje;
	paquete->cola_de_mensajes = cola_de_mensajes;
	paquete->operacion = ENVIAR_MENSAJE;
	paquete->mensaje = mensaje;

	send_status = _enviar_mensaje(cliente,paquete);

	free(paquete);

	return send_status;
}

/* Genericas */
t_cliente* crear_o_actualizar_cliente(uint32_t id_cliente, int socket, int cola){

	t_cliente* cliente = obtener_cliente_por_id(id_cliente);

	if(cliente == NULL){
		
		if(debug_broker) log_debug(broker_logger,"Creando nuevo cliente %d para la cola %s en socket %d",id_cliente,obtener_nombre_cola(cola),socket);
		
		cliente = crear_cliente(id_cliente, socket, cola);
		
		agregar_cliente_a_cache(cliente);
		
		return cliente;
	}

	t_socket_cliente* socket_cliente = obtener_socket_cliente_de_cola(cliente,cola);

	if(socket_cliente == NULL){

		if(debug_broker) log_debug(broker_logger,"Agregado nuevo socket al cliente %d para la cola %s en socket %d",id_cliente,obtener_nombre_cola(cola),socket);

		agregar_socket_cliente(cliente,socket,cola);

		return cliente;
	}

	if(debug_broker) log_debug(broker_logger,"Actualizando socket del cliente %d para la cola %s al socket %d",id_cliente,obtener_nombre_cola(cola),socket);

	actualizar_socket_cliente(cliente,socket,cola);

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

t_cliente* crear_cliente(uint32_t id_cliente, int socket, int cola_de_mensajes){

	t_cliente* cliente = (t_cliente*)malloc(sizeof(t_cliente));
	t_socket_cliente* socket_cliente = crear_socket_cliente(socket,cola_de_mensajes);

	cliente->id = id_cliente;
	cliente->sockets = list_create();

	list_add(cliente->sockets,socket_cliente);

	return cliente;
}

t_socket_cliente* crear_socket_cliente(int socket, int cola_de_mensajes){

	t_socket_cliente* socket_cliente = (t_socket_cliente*)malloc(sizeof(t_socket_cliente));

	socket_cliente->socket = socket;
	socket_cliente->cola_de_mensajes = cola_de_mensajes;

	return socket_cliente;
}

t_mensaje_cola* crear_mensaje(int cola_de_mensajes, int id_correlacional, uint32_t tamanio_payload, void* _mensaje_recibido){

    t_mensaje_cola* mensaje = (t_mensaje_cola*)malloc(sizeof(t_mensaje_cola));
	void* mensaje_recibido = malloc(tamanio_payload);

	memcpy(mensaje_recibido,_mensaje_recibido,tamanio_payload);

	mensaje->id_mensaje = cache_mensajes->proximo_id_mensaje;
    cache_mensajes->proximo_id_mensaje++;
    
	mensaje->cola_de_mensajes = cola_de_mensajes;
	mensaje->id_correlacional = id_correlacional;
	mensaje->tamanio_mensaje = tamanio_payload;
	mensaje->suscriptores_enviados = list_create();
	mensaje->suscriptores_ack = list_create();
	mensaje->mensaje = mensaje_recibido;

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
  
    bool es_mensaje_buscado(void* _bloque){
		t_bloque_memoria* bloque = (t_bloque_memoria*) _bloque;

		if(bloque->esta_vacio) return false;

        return bloque->estructura_mensaje->id_mensaje == id_mensaje;
    }

    t_bloque_memoria* bloque = list_find(cache_mensajes->memoria,es_mensaje_buscado);

	return bloque != NULL ? bloque->estructura_mensaje : NULL;
}

t_cliente* obtener_cliente_por_id(int id_cliente){
  
    bool es_cliente_buscado(void* cliente){
        return ((t_cliente*)cliente)->id == id_cliente;
    }

    return list_find(cache_mensajes->clientes,es_cliente_buscado);
}

t_socket_cliente* obtener_socket_cliente_de_cola(t_cliente* cliente, int cola_de_mensajes){

	bool buscar_socket_cliente(void* _socket){

		t_socket_cliente* socket = (t_socket_cliente*) _socket;

		return socket->cola_de_mensajes == cola_de_mensajes;

	}

	return list_find(cliente->sockets, buscar_socket_cliente);
}

t_list* obtener_mensajes_de_cola(t_cola_mensajes* cola){

	t_list* mensajes = list_create();

   	void listar_mensajes(void* _bloque){

		t_bloque_memoria* bloque = (t_bloque_memoria*) _bloque;

		if(!bloque->esta_vacio){
			if(es_memoria_de_cola(bloque,cola)){
				list_add(mensajes,(void*) bloque->estructura_mensaje);
			}
		}	

	}		

	list_iterate((cache_mensajes->memoria),listar_mensajes);

    return mensajes;
}

t_list* obtener_memoria_de_cola(t_cola_mensajes* cola){

	bool filtro_mensajes(void* _bloque){

		t_bloque_memoria* bloque = (t_bloque_memoria*) _bloque;

		if(bloque->esta_vacio) return false;

		return (bloque->estructura_mensaje)->cola_de_mensajes == cola->cola_de_mensajes;
	}

	return list_filter(cache_mensajes->memoria, filtro_mensajes);
}

bool es_memoria_de_cola(t_bloque_memoria* bloque, t_cola_mensajes* cola){
	return bloque->estructura_mensaje->cola_de_mensajes == cola->cola_de_mensajes;
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

	asignar_particion_memoria(mensaje);
}

void agregar_cliente_a_cache(t_cliente* cliente){
	list_add(cache_mensajes->clientes,(void*)cliente);
}

void agregar_socket_cliente(t_cliente* cliente, int socket, int cola_de_mensajes){

	t_socket_cliente* socket_cliente = crear_socket_cliente(socket,cola_de_mensajes);

	list_add(cliente->sockets, (void*) socket_cliente);
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

void agregar_cliente_a_enviados(t_mensaje_cola* mensaje, int _id_cliente){

	int* id_cliente = (int*)malloc(sizeof(int));

	memcpy(id_cliente,&_id_cliente,sizeof(int));

	list_add(mensaje->suscriptores_enviados,(void*)id_cliente);
}

void actualizar_socket_cliente(t_cliente* cliente, int socket, int cola){

	t_socket_cliente* socket_cliente = obtener_socket_cliente_de_cola(cliente,cola);

	socket_cliente->socket = socket;
}

void eliminar_mensaje_enviado(t_cola_mensajes* cola){
    list_remove_and_destroy_element(cola->envios_pendientes,0,eliminar_envio_pendiente);
}

void eliminar_envio_pendiente(void* pendiente){
	free(pendiente);
	return;
}

void eliminar_mensaje_cola(t_mensaje_cola* mensaje){

	list_destroy_and_destroy_elements(mensaje->suscriptores_enviados,free);

	list_destroy_and_destroy_elements(mensaje->suscriptores_ack,free);	

	free(mensaje);
	
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