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

			pthread_create(&hilo_cliente,NULL,ObtenerMensaje,cliente);

			logger(escribir_loguear,l_trace,"\nSe aceptó un nuevo cliente");

		}
	
	}
	//return NULL;

}

void* ObtenerMensaje(void* cliente){

		int socket_cliente = (int*)cliente;

		//Creo un paquete y recibo el mensaje
		t_packed* paquete;
		paquete = recibir_mensaje(socket_cliente);

		//Esto me devuelve el paquete con todos los datos
		/* El nro de operacion y cola de mensajes indican el 
		tipo de estructura que contiene el paquete        */
		printf("operacion: %d \n",paquete->operacion);
		printf("cola_de_mensajes: %d \n",paquete->cola_de_mensajes);
		printf("id_correlacional: %d  \n",paquete->id_correlacional);
		printf("id_mensaje: %d \n",paquete->id_mensaje);
		printf("tamanio_payload: %d \n",paquete->tamanio_payload);

		/* Genero un puntero de ese tipo y lo inicializo */
		t_new_pokemon* pkmn;
		pkmn =(t_new_pokemon*)malloc(sizeof(t_new_pokemon));

		/* Apunto a los datos del mensaje */
		pkmn = paquete->mensaje;

		/*Libero la memoria del paquete*/
		eliminar_mensaje(paquete);

		/* Ya puedo usar mi copia de la estructura enviada*/
		printf("posx: %d \n",pkmn->coordenadas.posx);
		printf("posy: %d \n",pkmn->coordenadas.posy);
		printf("cantidad: %d \n",pkmn->cantidad);
		printf("pokemon: %s \n",pkmn->pokemon);
	
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
