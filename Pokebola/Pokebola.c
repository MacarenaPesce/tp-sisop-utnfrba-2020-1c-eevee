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
	int bytes_recibidos;
	if((bytes_recibidos = recv(sock, mensaje, tamanio, 0)) < 0) {
		perror("Error en el recv.\n");
		return -1;
	}

	//validación
	// no recibe la cantidad esperada
			if (bytes_recibidos < tamanio){
				perror("No se recibió el mensaje completo");
						return -1;
			}

	return bytes_recibidos;
}

int enviar_paquete(int sock, void *paquete, int tamanio){

	int bytes_enviados;

	bytes_enviados = send(sock, paquete, tamanio, 0);

	if (bytes_enviados <= 0) {
		perror("Error en el send");
		return -1;
	}

	//agrego validacion
	// envia el mensaje pero no completo
		if (bytes_enviados < tamanio){
			perror("No se ha podido enviar el mensaje completo");
					return -1;
		}
	return bytes_enviados;
}

//Manejo de mensaje
int _enviar_mensaje(int sock,
				   t_packed *paquete){	
	int envio_payload, envio_header;

	envio_header = enviar_paquete(sock, paquete, sizeof(t_packed)-sizeof(paquete->mensaje));


	envio_payload = enviar_paquete(sock, paquete->mensaje, paquete->tamanio_payload);


	return envio_header+envio_payload;

}

t_packed* recibir_mensaje(int sock){

	t_packed* paquete;	
	void* mensaje;

	paquete = (t_packed*)malloc(sizeof(t_packed));
	recibir_paquete(sock, paquete,sizeof(t_packed)-sizeof(paquete->mensaje));

	mensaje = (void*)malloc(paquete->tamanio_payload);
	recibir_paquete(sock, mensaje, paquete->tamanio_payload);

	switch(paquete->operacion){
		case ENVIAR_MENSAJE:
			_recuperar_mensaje(mensaje,paquete);
			break;
		
		case SUSCRIBIRSE_A_COLA:
			_recibir_solicitud_suscripcion(mensaje,paquete);
			break;
		
		default:
			printf("Error, operacion desconocida: %d\n",paquete->operacion);
			break;
	}

	return paquete;
}

void _recuperar_mensaje(void *mensaje,t_packed *paquete){

	switch(paquete->cola_de_mensajes){

		case -1:
			_recibir_mensaje_string(mensaje,paquete);
			break;

		case COLA_CATCH_POKEMON:
		case COLA_APPEARED_POKEMON:
			_recibir_catch_o_appeared_pokemon(mensaje,paquete);
			break;

		case COLA_NEW_POKEMON:
			_recibir_new_pokemon(mensaje,paquete);
			break;
		
		case COLA_CAUGHT_POKEMON:
			_recibir_solicitud_suscripcion(mensaje,paquete);
			break;
		
		case COLA_GET_POKEMON:
			_recibir_get_pokemon(mensaje,paquete);
				printf("se recibio el pkmn %s\n",((t_get_pokemon*)paquete->mensaje)->pokemon);
			break;

		case COLA_LOCALIZED_POKEMON:
			_recibir_localized_pokemon(mensaje,paquete);
			break;			

		default:
			printf("Error, cola de mensajes desconocida: %d\n",paquete->cola_de_mensajes);
			break;
	}
}

void _eliminar_mensaje(t_packed* paquete){
	free(paquete->mensaje);
	free(paquete);
}

void eliminar_mensaje(t_packed* paquete){
	free(paquete);
}

//Serializacion
t_packed* _crear_paquete(enum OPERACIONES operacion){
	t_packed* paquete;
	paquete = (t_packed*)malloc(sizeof(t_packed));	

	paquete->id_mensaje = -1;
	paquete->id_correlacional = -1;
	paquete->cola_de_mensajes = -1;
	paquete->tamanio_payload = 0;
	paquete->operacion = operacion;
	paquete->mensaje = (void*)malloc(0);

	return paquete;
}

void _agregar_dato_a_paquete(t_packed *paquete, void *value, int size){

	paquete->mensaje = realloc(paquete->mensaje, paquete->tamanio_payload + size );
	
	memcpy(paquete->mensaje + paquete->tamanio_payload, value, size);

	paquete->tamanio_payload += size;

}

void _agregar_string_a_paquete(t_packed* paquete, char* string){

	_agregar_dato_a_paquete(paquete, string, strlen(string)+1);

}

