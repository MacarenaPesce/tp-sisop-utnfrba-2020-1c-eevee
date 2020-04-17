/*
 * Pokebola.c
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#include "Pokebola.h"

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
