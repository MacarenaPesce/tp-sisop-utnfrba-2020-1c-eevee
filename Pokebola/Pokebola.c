/*
 * Pokebola.c
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#include "Pokebola.h"


/* REFACTORED */ 

//Conexion
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

	//esto no va más
	//int activado = 1;
	//setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	int res_connect = connect(new_socket, server_info->ai_addr, server_info->ai_addrlen);
	if(res_connect < 0){
		freeaddrinfo(server_info);
		close(new_socket);
		return -1;
	}

	freeaddrinfo(server_info);
	return new_socket;

}

void cerrar_conexion(int sock){
	close(sock);
}

//Envio y recepcion
int recibir_paquete(int sock, void *mensaje, int tamanio){

	int bytes_recibidos = recv(sock, mensaje, tamanio, 0);

	if(bytes_recibidos < 0) {
		//perror("Error en el recv.\n");
		return -1;
	}

	//validación
	// no recibe la cantidad esperada
	if (bytes_recibidos < tamanio){
		//perror("No se recibió el mensaje completo");
		return -2;
	}

	return bytes_recibidos;
}

int enviar_paquete(int sock, void *paquete, int tamanio){

	int bytes_enviados;

	bytes_enviados = send(sock, paquete, tamanio, 0);

	if (bytes_enviados <= 0) {
		return -1;
	}else{
		return bytes_enviados;
	}


	//agrego validacion
	// envia el mensaje pero no completo
		if (bytes_enviados < tamanio){
			//perror("No se ha podido enviar el mensaje completo");
			return -1;
		}
	return bytes_enviados;

}

//Manejo de mensaje
int _enviar_mensaje(int sock, t_packed *paquete){	
	int envio_payload=0, envio_header=0;

	envio_header = enviar_paquete(sock, paquete, sizeof(t_packed)-sizeof(paquete->mensaje));

	//printf("\nEnvio header: %d bytes enviados a %d\n",envio_header,sock);

	if(paquete->tamanio_payload > 0 && envio_header != -1){
		envio_payload = enviar_paquete(sock, paquete->mensaje, paquete->tamanio_payload);
	//	printf("\nEnvio payload: %d\n",envio_header);
	}

	return envio_header || envio_payload;

}

t_packed* recibir_mensaje(int sock){

	t_packed* paquete = recibir_mensaje_serializado(sock);

	if(paquete == (t_packed*)-1 || paquete == (t_packed*)-2) return paquete;

	deserializar_paquete(paquete);	
	
	return paquete;
}

t_packed* recibir_mensaje_serializado(int sock){

	t_packed* paquete;	

	paquete = (t_packed*)malloc(sizeof(t_packed));
	
	int cantidad_bytes = recibir_paquete(sock, paquete,sizeof(t_packed)-sizeof(paquete->mensaje));

	if(cantidad_bytes == -1){
		free(paquete);
		return (t_packed*)-1;
	}

	if(cantidad_bytes == -2){
		free(paquete);
		return(t_packed*)-2;
	}

	if(paquete->tamanio_payload <= 0) {
		return paquete;
	}

	/* Obtener payload */
	paquete->mensaje = (void*)malloc(paquete->tamanio_payload);
	cantidad_bytes = recibir_paquete(sock, paquete->mensaje, paquete->tamanio_payload);

	if(cantidad_bytes < 0){
		return (t_packed*)cantidad_bytes;
	}

	return paquete;

}

void deserializar_paquete(t_packed* paquete){

	switch(paquete->operacion){
		case ENVIAR_MENSAJE:
			_recuperar_mensaje(paquete);
			break;
		
		case SUSCRIBIRSE_A_COLA:
			_recibir_solicitud_suscripcion(paquete);
			break;

		case ACK:
			break;
		
		default:
			//printf("Error, operacion desconocida: %d\n",paquete->operacion);
			break;
	}

}