void _agregar_uint32_t_a_paquete(t_packed* paquete, uint32_t value){

	_agregar_dato_a_paquete(paquete, &value, sizeof(uint32_t));

}

void _agregar_int_t_a_paquete(t_packed* paquete, int value){

	_agregar_dato_a_paquete(paquete, &value, sizeof(int));

}

//Implementaciones Envio
void enviar_mensaje_string(int socket, char* mensaje){
	t_packed* paquete;
	paquete = _crear_paquete(ENVIAR_MENSAJE);

	_agregar_string_a_paquete(paquete, mensaje);

	_enviar_mensaje(socket,paquete);
	_eliminar_mensaje(paquete);
}

void _enviar_catch_o_appeared_pokemon(int socket,
							 uint32_t id_mensaje, 
							 uint32_t id_correlacional, 
							 uint32_t cola_de_mensajes,
							 t_appeared_pokemon* appeared_pokemon ){
	t_packed* paquete;
	paquete = _crear_paquete(ENVIAR_MENSAJE);

	paquete->id_mensaje = id_mensaje;
	paquete->id_correlacional = id_correlacional;
	paquete->cola_de_mensajes = cola_de_mensajes;
		
	_agregar_uint32_t_a_paquete(paquete, appeared_pokemon->coordenadas.posx);
	_agregar_uint32_t_a_paquete(paquete, appeared_pokemon->coordenadas.posy);
	_agregar_uint32_t_a_paquete(paquete, strlen(appeared_pokemon->pokemon)+1);
	_agregar_string_a_paquete(paquete, appeared_pokemon->pokemon);

	_enviar_mensaje(socket, paquete);
	_eliminar_mensaje(paquete);
}

void enviar_appeared_pokemon(int socket,
							 uint32_t id_mensaje, 
							 uint32_t id_correlacional, 
							 t_appeared_pokemon* appeared_pokemon ){

	_enviar_catch_o_appeared_pokemon(socket,id_mensaje,id_correlacional,COLA_APPEARED_POKEMON,appeared_pokemon);

}

void enviar_catch_pokemon(int socket,
							 uint32_t id_mensaje, 
							 uint32_t id_correlacional, 
							 t_catch_pokemon* catch_pokemon){

	_enviar_catch_o_appeared_pokemon(socket,id_mensaje,id_correlacional,COLA_CATCH_POKEMON,catch_pokemon);

};

void enviar_new_pokemon(int socket,
							 uint32_t id_mensaje, 
							 uint32_t id_correlacional, 
							 t_new_pokemon* new_pokemon){
	t_packed* paquete;
	paquete = _crear_paquete(ENVIAR_MENSAJE);

	paquete->id_mensaje = id_mensaje;
	paquete->id_correlacional = id_correlacional;
	paquete->cola_de_mensajes = COLA_NEW_POKEMON;

	_agregar_uint32_t_a_paquete(paquete, new_pokemon->coordenadas.posx);
	_agregar_uint32_t_a_paquete(paquete, new_pokemon->coordenadas.posy);
	_agregar_uint32_t_a_paquete(paquete, new_pokemon->cantidad);
	_agregar_uint32_t_a_paquete(paquete, strlen(new_pokemon->pokemon)+1);
	_agregar_string_a_paquete(paquete, new_pokemon->pokemon);
	
	_enviar_mensaje(socket, paquete);
	_eliminar_mensaje(paquete);

};

void enviar_caught_pokemon(int socket,
							 uint32_t id_mensaje, 
							 uint32_t id_correlacional, 
							 t_caught_pokemon* caught_pokemon){
	t_packed* paquete;
	paquete = _crear_paquete(ENVIAR_MENSAJE);

	paquete->id_mensaje = id_mensaje;
	paquete->id_correlacional = id_correlacional;
	paquete->cola_de_mensajes = COLA_CAUGHT_POKEMON;

	_agregar_uint32_t_a_paquete(paquete, caught_pokemon->status);
	
	_enviar_mensaje(socket, paquete);
	_eliminar_mensaje(paquete);

};

void enviar_get_pokemon(int socket,
							 uint32_t id_mensaje, 
							 uint32_t id_correlacional, 
							 t_get_pokemon* get_pokemon){
	t_packed* paquete;
	paquete = _crear_paquete(ENVIAR_MENSAJE);

	paquete->id_mensaje = id_mensaje;
	paquete->id_correlacional = id_correlacional;
	paquete->cola_de_mensajes = COLA_GET_POKEMON;

	_agregar_uint32_t_a_paquete(paquete, strlen(get_pokemon->pokemon)+1);
	_agregar_string_a_paquete(paquete, get_pokemon->pokemon);

	_enviar_mensaje(socket, paquete);
	_eliminar_mensaje(paquete);

};

