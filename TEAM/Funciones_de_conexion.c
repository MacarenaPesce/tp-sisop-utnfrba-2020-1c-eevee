/*
 * Funciones_de_conexion.c
 *
 *  Created on: 2 jun. 2020
 *      Author: utnso
 */

#include "Funciones_de_conexion.h"

int crear_listen_socket(char * puerto){
	struct sockaddr_in dir_sock;

	//Convierto el string a INT para htons
	unsigned int puerto_i = atoi(puerto);

	dir_sock.sin_family = AF_INET;
	dir_sock.sin_addr.s_addr = INADDR_ANY;
	dir_sock.sin_port = htons(puerto_i);

	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0){
		return -1;
	}

	int activado = 1;
	setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR, &activado, sizeof(activado));

	if(bind(server_socket, (void*)&dir_sock, sizeof(dir_sock)) != 0){
		return -1;
	}

	listen(server_socket, SOMAXCONN);

	return server_socket;

}

int iniciar_servidor(){
	int server_socket = crear_listen_socket(PUERTO);

	if(server_socket < 0){
		log_info(team_logger, "\nFalló la creación del socket servidor");
		terminar_team_correctamente();
		exit(1);
	}
	else{
		log_info(team_logger, "Team esta listo para recibir mensajes de gameboy en el socket servidor (%d)\n", server_socket);
	}

	return server_socket;
}
