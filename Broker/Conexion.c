#include "Conexion.h"

extern enum SERVER_STATUS server_status;

/* Setup de servidor */

void iniciar_servidor(void){

	if(debug_broker) log_debug(broker_logger, "7) Iniciando servidor...", NULL);
	
	int socket_servidor;
	int bind_status;
	
    struct addrinfo hints, *servinfo, *p; /* REVISAR DE LIBERAR AL FINAL DEL SERVER  */

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo("127.0.0.1", "5003", &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next){
		if(debug_broker) log_debug(broker_logger, "8) Intentando bindear sever...", NULL);

        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			log_error(broker_logger, "Error en socket broker", NULL);
            continue;
		}

		int flag = 1;  
		if (-1 == setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag))) {  
			log_error(broker_logger, "Error al setear las opciones del socket", NULL);
		}  

        if ((bind_status = bind(socket_servidor, p->ai_addr, p->ai_addrlen)) == -1) {
			log_error(broker_logger, "Error en bind broker: %d", bind_status);
			log_error(broker_logger, "Código de error: %d", errno);
            close(socket_servidor);
            continue;
        }
		if(debug_broker) log_debug(broker_logger, "9) Server bindeado correctamente!!!", NULL);
        break;
    }

	listen(socket_servidor, SOMAXCONN);

	int * socket = (int*)malloc(sizeof(int));

	memcpy(socket,&socket_servidor,sizeof(int));

	esperar_cliente((void*)socket);
}

void* esperar_cliente(void* socket){

	pthread_mutex_lock(&mutex_server_status);
	server_status = RUNNING;
	pthread_mutex_unlock(&mutex_server_status);

	if(debug_broker) log_debug(broker_logger, "10) Hilo del servidor cargado correctamente!!!", NULL);

	while(server_status != ENDING){

		struct sockaddr_in dir_cliente;

		int socket_servidor = *((int*)socket);

		int tam_direccion = sizeof(struct sockaddr_in);

		int socket_cliente = accept(socket_servidor, (void*) &dir_cliente,(socklen_t*) &tam_direccion);

		int* soc_cliente = (int*)malloc(sizeof(int));

		memcpy(soc_cliente,&socket_cliente,sizeof(int));

		//Iniciar un hilo por cliente
		if(socket_cliente != -1){

			pthread_t hilo_cliente;

			pthread_create(&hilo_cliente,NULL,esperar_mensajes,(void *)soc_cliente);

			pthread_join(hilo_cliente,NULL);
			
		}
	}
	
	return NULL;
}

void* esperar_mensajes(void* cliente){

	pthread_mutex_lock(&mutex_recepcion_mensajes);
	int socket_cliente = *((int*)cliente);

	//Creo un paquete y recibo el mensaje
	t_packed* paquete;

	while(1){

		paquete = recibir_mensaje_serializado(socket_cliente);

		if(paquete > (t_packed*)0){
			 
			t_produccion_paquete* produccion = (t_produccion_paquete*)malloc(sizeof(t_produccion_paquete));
			produccion->socket = socket_cliente;
			produccion->paquete = paquete;

			list_add(paquetes_pendientes,(void*)produccion);

			sem_post(&transaccionar_paquetes_pendientes);			

			break;
		}
	}

	pthread_mutex_unlock(&mutex_recepcion_mensajes);

	free(cliente);

	return NULL;
}

/* Recepcion de mensajes */

void* transaccionar_paquete_recibido(){

	while(1){

		sem_wait(&transaccionar_paquetes_pendientes);

		pthread_mutex_lock(&mutex_server_status);
		if(server_status == ENDING){
			pthread_mutex_unlock(&mutex_server_status);
			break;
		} 
		pthread_mutex_unlock(&mutex_server_status);

		pthread_mutex_lock(&mutex_recepcion_mensajes);
		t_produccion_paquete* produccion = list_remove(paquetes_pendientes,0);
		pthread_mutex_unlock(&mutex_recepcion_mensajes);

		if(produccion == NULL){
			continue;
		}
		
		t_packed* paquete = produccion->paquete;
		int socket_cliente = produccion->socket;

		free(produccion);

	 if(debug_broker) {
			log_debug(broker_logger,"Mensaje Recibido en el socket %d:\noperacion: %d \ncola_de_mensajes: %s \nid_correlacional: %d \nid_mensaje: %d \nid_cliente: %d \ntamanio_payload: %d",
				socket_cliente,
				paquete->operacion,
				obtener_nombre_cola(paquete->cola_de_mensajes),
				paquete->id_correlacional,
				paquete->id_mensaje,
				paquete->id_cliente,
				paquete->tamanio_payload
			);
		}; 

		switch(paquete->operacion){
			case ENVIAR_MENSAJE:
				recibir_mensaje_de_colas(paquete,socket_cliente);
				break;
			
			case SUSCRIBIRSE_A_COLA:
				recibir_solicitud_suscripcion(paquete,socket_cliente);
				break;

			case ACK:
				if(paquete->id_mensaje != -1){
					recibir_ack(paquete,socket_cliente);
				}
				break;
			
			default:
				log_error(broker_logger, "Error, operacion desconocida: %d", paquete->operacion);
				break;
		}

	}	

	return NULL;

}

void recibir_mensaje_de_colas(t_packed* paquete,int socket_cliente){

	log_info(broker_logger, "El cliente %d envió un mensaje para la cola %d a través de socket %d", paquete->id_cliente, paquete->cola_de_mensajes,socket_cliente);
	int id_mensaje = agregar_mensaje_a_cola(paquete);
	
	distribuir_ack(socket_cliente,id_mensaje,-1);

	_eliminar_mensaje(paquete);

	return;

}

void recibir_solicitud_suscripcion(t_packed *paquete,int socket_cliente){

	log_info(broker_logger, "El cliente %d solicitó suscribirse a la cola %d a través del socket %d", paquete->id_cliente, paquete->cola_de_mensajes, socket_cliente);

	agregar_suscriptor_a_cola(paquete->cola_de_mensajes, paquete->id_cliente, socket_cliente);
	
	distribuir_ack(socket_cliente,-1,-1);

	eliminar_mensaje(paquete);
    
	return;
}

void recibir_ack(t_packed *paquete,int socket_cliente){

	log_info(broker_logger, "El cliente %d envió confirmación del mensaje %d a través del socket %d ",paquete->id_cliente, paquete->id_mensaje, socket_cliente);

	agregar_ack_a_mensaje(paquete->id_mensaje, paquete->id_cliente);

	eliminar_mensaje(paquete);

}
