#include "Broker.h"


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
	
	pthread_create(&hilo_sockets,NULL,esperar_cliente,socket_servidor);
}

void* esperar_cliente(void* socket_servidor){
	while(1){
		int* int_socket_servidor = (int*) socket_servidor;

		struct sockaddr_in dir_cliente;

		int tam_direccion = sizeof(struct sockaddr_in);

		int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

		//Iniciar un hilo por cliente
		if(socket_cliente != -1){

			pthread_t hilo_cliente;

			void *cliente = (void*)malloc(sizeof(int));

			cliente = socket_cliente;

			pthread_create(&hilo_cliente,NULL,esperar_mensajes,cliente);

			logger(escribir_loguear,l_trace,"\nSe aceptó un nuevo cliente");

		}
	
	}
	//return NULL;

}

void* esperar_mensajes(void* cliente){

	int socket_cliente = (int*)cliente;

	while(1){
		//Creo un paquete y recibo el mensaje
		t_packed* paquete;
		paquete = recibir_mensaje(socket_cliente);

		if(paquete != -1){
			//Esto me devuelve el paquete con todos los datos
			/* El nro de operacion y cola de mensajes indican el 
			tipo de estructura que contiene el paquete */

			printf("\n\nMensaje Recibido: %d \n",paquete->operacion);
			printf("operacion: %d \n",paquete->operacion);
			printf("cola_de_mensajes: %d \n",paquete->cola_de_mensajes);
			printf("id_correlacional: %d  \n",paquete->id_correlacional);
			printf("id_mensaje: %d \n",paquete->id_mensaje);
			printf("tamanio_payload: %d \n",paquete->tamanio_payload);

			switch(paquete->operacion){
				case ENVIAR_MENSAJE:
					agregar_mensaje_a_queue(paquete,socket_cliente);
					break;
				
				case SUSCRIBIRSE_A_COLA:
					recibir_solicitud_suscripcion(paquete);
					break;

				case ACK:
					recibir_ack(paquete);
					break;
				
				default:
					printf("Error, operacion desconocida: %d\n",paquete->operacion);
					break;
			}

		}
		
	}
	
}

void agregar_mensaje_a_queue(t_packed *paquete,int socket_cliente){
	enviar_ack(socket_cliente,123,-1);
	
	switch(paquete->cola_de_mensajes){

		case COLA_NEW_POKEMON:;
			/* Genero un puntero de ese tipo y lo inicializo */
			t_new_pokemon* pkmn;
			pkmn =(t_new_pokemon*)malloc(sizeof(t_new_pokemon));

			/* Apunto a los datos del mensaje */
			pkmn = paquete->mensaje;

			/* Ya puedo usar mi copia de la estructura enviada*/
			if(paquete->tamanio_payload > 0){
				printf("posx: %d \n",pkmn->coordenadas.posx);
				printf("posy: %d \n",pkmn->coordenadas.posy);
				printf("cantidad: %d \n",pkmn->cantidad);
				printf("pokemon: %s \n",pkmn->pokemon);
			}

			break;

	}

	/*Libero la memoria del paquete*/
	eliminar_mensaje(paquete);
	
}

void recibir_solicitud_suscripcion(void *paquete,int socket_cliente){
	enviar_ack(socket_cliente,123,-1);
}

void recibir_ack(void *paquete,int socket_cliente){
	//enviar_ack(socket_cliente,123,-1);
}

int main(){

	inicializar_logger();
	inicializar_archivo_de_configuracion();
	configurar_signals();

	iniciar_servidor();

	/* TODO: Obtener este valor desde la config */
	int tamanio_memoria_inicial = 8000;

	//primer_bloque = AsignarMemoriaInicial(tamanio_memoria_inicial,lista_memoria);
	while(true){
	
	}
}