void _recuperar_mensaje(t_packed *paquete){

	switch(paquete->cola_de_mensajes){

		case MENSAJE_TEXTO:
			break;

		case COLA_CATCH_POKEMON:
		case COLA_APPEARED_POKEMON:
			_recibir_catch_o_appeared_pokemon(paquete);
			break;

		case COLA_NEW_POKEMON:
			_recibir_new_pokemon(paquete);
			break;
		
		case COLA_CAUGHT_POKEMON:
			_recibir_caught_pokemon(paquete);
			break;
		
		case COLA_GET_POKEMON:
			_recibir_get_pokemon(paquete);
			break;

		case COLA_LOCALIZED_POKEMON:
			_recibir_localized_pokemon(paquete);
			break;			

		default:
			//printf("Error, cola de mensajes desconocida: %d\n",paquete->cola_de_mensajes);
			break;
	}
}

void _eliminar_mensaje(t_packed* paquete){	
	free(paquete->mensaje);
	free(paquete);
}

void eliminar_mensaje(t_packed* paquete){
	switch(paquete->operacion){

		case ENVIAR_MENSAJE:
			_eliminar_contenido_mensaje_segun_cola(paquete);
			_eliminar_mensaje(paquete);			
			break;

		case SUSCRIBIRSE_A_COLA:
		case ACK:
			free(paquete);
			break;

		default:
			//printf("\nOperacion desconocida %d\n",paquete->operacion);
			break;

	}
		
}

void _eliminar_contenido_mensaje_segun_cola(t_packed* paquete){

	switch (paquete->cola_de_mensajes)
	{
		case COLA_CATCH_POKEMON:;
			t_catch_pokemon* catch_pokemon = (t_catch_pokemon*) paquete->mensaje;
			free(catch_pokemon->pokemon);
			break;

		case COLA_APPEARED_POKEMON:;
			t_appeared_pokemon* appeared_pokemon = (t_appeared_pokemon*) paquete->mensaje;
			free(appeared_pokemon->pokemon);
			break;

		case COLA_NEW_POKEMON:;
			t_new_pokemon* new_pokemon = (t_new_pokemon*) paquete->mensaje;
			free(new_pokemon->pokemon);
			break;

		case COLA_GET_POKEMON:;
			t_get_pokemon* get_pokemon = (t_get_pokemon*) paquete->mensaje;
			free(get_pokemon->pokemon);
			break;

		case COLA_LOCALIZED_POKEMON:;
			t_localized_pokemon* localized_pokemon = (t_localized_pokemon*) paquete->mensaje;
			free(localized_pokemon->pokemon);
			list_destroy_and_destroy_elements(localized_pokemon->lista_coordenadas,(void*)free);
			break;
	
		default:
			break;
	}

}

//Serializacion
t_packed* _crear_paquete(enum OPERACIONES operacion){
	t_packed* paquete;
	paquete = (t_packed*)malloc(sizeof(t_packed));	

	paquete->id_mensaje = -1;
	paquete->id_correlacional = -1;
	paquete->id_cliente = -1;
	paquete->tamanio_payload = 0;
	paquete->cola_de_mensajes = -1;
	paquete->operacion = operacion;
	paquete->mensaje = malloc(sizeof(0));

	return paquete;
}

void _agregar_dato_a_paquete(t_packed *paquete, void *value, int size){

	paquete->mensaje = realloc(paquete->mensaje, paquete->tamanio_payload + size );
	
	memcpy(paquete->mensaje + paquete->tamanio_payload, value, size);

	paquete->tamanio_payload += size;

}

void _agregar_string_a_paquete(t_packed* paquete, char* string){

	_agregar_dato_a_paquete(paquete, string, strlen(string));

}

void _agregar_uint32_t_a_paquete(t_packed* paquete, uint32_t value){

	_agregar_dato_a_paquete(paquete, &value, sizeof(uint32_t));

}

void _agregar_int_t_a_paquete(t_packed* paquete, int value){

	_agregar_dato_a_paquete(paquete, &value, sizeof(int));

}