//TODO
void enviar_localized_pokemon(int socket,
							 uint32_t id_mensaje, 
							 uint32_t id_correlacional, 
							 t_localized_pokemon* localized_pokemon){
	/*t_packed* paquete;
	paquete = _crear_paquete(ENVIAR_MENSAJE);

	paquete->id_mensaje = id_mensaje;
	paquete->id_correlacional = id_correlacional;
	paquete->cola_de_mensajes = COLA_LOCALIZED_POKEMON;

	_agregar_uint32_t_a_paquete(paquete, localized_pokemon->cantidad_coordenadas);
	_agregar_uint32_t_a_paquete(paquete, strlen(localized_pokemon->pokemon)+1);
	_agregar_string_a_paquete(paquete, localized_pokemon->pokemon);
	
	list_iterate(localized_pokemon->lista_coordenadas,(*_agregar_lista_a_paquete)(paquete))

	_enviar_mensaje(socket, paquete);
	_eliminar_mensaje(paquete);*/

};

void enviar_solicitud_suscripcion(int socket,uint32_t cola_de_mensajes, t_suscripcion* suscripcion){
	t_packed* paquete;
	paquete = _crear_paquete(SUSCRIBIRSE_A_COLA);

	paquete->cola_de_mensajes = cola_de_mensajes;

	_agregar_uint32_t_a_paquete(paquete, suscripcion->tipo_suscripcion);
	_agregar_uint32_t_a_paquete(paquete, suscripcion->minutos_suscripcion);

	_enviar_mensaje(socket, paquete);
	_eliminar_mensaje(paquete);	

}


//Implementaciones Recepcion
void _recibir_mensaje_string(void *mensaje,t_packed *paquete){

	paquete->mensaje = (t_appeared_pokemon*)malloc(paquete->tamanio_payload);

	memcpy(paquete->mensaje,mensaje,paquete->tamanio_payload);

	free(mensaje);

	return;

}

void _recibir_catch_o_appeared_pokemon(void *mensaje,t_packed *paquete){
	t_appeared_pokemon* aux;

	aux 			 = (t_appeared_pokemon*)malloc(sizeof(t_appeared_pokemon));
	paquete->mensaje = (t_appeared_pokemon*)malloc(sizeof(t_appeared_pokemon));

	memcpy(aux,mensaje,sizeof(t_appeared_pokemon)-sizeof(aux->pokemon));

	aux->pokemon = (char*)malloc(aux->_tamanio_string_pokemon);

	memcpy(aux->pokemon,mensaje+sizeof(t_appeared_pokemon)-sizeof(aux->pokemon),aux->_tamanio_string_pokemon);

	paquete->mensaje = aux;

	free(mensaje);

	return;
}

void _recibir_new_pokemon(void *mensaje,t_packed *paquete){
	t_new_pokemon* aux;

	aux 			 = (t_new_pokemon*)malloc(sizeof(t_new_pokemon));
	paquete->mensaje = (t_new_pokemon*)malloc(sizeof(t_new_pokemon));

	memcpy(aux,mensaje,sizeof(t_new_pokemon)-sizeof(aux->pokemon));

	aux->pokemon = (char*)malloc(aux->_tamanio_string_pokemon);

	memcpy(aux->pokemon,mensaje+sizeof(t_new_pokemon)-sizeof(aux->pokemon),aux->_tamanio_string_pokemon);

	paquete->mensaje = aux;

	free(mensaje);

	return;
}

void _recibir_caught_pokemon(void *mensaje,t_packed *paquete){

	paquete->mensaje = (t_caught_pokemon*)malloc(sizeof(t_caught_pokemon));

	memcpy(paquete->mensaje,mensaje,sizeof(t_caught_pokemon));

	free(mensaje);

	return;
}

void _recibir_get_pokemon(void *mensaje,t_packed *paquete){
	t_get_pokemon* aux;

	aux 			 = (t_get_pokemon*)malloc(sizeof(t_get_pokemon));
	paquete->mensaje = (t_get_pokemon*)malloc(sizeof(t_get_pokemon));

	memcpy(aux,mensaje,sizeof(t_get_pokemon)-sizeof(aux->pokemon));

	aux->pokemon = (char*)malloc(aux->_tamanio_string_pokemon);
	memcpy(aux->pokemon,mensaje+sizeof(t_get_pokemon)-sizeof(aux->pokemon),aux->_tamanio_string_pokemon);

	paquete->mensaje = aux;

	free(mensaje);

	return;
}

