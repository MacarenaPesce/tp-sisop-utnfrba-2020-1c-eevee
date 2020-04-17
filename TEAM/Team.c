/*
 * Team.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Team.h"

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

	t_header header = recibir_header(socket_cliente);
	int status_header = header.tamanio;

	if(header.tipo_de_mensaje == CHAR_MESSAGE){
		recibir_mensaje_de_texto(socket_cliente, header.tamanio);
	}

}


int main(){

	inicializar_logger();
	inicializar_archivo_de_configuracion();
	configurar_signals();

	iniciar_servidor();

}