//Implementaciones Envio
t_packed* enviar_mensaje_string(t_servidor* servidor, char* mensaje){

	int socket =  conectar_a_server(servidor->ip,servidor->puerto);

	if(socket == -1) return (t_packed *)-1;

	t_packed* paquete;
	paquete = _crear_paquete(ENVIAR_MENSAJE);
	paquete->id_cliente = servidor->id_cliente;

	_agregar_string_a_paquete(paquete, mensaje);

	_enviar_mensaje(socket,paquete);
	_eliminar_mensaje(paquete);

	t_packed* ack;
	ack = _esperar_ack(socket);

	cerrar_conexion(socket);

	return ack;
}

int _enviar_catch_o_appeared_pokemon(int socket,
							 uint32_t id_mensaje, 
							 uint32_t id_correlacional, 
							 uint32_t id_cliente,
							 uint32_t cola_de_mensajes,
							 t_appeared_pokemon* appeared_pokemon ){

	t_packed* paquete;
	paquete = _crear_paquete(ENVIAR_MENSAJE);

	paquete->id_mensaje = id_mensaje;
	paquete->id_correlacional = id_correlacional;
	paquete->cola_de_mensajes = cola_de_mensajes;
	paquete->id_cliente = id_cliente;
		
	_agregar_uint32_t_a_paquete(paquete, appeared_pokemon->coordenadas.posx);
	_agregar_uint32_t_a_paquete(paquete, appeared_pokemon->coordenadas.posy);
	_agregar_uint32_t_a_paquete(paquete, strlen(appeared_pokemon->pokemon));
	_agregar_string_a_paquete(paquete, appeared_pokemon->pokemon);

	int send_status = _enviar_mensaje(socket, paquete);
	_eliminar_mensaje(paquete);

	return send_status;

}

t_packed* enviar_appeared_pokemon(t_servidor* servidor,
							 uint32_t id_correlacional, 
							 t_appeared_pokemon* appeared_pokemon ){

    int socket =  conectar_a_server(servidor->ip,servidor->puerto);

	if(socket == -1) return (t_packed *)-1;

	int send_status = distribuir_appeared_pokemon(socket,-1,id_correlacional,servidor->id_cliente,appeared_pokemon);

	// printf("\nsend status appeared: %d",send_status);

	if(send_status == -1) {
		cerrar_conexion(socket);
		return (t_packed *) -1;
	} 
	
	t_packed* ack;
	
	ack = _esperar_ack(socket);
	
    cerrar_conexion(socket);

	return ack;

}

int distribuir_appeared_pokemon(int socket,
								uint32_t id_mensaje, 
								uint32_t id_correlacional,
								uint32_t id_cliente,
								t_appeared_pokemon* appeared_pokemon ){

	int send_status = _enviar_catch_o_appeared_pokemon(socket,
									 id_mensaje,
									 id_correlacional,
									 id_cliente,
									 COLA_APPEARED_POKEMON,
									 appeared_pokemon);
	
	return send_status;

}

t_packed* enviar_catch_pokemon(t_servidor* servidor,
							 uint32_t id_correlacional, 
							 t_catch_pokemon* catch_pokemon){

    int socket =  conectar_a_server(servidor->ip,servidor->puerto);

	if(socket == -1) return (t_packed *)-1;

	int send_status = distribuir_catch_pokemon(socket,-1,id_correlacional,servidor->id_cliente,catch_pokemon);

	if(send_status == -1) {
		cerrar_conexion(socket);
		return (t_packed *) -1;
	} 
    
	t_packed* ack;
	ack = _esperar_ack(socket);

	cerrar_conexion(socket);

	return ack;
};

int distribuir_catch_pokemon(int socket,
							uint32_t id_mensaje, 
							uint32_t id_correlacional,
							uint32_t id_cliente,
							t_catch_pokemon* catch_pokemon ){

	int send_status = _enviar_catch_o_appeared_pokemon(socket,id_mensaje,id_correlacional,id_cliente,COLA_CATCH_POKEMON,catch_pokemon);

	return send_status;
};

