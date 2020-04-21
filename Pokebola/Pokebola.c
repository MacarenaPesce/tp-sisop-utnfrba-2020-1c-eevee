/*
 * Pokebola.c
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#include "Pokebola.h"

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

void cerrar_conexion(int sock){
	close(sock);
}

int enviar_mensaje(int sock, void *mensaje, int tamanio){
	int bytes_enviados;
	bytes_enviados = send(sock, mensaje, tamanio, 0);
	if (bytes_enviados <= 0) {
		perror("Error en el send");
		return -1;
	}
	return bytes_enviados;
}

int recibir_mensaje(int sock, void *mensaje, int tamanio){
	int bytes_recibidos;
	if((bytes_recibidos = recv(sock, mensaje, tamanio, 0)) < 0) {
		perror("Error en el recv.\n");
		return -1;
	}
	return bytes_recibidos;
}

int enviar_header(int sock, enum MENSAJES tipo_de_mensaje,int size){
	t_header header;
	header.tamanio = size;
	header.tipo_de_mensaje = tipo_de_mensaje;
	return enviar_mensaje(sock, &header, sizeof(header));

}

t_header recibir_header(int sock){
	int resultado = -1;
	t_header header;

	resultado = recibir_mensaje(sock, &header, sizeof(header));

	if(resultado < 0){
		perror("Error al recibir la cabecera.\n");
	}else if (resultado == 0){
		header.tamanio = -1;
	}
	return header;

}

t_paquete* crear_paquete(enum MENSAJES tipo_de_mensaje){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->header.tipo_de_mensaje = tipo_de_mensaje;
	crear_buffer(paquete);
	return paquete;
}

void crear_buffer(t_paquete* paquete){
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->header.tamanio = 0;
	paquete->buffer->stream = NULL;
}

void enviar_paquete(t_paquete* paquete, int socket_client){
	int bytes = paquete->header.tamanio + sizeof(t_header);
	void* a_enviar = serializar_paquete(paquete, bytes);
	enviar_mensaje(socket_client, a_enviar, bytes);
	free(a_enviar);
}

void* serializar_paquete(t_paquete* paquete, int bytes){
	void * mem_serializada = malloc(bytes);
	int desplazamiento = 0;

	memcpy(mem_serializada + desplazamiento, &(paquete->header), sizeof(t_header));
	desplazamiento+= sizeof(t_header);
	memcpy(mem_serializada + desplazamiento, paquete->buffer->stream, paquete->header.tamanio);
	desplazamiento+= paquete->header.tamanio;

	return mem_serializada;
}

void agregar_string_a_paquete(t_paquete* paquete, void* string_value, int size){
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->header.tamanio + size + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->header.tamanio, &size, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->header.tamanio + sizeof(int), string_value, size);

	paquete->header.tamanio += size + sizeof(int);
}

void agregar_int_a_paquete(t_paquete* paquete, int value){
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->header.tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->header.tamanio, &value, sizeof(int));

	paquete->header.tamanio += sizeof(int);
}

void agregar_uint32_t_a_paquete(t_paquete* paquete, uint32_t valor_int){
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->header.tamanio + sizeof(uint32_t));

	memcpy(paquete->buffer->stream + paquete->header.tamanio, &valor_int, sizeof(uint32_t));

	paquete->header.tamanio += sizeof(uint32_t);
}

void eliminar_paquete(t_paquete* paquete){
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void enviar_mensaje_char(char* mensaje, int socket){
	t_paquete* paquete = crear_paquete(CHAR_MESSAGE);
	agregar_string_a_paquete(paquete, mensaje, strlen(mensaje)+1);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

tp_mensaje_char recibir_mensaje_char(int paquete_size, int socket){
	void * buffer = malloc(paquete_size);
	recibir_mensaje(socket, buffer, paquete_size);
	int tamanio_mensaje_char;
	int desplazamiento = 0;
	memcpy(&tamanio_mensaje_char, buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	char* mensaje_char=malloc(tamanio_mensaje_char);
	memcpy(mensaje_char, buffer+desplazamiento, tamanio_mensaje_char);
	tp_mensaje_char contenido=malloc(sizeof(t_mensaje_char));
	contenido->mensaje=mensaje_char;
	free(buffer);
	return contenido;
}

void enviar_appeared_pokemon(char* pokemon, int posx, int posy, int socket){
	t_paquete* paquete = crear_paquete(APPEARED_POKEMON);
	agregar_string_a_paquete(paquete, pokemon, strlen(pokemon)+1);
	agregar_int_a_paquete(paquete, posx);
	agregar_int_a_paquete(paquete, posy);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}


tp_appeared_pokemon recibir_appeared_pokemon(int paquete_size, int socket){
	void * buffer = malloc(paquete_size);
	recibir_mensaje(socket, buffer, paquete_size);
	int tamanio_pokemon;
	int posx;
	int posy;
	int desplazamiento = 0;
	memcpy(&tamanio_pokemon, buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	char* pokemon=malloc(tamanio_pokemon);
	memcpy(pokemon, buffer+desplazamiento, tamanio_pokemon);
	desplazamiento+=tamanio_pokemon;
	memcpy(&posx, buffer + desplazamiento, sizeof(int));
	desplazamiento=tamanio_pokemon+sizeof(int)+sizeof(int);
	memcpy(&posy, buffer + desplazamiento, sizeof(int));
	tp_appeared_pokemon contenido = malloc(sizeof(t_appeared_pokemon));
	contenido->pokemon=pokemon;
	contenido->posx=posx;
	contenido->posy=posy;
	free(buffer);
	return contenido;
}


void enviar_new_pokemon(char* pokemon, int posx, int posy,int cantidad,int server_gamecard){

	t_paquete* paquete = crear_paquete(NEW_POKEMON);
		agregar_string_a_paquete(paquete, pokemon, strlen(pokemon)+1);
		agregar_int_a_paquete(paquete, posx);
		agregar_int_a_paquete(paquete, posy);
		agregar_int_a_paquete(paquete, cantidad);
		enviar_paquete(paquete, socket);
		eliminar_paquete(paquete);
};

tp_new_pokemon recibir_new_pokemon(int paquete_size, int socket){
	void * buffer = malloc(paquete_size);
	recibir_mensaje(socket, buffer, paquete_size);
	int tamanio_pokemon;
	int posx;
	int posy;
	int cantidad;
	int desplazamiento = 0;
	//copiar en el primer parametro lo que hay en el segundo y el tamaño es el tercero
	memcpy(&tamanio_pokemon, buffer + desplazamiento, sizeof(int));
	//me corro bytes
	desplazamiento+=sizeof(int);
	//reservo memo
	char* pokemon=malloc(tamanio_pokemon);
	//copio en 1, lo que hay en 2, cant en bytes 3
	memcpy(pokemon, buffer+desplazamiento, tamanio_pokemon);
	//me corro esas pos
	desplazamiento+=tamanio_pokemon;
	//copio en 1, lo que indica2, cant bytes de 3
	memcpy(&posx, buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	memcpy(&posy, buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	tp_new_pokemon contenido = malloc(sizeof(t_new_pokemon));;
	contenido->pos_en_mapa_poke->pokemon=pokemon;
	contenido->pos_en_mapa_poke->posx=posx;
	contenido->pos_en_mapa_poke->posy=posy;
	contenido->cantidad_en_la_pos=cantidad;
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
	if ((tipo_esc == loguear) || (tipo_esc == escribir_loguear)) {
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
	}
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
