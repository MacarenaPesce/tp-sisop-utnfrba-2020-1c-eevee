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

void hacer_intento_de_reconexion(){
	log_info(team_logger, "Haciendo intento de reconexion...\n");
	sleep(tiempo_reconexion);
	broker_socket = conectar_broker();

}

int conectar_broker() {
	broker_socket = conectar_a_server(ip_broker, "6009");

	if (broker_socket < 0){
		log_info(team_logger, "Error al intentar conectar al broker\n");
		//hacer_intento_de_reconexion();
		crear_hilo_para_broker();
	}
	else{
		if (broker_socket == 0){
			log_info(team_logger, "Error al intentar conectar al broker\n");
			//hacer_intento_de_reconexion();
			crear_hilo_para_broker();
		}else{
			log_info(team_logger, "Conectado con el broker! (%d)",broker_socket);
			//convertirse_en_suscriptor_global_del_broker();
		}
	}

	return broker_socket;
}