void _recibir_solicitud_suscripcion(void *mensaje,t_packed *paquete){

	paquete->mensaje = malloc(sizeof(t_suscripcion));

	memcpy(paquete->mensaje,mensaje,sizeof(t_suscripcion));

	free(mensaje);

	return;
}

//TODO
void _recibir_localized_pokemon(void *mensaje,t_packed *paquete){
	t_get_pokemon* aux;

	aux 			 = (t_get_pokemon*)malloc(sizeof(t_get_pokemon));
	paquete->mensaje = (t_get_pokemon*)malloc(sizeof(t_get_pokemon));

	memcpy(aux,mensaje,sizeof(t_get_pokemon)-sizeof(aux->pokemon));

	aux->pokemon = (char*)malloc(aux->_tamanio_string_pokemon);
	memcpy(aux->pokemon,mensaje+sizeof(t_get_pokemon)-sizeof(aux->pokemon),aux->_tamanio_string_pokemon);

	paquete->mensaje = aux;

	free(mensaje);

	return;
}




/*

void enviar_modo_suscriptor(uint32_t cola_de_mensajes, uint32_t tiempo, int socket){
	t_paquete* paquete = crear_paquete(SUSCRIPTOR);
	agregar_uint32_t_a_paquete(paquete, cola_de_mensajes);
	agregar_uint32_t_a_paquete(paquete, tiempo);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

tp_modo_suscriptor recibir_modo_suscriptor(int paquete_size, int socket){
	void * buffer = malloc(paquete_size);
	recibir_mensaje(socket, buffer, paquete_size);
	uint32_t cola_de_mensajes;
	uint32_t tiempo;
	int desplazamiento = 0;
	memcpy(&cola_de_mensajes, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);
	memcpy(&tiempo, buffer + desplazamiento, sizeof(uint32_t));
	tp_modo_suscriptor contenido = malloc(sizeof(t_modo_suscriptor));
	contenido->cola_de_mensajes=cola_de_mensajes;
	contenido->tiempo=tiempo;
	free(buffer);
	return contenido;
}



/**************FUNCIONES PARA EL LOG*********************/
void escribir_en_pantalla(int tipo_esc, int tipo_log, char* console_buffer, char* log_colors[8], char* msj_salida){

	if ((tipo_esc == escribir) || (tipo_esc == escribir_loguear)) {
		console_buffer = string_from_format("%s%s%s", log_colors[tipo_log],
				msj_salida, log_colors[0]);
		printf("%s", console_buffer);
		printf("%s","\n");
		fflush(stdout);
		free(console_buffer);
	}
}

void definir_nivel_y_loguear(int tipo_esc, int tipo_log, char* msj_salida) {
	
	/*if ((tipo_esc == loguear) || (tipo_esc == escribir_loguear)) {
		if (tipo_log == l_info) {
			log_info(logger, msj_salida);
		} else if (tipo_log == l_warning) {
			log_warning(logger, msj_salida);
		} else if (tipo_log == l_error) {
			log_error(logger, msj_salida);
		} else if (tipo_log == l_debug) {
			log_debug(logger, msj_salida);
		} else if (tipo_log == l_trace) {
			log_trace(logger, msj_salida);
		}
	}*/
}

void logger(int tipo_esc, int tipo_log, const char* mensaje, ...){

	//Colores (reset,vacio,vacio,cian,verde,vacio,amarillo,rojo)
	char *log_colors[8] = {"\x1b[0m","","","\x1b[36m", "\x1b[32m", "", "\x1b[33m", "\x1b[31m" };
	char *console_buffer=NULL;
	char *msj_salida = malloc(sizeof(char) * 256);

	//Captura los argumenas en una lista
	va_list args;
	va_start(args, mensaje);

	//Arma el mensaje formateado con sus argumenas en msj_salida.
	vsprintf(msj_salida, mensaje, args);

	escribir_en_pantalla(tipo_esc, tipo_log, console_buffer, log_colors,
			msj_salida);

	definir_nivel_y_loguear(tipo_esc, tipo_log, msj_salida);

	va_end(args);
	free(msj_salida);

	return;
}
