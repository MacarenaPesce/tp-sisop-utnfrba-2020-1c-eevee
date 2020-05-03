/*
 * Broker.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Broker.h"

void recibir_mensaje_de_texto(int cliente, int tamanio){
	logger(escribir_loguear,l_info,"Voy a recibir un mensaje de texto");
	tp_mensaje_char contenido_del_paquete = recibir_mensaje_char(tamanio, cliente);
	logger(escribir_loguear,l_info,"Esto es lo que recibi: %s", contenido_del_paquete->mensaje);
	free(contenido_del_paquete->mensaje);
	free(contenido_del_paquete);
}

void iniciar_servidor(void){
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP, PUERTO, &hints, &servinfo);

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

    while(1)
    	esperar_cliente(socket_servidor);
}

void esperar_cliente(int socket_servidor){
	struct sockaddr_in dir_cliente;
	int tam_direccion = sizeof(struct sockaddr_in);
	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
	logger(escribir_loguear,l_trace,"\nSe aceptó un nuevo cliente");

	//PARA HACER QUE EL SERVIDOR SEA MULTIHILO, A PARTIR DE aca tendriamos que crear un hilo por cliente, y hacer que derive las operaciones
	//o sea:
	//pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	//pthread_detach(thread);
	//y cerrar el corchete. Luego la funcion que recibe el pthread_create usa lo siguiente:

	/*
	 * void serve_client(int* socket){
			int cod_op;
			if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
				cod_op = -1;
			process_request(cod_op, *socket);
	   }

		void process_request(int cod_op, int cliente_fd) {
			int size;
			void* msg;
				switch (cod_op) {
				case MENSAJE:
					msg = recibir_mensaje(cliente_fd, &size);
					devolver_mensaje(msg, size, cliente_fd);
					free(msg);
					break;
				case 0:
					pthread_exit(NULL);
				case -1:
					pthread_exit(NULL);
				}
		}

		NOSOTROS LO TENEMOS QUE ACOPLAR A NUESTRO PROTOCOLO!!
	 */


	//Por ahora queda en un loop esperando que le lleguen cosas, no es multihilo, eso esta claro.
	//Todavia queda definir si team y gamecard seran servers multihilo o no. Necesitamos definirlo urgente, porque meter el select
	//despues es una fiaca. Tambien tenemos que ver si el mensaje lo ingresamos por consola o no. Ahora a team le llega algo hardcodeado.
	t_header header = recibir_header(socket_cliente);
	if(header.tipo_de_mensaje == CHAR_MESSAGE){
		recibir_mensaje_de_texto(socket_cliente, header.tamanio);
	}

}


int main(){

	inicializar_logger();
	inicializar_archivo_de_configuracion();
	configurar_signals();

	iniciar_servidor();

	/* TODO: Obtener este valor desde la config */
	int tamanio_memoria_inicial = 8000;

	primer_bloque = AsignarMemoriaInicial(tamanio_memoria_inicial,lista_memoria);

}
