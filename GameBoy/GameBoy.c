/*
 * GameBoy.c
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#include "GameBoy.h"

int conectar_a_server(char* ip, char* puerto){
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;	 // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;  // Indica que usaremos el protocolo TCP
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info); // Carga en server_info los datos de la conexion

	int new_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	if(new_socket < 0){
		return -1;
	}

	int activado = 1;
	setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	int res_connect = connect(new_socket, server_info->ai_addr, server_info->ai_addrlen);
	if(res_connect < 0){
		freeaddrinfo(server_info);
		close(new_socket);
		return -1;
	}

	freeaddrinfo(server_info);
	return new_socket;

}


int main(){

	inicializar_logger();
	inicializar_archivo_de_configuracion();
	configurar_signals();

	logger(escribir_loguear,l_info,"\nListo para enviar algo");

	int server_team = conectar_a_server(ip_team, puerto_team);
	enviar_mensaje_char("HOla team", server_team);

	logger(escribir_loguear,l_info,"\nYa lo envie");

}