t_packed* enviar_new_pokemon(t_servidor* servidor,
							 uint32_t id_correlacional, 
							 t_new_pokemon* new_pokemon){

    int socket =  conectar_a_server(servidor->ip,servidor->puerto);

	if(socket == -1) return (t_packed *)-1;

	int send_status = distribuir_new_pokemon(socket,-1,id_correlacional,servidor->id_cliente,new_pokemon);

	if(send_status == -1) {
		cerrar_conexion(socket);
		return (t_packed *) -1;
	} 

	t_packed* ack;
	ack = _esperar_ack(socket);

	cerrar_conexion(socket);

	return ack;

};

int distribuir_new_pokemon(int socket,
							 uint32_t id_mensaje, 
							 uint32_t id_correlacional, 
							 uint32_t id_cliente,
							 t_new_pokemon* new_pokemon){

	t_packed* paquete;
	paquete = _crear_paquete(ENVIAR_MENSAJE);

	paquete->id_mensaje = id_mensaje;
	paquete->id_correlacional = id_correlacional;
	paquete->cola_de_mensajes = COLA_NEW_POKEMON;
	paquete->id_cliente = id_cliente;

	_agregar_uint32_t_a_paquete(paquete, new_pokemon->coordenadas.posx);
	_agregar_uint32_t_a_paquete(paquete, new_pokemon->coordenadas.posy);
	_agregar_uint32_t_a_paquete(paquete, new_pokemon->cantidad);
	_agregar_uint32_t_a_paquete(paquete, strlen(new_pokemon->pokemon));
	_agregar_string_a_paquete(paquete, new_pokemon->pokemon);
	
	int send_status = _enviar_mensaje(socket, paquete);
	_eliminar_mensaje(paquete);

	return send_status;

};

t_packed* enviar_caught_pokemon(t_servidor* servidor,
							 uint32_t id_correlacional, 
							 t_caught_pokemon* caught_pokemon){

	int socket =  conectar_a_server(servidor->ip,servidor->puerto);

	if(socket == -1) return (t_packed *) -1;

	int send_status = distribuir_caught_pokemon(socket,-1,id_correlacional,servidor->id_cliente,caught_pokemon);

	if(send_status == -1) {
		cerrar_conexion(socket);
		return (t_packed *) -1;
	} 

	t_packed* ack;
	ack = _esperar_ack(socket);

	cerrar_conexion(socket);

	return ack;

};

int distribuir_caught_pokemon(int socket,
							 uint32_t id_mensaje, 
							 uint32_t id_correlacional, 
							 uint32_t id_cliente,
							 t_caught_pokemon* caught_pokemon){

	t_packed* paquete;
	paquete = _crear_paquete(ENVIAR_MENSAJE);

	paquete->id_mensaje = id_mensaje;
	paquete->id_correlacional = id_correlacional;
	paquete->cola_de_mensajes = COLA_CAUGHT_POKEMON;
	paquete->id_cliente = id_cliente;

	_agregar_uint32_t_a_paquete(paquete, caught_pokemon->status);
	
	int send_status = _enviar_mensaje(socket, paquete);
	_eliminar_mensaje(paquete);

	return send_status;

};

t_packed* enviar_get_pokemon(t_servidor* servidor,
							 uint32_t id_correlacional,
							 t_get_pokemon* get_pokemon){

    int socket =  conectar_a_server(servidor->ip,servidor->puerto);

	if(socket == -1){
		return (t_packed *) -1;
	}	

	int send_status = distribuir_get_pokemon(socket,-1,id_correlacional,servidor->id_cliente,get_pokemon);

	if(send_status == -1) {
		cerrar_conexion(socket);
		return (t_packed *) -1;
	} 	

	t_packed* ack;
	ack = _esperar_ack(socket);

	cerrar_conexion(socket);

	return ack;

};

