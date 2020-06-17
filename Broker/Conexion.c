#include "Conexion.h"

extern t_cache_colas* cache_mensajes;
extern enum SERVER_STATUS server_status; 

/* Setup de servidor */

void iniciar_servidor(void){

	printf("\n\n7) Iniciando servidor...");
	
	int socket_servidor;
	int bind_status;
	
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo("127.0.0.1", "32587", &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next){
		printf("\n\n8) Intentando bindear sever...");

        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			printf("\n\nError en socket broker");
            continue;
		}

		int flag = 1;  
		if (-1 == setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag))) {  
			printf("setsockopt fail");  
		}  

        if ((bind_status = bind(socket_servidor, p->ai_addr, p->ai_addrlen)) == -1) {
			printf("\n\nError en bind broker: %d",bind_status);
			printf("\nCodigo de error: %d \n", errno);
            close(socket_servidor);
            continue;
        }
		printf("\n\n9) Server bindeado correctamente!!!");
        break;
    }

	listen(socket_servidor, SOMAXCONN);

	int * socket = (int*)malloc(sizeof(int));

	memcpy(socket,&socket_servidor,sizeof(int));

	pthread_t hilo_sockets;

	pthread_create(&hilo_sockets,NULL,esperar_cliente,(void*)socket);
	pthread_detach(hilo_sockets);
	
}

void* esperar_cliente(void* socket){

	printf("\n\n10) Hilo del servidor cargado correctamente!!!");

	while(1){

		struct sockaddr_in dir_cliente;

		int socket_servidor = *((int*)socket);

		int tam_direccion = sizeof(struct sockaddr_in);

		int socket_cliente = accept(socket_servidor, (void*) &dir_cliente,(socklen_t*) &tam_direccion);

		//Iniciar un hilo por cliente
		if(socket_cliente != -1){

			pthread_t hilo_cliente;

			void *cliente = (void*)malloc(sizeof(int));

			cliente = &socket_cliente;

			pthread_create(&hilo_cliente,NULL,esperar_mensajes,cliente);

			pthread_join(hilo_cliente,NULL);

		}
	
	}
	
	return NULL;

}

void* esperar_mensajes(void* cliente){

	printf("\nSe aceptó un nuevo cliente\n");

	int socket_cliente = *(int*)cliente;

	//Creo un paquete y recibo el mensaje
	t_packed* paquete;

	while(1){
		paquete = recibir_mensaje(socket_cliente);

		if(paquete != (t_packed*)-1){
			//Esto me devuelve el paquete con todos los datos
			/* El nro de operacion y cola de mensajes indican el 
			tipo de estructura que contiene el paquete */

			
		/* 	printf("\n\nMensaje Recibido: %d \n",paquete->operacion);
			printf("operacion: %d \n",paquete->operacion);
			printf("cola_de_mensajes: %d \n",paquete->cola_de_mensajes);
			printf("id_correlacional: %d  \n",paquete->id_correlacional);
			printf("id_mensaje: %d \n",paquete->id_mensaje);
			printf("id_cliente: %d \n",paquete->id_cliente);
			printf("tamanio_payload: %d \n",paquete->tamanio_payload); */		

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
					printf("Error, operacion desconocida: %d\n",paquete->operacion);
					break;
			}

			break;
		}

	}

	return NULL;
	
}

/* Recepcion de mensajes */

void recibir_mensaje_de_colas(t_packed* paquete,int socket_cliente){

	printf("\nSe recibió un mensaje para la cola %d\n",paquete->cola_de_mensajes);

	int id_mensaje = agregar_mensaje_a_cola(paquete);
	
	int send_status = distribuir_ack(socket_cliente,id_mensaje,-1);

	free(paquete);

	return;

}

void recibir_solicitud_suscripcion(t_packed *paquete,int socket_cliente){

	agregar_suscriptor_a_cola(paquete->cola_de_mensajes, paquete->id_cliente, socket_cliente);
	distribuir_ack(socket_cliente,-1,-1);
    
	return;
}

void recibir_ack(t_packed *paquete,int socket_cliente){

	agregar_ack_a_mensaje(paquete->id_mensaje, paquete->id_cliente, socket_cliente);

}