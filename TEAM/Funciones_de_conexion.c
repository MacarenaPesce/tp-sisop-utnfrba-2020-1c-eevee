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
		log_info(team_logger, "\nSocket servidor (%d) escuchando", server_socket);
	}

	return server_socket;
}

void hacer_intento_de_reconexion(){
	log_info(team_logger, "Haciendo intento de reconexion...\n");
	sleep(tiempo_reconexion);
	broker_socket = conectar_broker();

}

int conectar_broker() {
	broker_socket = conectar_a_server(ip_broker, "6009");
	if (broker_socket < 0){
		log_info(team_logger, "Error al intentar conectar al broker\n");
		hacer_intento_de_reconexion();
	}
	else{
		if (broker_socket == 0){
			log_info(team_logger, "Error al intentar conectar al broker\n");
			hacer_intento_de_reconexion();
		}else{
			log_info(team_logger, "Conectado con el broker! (%d)",broker_socket);
			convertirse_en_suscriptor_global_del_broker();
			//recibir mensaje de broker
		}
	}

	return broker_socket;
}

void * atender_nuevo_gameboy(int serv_socket){
	struct sockaddr_in client_addr;

	//Setea la direccion en 0
	memset(&client_addr, 0, sizeof(client_addr));
	socklen_t client_len = sizeof(client_addr);

	//Acepta la nueva conexion
	int new_client_sock = accept(serv_socket, (struct sockaddr *)&client_addr, &client_len);
	if (new_client_sock < 0) {
		log_info(team_logger, "Error al aceptar un nuevo gameboy :(\n");
	}else{
		log_info(team_logger, "\nSe aceptó un nuevo gameboy, conexión (%d)", new_client_sock);
		recibir_mensaje_gameboy(new_client_sock);
	}

	 close(new_client_sock);
}