int distribuir_get_pokemon(int socket,
							 uint32_t id_mensaje, 
							 uint32_t id_correlacional, 
							 uint32_t id_cliente,
							 t_get_pokemon* get_pokemon){

	t_packed* paquete;
	paquete = _crear_paquete(ENVIAR_MENSAJE);

	paquete->id_mensaje = id_mensaje;
	paquete->id_correlacional = id_correlacional;
	paquete->cola_de_mensajes = COLA_GET_POKEMON;
	paquete->id_cliente = id_cliente;

	_agregar_uint32_t_a_paquete(paquete, strlen(get_pokemon->pokemon));
	_agregar_string_a_paquete(paquete, get_pokemon->pokemon);

	int send_status = _enviar_mensaje(socket, paquete);

	_eliminar_mensaje(paquete);

	return send_status;

};

int enviar_ack(t_servidor* servidor, uint32_t id_mensaje){

	int socket =  conectar_a_server(servidor->ip,servidor->puerto);

	if(socket == -1){
		return -1;
	}	

	int send_status = distribuir_ack(socket, id_mensaje, servidor->id_cliente);

	return send_status;
};

int distribuir_ack(int socket,uint32_t id_mensaje, uint32_t id_cliente){

	t_packed* paquete;
	paquete = _crear_paquete(ACK);

	paquete->id_mensaje = id_mensaje;
	paquete->id_correlacional = -1;
	paquete->id_cliente = id_cliente;

	int send_status = _enviar_mensaje(socket, paquete);

	_eliminar_mensaje(paquete);

	return send_status;
};

t_packed* enviar_localized_pokemon(t_servidor* servidor,
							 uint32_t id_correlacional, 
							 t_localized_pokemon* localized_pokemon){

	int socket =  conectar_a_server(servidor->ip,servidor->puerto);

	if(socket == -1){
		return (t_packed *) -1;
	}	

	int send_status = distribuir_localized_pokemon(socket,-1,id_correlacional,servidor->id_cliente,localized_pokemon);

	if(send_status == -1) {
		cerrar_conexion(socket);
		return (t_packed *) -1;
	} 	

	t_packed* ack;
	ack = _esperar_ack(socket);

	cerrar_conexion(socket);

	return ack;				

};

int distribuir_localized_pokemon(int socket,
							 uint32_t id_mensaje, 
							 uint32_t id_correlacional, 
							 uint32_t id_cliente,
							 t_localized_pokemon* localized_pokemon){


	t_packed* paquete;
	paquete = _crear_paquete(ENVIAR_MENSAJE);

	paquete->id_mensaje = id_mensaje;
	paquete->id_correlacional = id_correlacional;
	paquete->cola_de_mensajes = COLA_LOCALIZED_POKEMON;
	paquete->id_cliente = id_cliente;

	_agregar_uint32_t_a_paquete(paquete, localized_pokemon->cantidad_coordenadas);

	void agregar_coordenadas_a_paquete(void* _coordenadas){
		
		t_coordenadas* coordenadas = (t_coordenadas*) _coordenadas;
		_agregar_uint32_t_a_paquete(paquete,coordenadas->posx);
		_agregar_uint32_t_a_paquete(paquete,coordenadas->posy);

	}

	list_iterate(localized_pokemon->lista_coordenadas,agregar_coordenadas_a_paquete);
	
	_agregar_uint32_t_a_paquete(paquete, strlen(localized_pokemon->pokemon));
	_agregar_string_a_paquete(paquete, localized_pokemon->pokemon);

	int send_status = _enviar_mensaje(socket, paquete);

	_eliminar_mensaje(paquete);

	return send_status;

}

int enviar_solicitud_suscripcion(t_servidor* servidor,uint32_t cola_de_mensajes){

	int socket =  conectar_a_server(servidor->ip,servidor->puerto);

	if(socket == -1) return -1;

	t_packed* paquete = _crear_paquete(SUSCRIBIRSE_A_COLA);

	paquete->cola_de_mensajes = cola_de_mensajes;
	paquete->id_cliente = servidor->id_cliente;

	int send_status = _enviar_mensaje(socket, paquete);

	if(send_status == -1) {
		cerrar_conexion(socket);
		_eliminar_mensaje(paquete);
		return -1;
	} 

	_eliminar_mensaje(paquete);	

	return socket;

}

