#include "Conexion.h"

extern t_list* cola_mensajes;

//pthread_mutex_t mutex_queue_mensajes;

void* print_operacion(t_mensaje_cola* mensaje){

	printf("\n\n Mensaje: ");
	printf("\n\n Cola: %d",mensaje->cola_de_mensajes);

	return;
}

void iniciar_servidor(void){
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo("127.0.0.1", "6009", &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next){
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

	pthread_t hilo_sockets;
	
	pthread_create(&hilo_sockets,NULL,esperar_cliente,(void*)socket_servidor);
}

void* esperar_cliente(void* socket_servidor){
	while(1){

		struct sockaddr_in dir_cliente;

		int tam_direccion = sizeof(struct sockaddr_in);

		int socket_cliente = accept((int)socket_servidor, (void*) &dir_cliente,(socklen_t*) &tam_direccion);

		//Iniciar un hilo por cliente
		if(socket_cliente != -1){

			pthread_t hilo_cliente;

			void *cliente = (void*)malloc(sizeof(int));

			cliente = &socket_cliente;

			pthread_create(&hilo_cliente,NULL,esperar_mensajes,cliente);

			printf("\nSe aceptó un nuevo cliente");

		}
	
	}
	//return NULL;

}

void* esperar_mensajes(void* cliente){

	int socket_cliente = *(int*)cliente;

	//Creo un paquete y recibo el mensaje
	t_packed* paquete;
	paquete = recibir_mensaje(socket_cliente);

	if(paquete != (t_packed*)-1){
		//Esto me devuelve el paquete con todos los datos
		/* El nro de operacion y cola de mensajes indican el 
		tipo de estructura que contiene el paquete */

		{
			printf("\n\nMensaje Recibido: %d \n",paquete->operacion);
			printf("operacion: %d \n",paquete->operacion);
			printf("cola_de_mensajes: %d \n",paquete->cola_de_mensajes);
			printf("id_correlacional: %d  \n",paquete->id_correlacional);
			printf("id_mensaje: %d \n",paquete->id_mensaje);
			printf("tamanio_payload: %d \n",paquete->tamanio_payload);
		}

		switch(paquete->operacion){
			case ENVIAR_MENSAJE:
				recibir_mensaje_de_colas(paquete,socket_cliente);
				break;
			
			case SUSCRIBIRSE_A_COLA:
				recibir_solicitud_suscripcion(paquete,socket_cliente);
				break;

			case ACK:
				recibir_ack(paquete,socket_cliente);
				break;
			
			default:
				printf("Error, operacion desconocida: %d\n",paquete->operacion);
				break;
		}

	}

	return NULL;
	
}

void recibir_mensaje_de_colas(t_packed* paquete,int socket_cliente){

	int id_mensaje = agregar_mensaje_a_cola(paquete);

	list_iterate(cola_mensajes,print_operacion);
	
	enviar_ack(socket_cliente,id_mensaje,-1);

	return;

}

void recibir_solicitud_suscripcion(void *paquete,int socket_cliente){
	//agregar suscriptor
	enviar_ack(socket_cliente,-1,-1);

	return;
}

void recibir_ack(void *paquete,int socket_cliente){
	//agregar ack a mensaje
	return;
}

int agregar_mensaje_a_cola(t_packed* paquete){	

	t_mensaje_cola* mensaje;
	mensaje = (t_mensaje_cola*)malloc(sizeof(t_mensaje_cola));

	mensaje->cola_de_mensajes = paquete->cola_de_mensajes;
	mensaje->id_correlacional = paquete->id_correlacional;
	mensaje->mensaje = paquete->mensaje;

	pthread_mutex_lock(&mutex_queue_mensajes);

	int id_mensaje = list_add(cola_mensajes,(void*)mensaje);

	pthread_mutex_unlock(&mutex_queue_mensajes);

	return id_mensaje;

}