//Implementaciones Recepcion

void _recibir_catch_o_appeared_pokemon(t_packed *paquete){

	uint32_t _tamanio_string_pokemon;
	int offset = 0;
	t_appeared_pokemon *aux;
	char* string_terminator = "\0";

	aux = (t_appeared_pokemon*)malloc(sizeof(t_appeared_pokemon));

	memcpy(aux,(paquete->mensaje)+offset,sizeof(t_coordenadas));
	offset += sizeof(t_coordenadas);

	memcpy(&_tamanio_string_pokemon,(paquete->mensaje)+offset,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	paquete->tamanio_payload -= sizeof(uint32_t);

	aux->pokemon = (char*)malloc(_tamanio_string_pokemon+1);
	memcpy(aux->pokemon,(paquete->mensaje)+offset,_tamanio_string_pokemon);
	memcpy((aux->pokemon)+_tamanio_string_pokemon,string_terminator,1);

	free(paquete->mensaje);

	paquete->mensaje = aux;

	return;
}

void _recibir_new_pokemon(t_packed *paquete){

	int offset = 0;
	uint32_t _tamanio_string_pokemon;
	char* string_terminator = "\0";
	//t_new_pokemon *aux;

	aux = (t_new_pokemon*)malloc(sizeof(t_new_pokemon));	

	memcpy(aux,(paquete->mensaje)+offset,sizeof(t_coordenadas)+sizeof(uint32_t));
	offset += sizeof(t_coordenadas)+sizeof(uint32_t);

	memcpy(&_tamanio_string_pokemon,(paquete->mensaje)+offset,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	paquete->tamanio_payload -= sizeof(uint32_t);

	aux->pokemon = (char*)malloc(_tamanio_string_pokemon+1);
	memcpy(aux->pokemon,(paquete->mensaje)+offset,_tamanio_string_pokemon);
	memcpy((aux->pokemon)+_tamanio_string_pokemon,string_terminator,1);


	free(paquete->mensaje);
	paquete->mensaje=aux;

	return;
}

void _recibir_caught_pokemon(t_packed *paquete){

	return;
	
}

void _recibir_get_pokemon(t_packed *paquete){

	int offset = 0;
	uint32_t _tamanio_string_pokemon;
	t_get_pokemon* aux;
	char* string_terminator = "\0";

	aux = (t_get_pokemon*)malloc(sizeof(t_get_pokemon));

	memcpy(&_tamanio_string_pokemon,(paquete->mensaje)+offset,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	paquete->tamanio_payload -= sizeof(uint32_t);

	aux->pokemon = (char*)malloc(_tamanio_string_pokemon+1);
	memcpy(aux->pokemon,(paquete->mensaje)+offset,_tamanio_string_pokemon);
	memcpy((aux->pokemon)+_tamanio_string_pokemon,string_terminator,1);

	free(paquete->mensaje);

	paquete->mensaje = aux;

	return;
}

void _recibir_solicitud_suscripcion(t_packed *paquete){

	return;

}

void _recibir_localized_pokemon(t_packed *paquete){

	t_localized_pokemon* localized_pokemon_aux = generar_localized(NULL);

	int offset = 0;

	char* string_terminator = "\0";

	memcpy(localized_pokemon_aux,(paquete->mensaje)+offset,sizeof(uint32_t));

	offset += sizeof(u_int32_t);

	for(int i=0;i<localized_pokemon_aux->cantidad_coordenadas;i++){

		uint32_t posx;
		memcpy(&posx,paquete->mensaje+offset,sizeof(uint32_t));
		offset += sizeof(u_int32_t);

		uint32_t posy;
		memcpy(&posy,paquete->mensaje+offset,sizeof(uint32_t));
		offset += sizeof(u_int32_t);

		t_coordenadas* coordenadas = (t_coordenadas*)malloc(sizeof(t_coordenadas));

		coordenadas->posx = posx;
		coordenadas->posy = posy;

		list_add(localized_pokemon_aux->lista_coordenadas,(void*)coordenadas);

	}

	uint32_t _tamanio_string_pokemon;	
	memcpy(&_tamanio_string_pokemon,paquete->mensaje+offset,sizeof(uint32_t));
	offset += sizeof(u_int32_t);

	localized_pokemon_aux->pokemon = (char*)malloc(_tamanio_string_pokemon+1);
	memcpy(localized_pokemon_aux->pokemon,(paquete->mensaje)+offset,_tamanio_string_pokemon);
	memcpy((localized_pokemon_aux->pokemon)+_tamanio_string_pokemon,string_terminator,1);

	free(paquete->mensaje);

	paquete->mensaje = localized_pokemon_aux;

	return;

}

t_packed* _esperar_ack(int socket){

	bool ack_obtenido = false;
	t_packed* ack;

	while(!ack_obtenido){
		
		ack = recibir_mensaje(socket);

		if(ack != (t_packed*)-1){
		/*	printf("\n\nACK Recibido:\n");
			printf("operacion: %d \n",ack->operacion);
			printf("cola_de_mensajes: %d \n",ack->cola_de_mensajes);
			printf("id_correlacional: %d  \n",ack->id_correlacional);
			printf("id_mensaje: %d \n",ack->id_mensaje);
			printf("id_cliente: %d \n",ack->id_cliente);
			printf("tamanio_payload: %d \n",ack->tamanio_payload);*/
			ack_obtenido = true;	
		}	

	}

	return ack;
	
}

char* obtener_nombre_cola(int cola_de_mensajes){

	switch(cola_de_mensajes){

		case COLA_APPEARED_POKEMON:
			return "APPEARED_POKEMON";
		case COLA_CATCH_POKEMON:
			return "CATCH_POKEMON";
		case COLA_CAUGHT_POKEMON:
			return "CAUGHT_POKEMON";
		case COLA_GET_POKEMON:
			return "GET_POKEMON";
		case COLA_LOCALIZED_POKEMON:
			return "LOCALIZED_POKEMON";
		case COLA_NEW_POKEMON:
			return "NEW_POKEMON";
		default:
			//printf("Cola %d no reconocida\n",cola_de_mensajes);
			return "";

	}
}

void agregar_coordenadas_a_localized(t_localized_pokemon* localized_pokemon, t_coordenadas* _coordenadas){
	
	t_coordenadas* coordenadas = malloc(sizeof(t_coordenadas));

	memcpy(coordenadas,_coordenadas,sizeof(t_coordenadas));

	list_add(localized_pokemon->lista_coordenadas,(void*) coordenadas);

	localized_pokemon->cantidad_coordenadas++;

}

t_localized_pokemon* generar_localized(char* nombre_pokemon){
	
	t_localized_pokemon* localized_pokemon = malloc(sizeof(t_localized_pokemon));
	
	if(nombre_pokemon != NULL){
		localized_pokemon->pokemon = malloc(strlen(nombre_pokemon)+1);
		memcpy(localized_pokemon->pokemon,nombre_pokemon,strlen(nombre_pokemon)+1);
	}

	localized_pokemon->lista_coordenadas = list_create();

	localized_pokemon->cantidad_coordenadas = 0;

	return localized_pokemon;

}

void eliminar_localized_pokemon(t_localized_pokemon* localized_pokemon){

	list_destroy_and_destroy_elements(localized_pokemon->lista_coordenadas,free);
	free(localized_pokemon->pokemon);
	free(localized_pokemon);
	
}

void dynamic_list_iterate(t_list* self, void(*closure)(void*)) {
    t_link_element *element = self->head;
    t_link_element *aux = NULL;
    while (element != NULL) {
        closure(element->data);
        aux = element->next;
        element = aux;
    }
}