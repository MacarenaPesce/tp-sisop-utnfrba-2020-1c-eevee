/*
 * GameBoy.c
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#include "GameBoy.h"

void broker_new_pokemon(char * pokemon, char * posx, char * posy, char * cantidad){
	validar_parametros_cuatro_argumentos(pokemon, posx, posy, cantidad);
	log_info(gameboy_logger,"Le voy a mandar a broker los parametros para un nuevo pokemon %s",pokemon);

	t_new_pokemon* new_pokemon = malloc(sizeof(t_new_pokemon));
	new_pokemon->cantidad = (uint32_t)atoi(cantidad);
	new_pokemon->coordenadas.posx = (uint32_t)atoi(posx);
	new_pokemon->coordenadas.posy = (uint32_t)atoi(posy);
	new_pokemon->pokemon = pokemon;

	int socket =  conectar_a_server(ip_broker,puerto_broker);

	if(socket < 0){
		log_info(gameboy_logger,"Broker caído");
	}else{
		int status = distribuir_new_pokemon(socket, -1, -1, new_pokemon);
		if(status < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de new pokemon enviado correctamente");
			close(socket);
		}
	}

	free(new_pokemon);
	terminar_gameboy_correctamente();
}

void broker_appeared_pokemon(char* pokemon, char * posx, char * posy, char * id_mensaje_correlativo){
	validar_parametros_cuatro_argumentos(pokemon, posx, posy, id_mensaje_correlativo);
	log_info(gameboy_logger,"Le voy a mandar a broker los parametros de un appeared pokemon %s",pokemon);

	t_appeared_pokemon* appeared_pokemon = malloc(sizeof(t_appeared_pokemon));
	appeared_pokemon->pokemon = pokemon;
	appeared_pokemon->coordenadas.posx = (uint32_t)atoi(posx);
	appeared_pokemon->coordenadas.posy = (uint32_t)atoi(posy);

	int socket =  conectar_a_server(ip_broker,puerto_broker);

	if(socket < 0){
		log_info(gameboy_logger,"Broker caído");
	}else{
		int status = distribuir_appeared_pokemon(socket, -1, (uint32_t)atoi(id_mensaje_correlativo), appeared_pokemon);
		if(status < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de appeared pokemon enviado correctamente");
			close(socket);
		}
	}

	free(appeared_pokemon);
	terminar_gameboy_correctamente();
}

void broker_catch_pokemon(char * pokemon, char* posx, char* posy){
	validar_parametros_tres_argumentos(pokemon, posx, posy);
	log_info(gameboy_logger,"Le voy a mandar a broker los parametros para un catch pokemon %s",pokemon);

	t_catch_pokemon* appeared_pokemon = malloc(sizeof(t_catch_pokemon));
	appeared_pokemon->pokemon = pokemon;
	appeared_pokemon->coordenadas.posx = (uint32_t)atoi(posx);
	appeared_pokemon->coordenadas.posy = (uint32_t)atoi(posy);

	int socket =  conectar_a_server(ip_broker,puerto_broker);

	if(socket < 0){
		log_info(gameboy_logger,"Broker caído");
	}else{
		int status = distribuir_catch_pokemon(socket, -1, -1, appeared_pokemon);
		if(status < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de catch pokemon enviado correctamente");
			close(socket);
		}
	}

	free(appeared_pokemon);
	terminar_gameboy_correctamente();
}

void broker_caught_pokemon(char * id_mensaje_correlativo, char *ok_or_fail){
	validar_parametros_dos_argumentos(id_mensaje_correlativo, ok_or_fail);
	log_info(gameboy_logger,"Le voy a mandar a broker los parametros para un caught pokemon %s");

	t_caught_pokemon* caught_pokemon = malloc(sizeof(t_caught_pokemon));
	caught_pokemon->status = (uint32_t)atoi(ok_or_fail);

	int socket =  conectar_a_server(ip_broker,puerto_broker);

	if(socket < 0){
		log_info(gameboy_logger,"Broker caído");
	}else{
		int status = distribuir_caught_pokemon(socket, -1, (uint32_t)atoi(id_mensaje_correlativo), caught_pokemon);
		if(status < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de caught pokemon enviado correctamente");
			close(socket);
		}
	}

	free(caught_pokemon);
	terminar_gameboy_correctamente();
}

void broker_get_pokemon(char * pokemon){
	if(pokemon==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}else{
		log_info(gameboy_logger," Envio a broker el pokemon %s", pokemon);

		t_get_pokemon * get_pokemon = malloc(sizeof(t_get_pokemon));
		get_pokemon->pokemon = pokemon;

		int socket =  conectar_a_server(ip_broker,puerto_broker);

		if(socket < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			int status = distribuir_get_pokemon(socket, -1, -1, get_pokemon);
			if(status < 0){
				log_info(gameboy_logger,"Broker caído");
			}else{
				log_info(gameboy_logger,"Pedido de get pokemon enviado correctamente");
				close(socket);
			}
		}
		free(get_pokemon);
		terminar_gameboy_correctamente();
	}
}

void consola_broker(char * parametro1, char * parametro2, char * parametro3, char * parametro4, char * parametro5){
	int key = -1;

	if(!strcmp(parametro1, "NEW_POKEMON"))
		key = new_pokemon;
	if(!strcmp(parametro1, "APPEARED_POKEMON"))
		key = appeared_pokemon;
	if(!strcmp(parametro1, "CATCH_POKEMON"))
		key = catch_pokemon;
	if(!strcmp(parametro1, "CAUGHT_POKEMON"))
		key = caught_pokemon;
	if(!strcmp(parametro1, "GET_POKEMON"))
		key = get_pokemon;

	switch(key){
		case new_pokemon:
			broker_new_pokemon(parametro2,parametro3,parametro4,parametro5);
			break;
		case appeared_pokemon:
			broker_appeared_pokemon(parametro2,parametro3,parametro4,parametro5);
			break;
		case catch_pokemon:
			broker_catch_pokemon(parametro2,parametro3,parametro4);
			break;
		case caught_pokemon:
			broker_caught_pokemon(parametro2,parametro3);
			break;
		case get_pokemon:
			broker_get_pokemon(parametro2);
			break;

		default:
			break;
	}
}

void mostrar_mensaje_de_error(){
	log_info(gameboy_logger,"Los argumentos ingresados no son validos o te faltan parametros, volve a intentar");
}

void team_appeared_pokemon(char* pokemon, char* posx, char* posy){
	validar_parametros_tres_argumentos(pokemon, posx, posy);
	log_info(gameboy_logger,"Le voy a mandar a team las coordenadas del pokemon %s",pokemon);

	t_appeared_pokemon* appeared_pokemon = malloc(sizeof(t_appeared_pokemon));
	appeared_pokemon->pokemon = pokemon;
	appeared_pokemon->coordenadas.posx = (uint32_t)atoi(posx);
	appeared_pokemon->coordenadas.posy = (uint32_t)atoi(posy);

	int socket =  conectar_a_server(ip_team,puerto_team);

	if(socket < 0){
		log_info(gameboy_logger,"TEAM caído");
	}else{
		int status = distribuir_appeared_pokemon(socket, -1, -1, appeared_pokemon);
		if(status < 0){
			log_info(gameboy_logger,"TEAM caído");
		}else{
			log_info(gameboy_logger,"Pedido de appeared pokemon enviado correctamente");
			close(socket);
		}
	}

	free(appeared_pokemon);
	terminar_gameboy_correctamente();
}

void consola_team(char * parametro1, char * parametro2, char * parametro3, char * parametro4){
	int key = -1;

	if(!strcmp(parametro1, "NEW_POKEMON"))
		key = new_pokemon;
	if(!strcmp(parametro1, "APPEARED_POKEMON"))
		key = appeared_pokemon;
	if(!strcmp(parametro1, "CATCH_POKEMON"))
		key = catch_pokemon;
	if(!strcmp(parametro1, "CAUGHT_POKEMON"))
		key = caught_pokemon;
	if(!strcmp(parametro1, "GET_POKEMON"))
		key = get_pokemon;

	switch(key){
		case new_pokemon:
			mostrar_mensaje_de_error();
			break;
		case appeared_pokemon:
			team_appeared_pokemon(parametro2,parametro3,parametro4);
			break;
		case catch_pokemon:
			mostrar_mensaje_de_error();
			break;
		case caught_pokemon:
			mostrar_mensaje_de_error();
			break;
		case get_pokemon:
			mostrar_mensaje_de_error();
			break;

		default:
			break;
	}
}

void gamecard_new_pokemon(char * pokemon, char * posx, char * posy, char * cantidad, char * id_mensaje_correlativo){

	validar_parametros_cinco_argumentos(pokemon,posx,posy,cantidad,id_mensaje_correlativo);
	log_info(gameboy_logger," Envio a gamecard la posicion en x -> %s, la posicion en y -> %s, con la cantidad %s del pokemon %s a crear en el mapa y id_correlativo %s",
			posx,posy,cantidad,pokemon,id_mensaje_correlativo);

	t_new_pokemon* new_pokemon = malloc(sizeof(t_new_pokemon));
	new_pokemon->cantidad = (uint32_t)atoi(cantidad);
	new_pokemon->coordenadas.posx = (uint32_t)atoi(posx);
	new_pokemon->coordenadas.posy = (uint32_t)atoi(posy);
	new_pokemon->pokemon = pokemon;

	int socket =  conectar_a_server(ip_gamecard,puerto_gamecard);

	if(socket < 0){
		log_info(gameboy_logger,"Gamecard caído");
	}else{
		int status = distribuir_new_pokemon(socket, -1, (uint32_t)atoi(id_mensaje_correlativo), new_pokemon);
		if(status < 0){
			log_info(gameboy_logger,"Gamecard caído");
		}else{
			log_info(gameboy_logger,"Pedido de new pokemon enviado correctamente");
			close(socket);
		}
	}

	free(new_pokemon);
	terminar_gameboy_correctamente();
}

void validar_parametros_dos_argumentos(char* arg1, char* arg2){
	if(arg1==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}
	if(arg2==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}
}

void validar_parametros_cuatro_argumentos(char* arg1,char* arg2,char* arg3,char* arg4){

	if(arg1==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}
	if(arg2==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}
	if(arg3==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}
	if(arg4==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}

}

void validar_parametros_cinco_argumentos(char* arg1,char* arg2,char* arg3,char* arg4,char* arg5){

	if(arg1==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}
	if(arg2==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}
	if(arg3==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}
	if(arg4==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}
	if(arg5==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}

}

void gamecard_catch_pokemon(char * pokemon, char* posx, char* posy, char * id_mensaje){

	validar_parametros_cuatro_argumentos(pokemon,posx,posy,id_mensaje);
	log_info(gameboy_logger," Envio a gamecard la posicion en x -> %s, la posicion en y -> %s, del pokemon %s a crear en el mapa, y id_mensaje %s",posx,posy,pokemon,id_mensaje);

	t_catch_pokemon* appeared_pokemon = malloc(sizeof(t_catch_pokemon));
	appeared_pokemon->pokemon = pokemon;
	appeared_pokemon->coordenadas.posx = (uint32_t)atoi(posx);
	appeared_pokemon->coordenadas.posy = (uint32_t)atoi(posy);

	int socket =  conectar_a_server(ip_gamecard,puerto_gamecard);

	if(socket < 0){
		log_info(gameboy_logger,"Gamecard caído");
	}else{
		int status = distribuir_catch_pokemon(socket, (uint32_t)atoi(id_mensaje), -1, appeared_pokemon);
		if(status < 0){
			log_info(gameboy_logger,"Gamecard caído");
		}else{
			log_info(gameboy_logger,"Pedido de appeared pokemon enviado correctamente");
			close(socket);
		}
	}

	free(appeared_pokemon);;
	terminar_gameboy_correctamente();
}

void validar_parametros_tres_argumentos(char* arg1,char* arg2,char* arg3){

	if(arg1==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}
	if(arg2==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}
	if(arg3==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}
}

void gamecard_get_pokemon(char * pokemon, char * id_mensaje){
	validar_parametros_dos_argumentos(pokemon, id_mensaje);
	log_info(gameboy_logger," Envio a gamecard el pokemon %s y id_mensaje %s", pokemon, id_mensaje);

	t_get_pokemon * get_pokemon = malloc(sizeof(t_get_pokemon));
	get_pokemon->pokemon = pokemon;

	int socket =  conectar_a_server(ip_gamecard,puerto_gamecard);

	if(socket < 0){
		log_info(gameboy_logger,"Gamecard caído");
	}else{
		int status = distribuir_get_pokemon(socket, (uint32_t)atoi(id_mensaje), -1, get_pokemon);
		if(status < 0){
			log_info(gameboy_logger,"Gamecard caído");
		}else{
			log_info(gameboy_logger,"Pedido de get pokemon enviado correctamente");
			close(socket);
		}
	}

	free(get_pokemon);
	terminar_gameboy_correctamente();
}

void consola_gamecard(char * parametro1, char * parametro2, char * parametro3, char * parametro4, char * parametro5, char* parametro6){
	int key = -1;

	if(!strcmp(parametro1, "NEW_POKEMON"))
		key = new_pokemon;
	if(!strcmp(parametro1, "APPEARED_POKEMON"))
		key = appeared_pokemon;
	if(!strcmp(parametro1, "CATCH_POKEMON"))
		key = catch_pokemon;
	if(!strcmp(parametro1, "CAUGHT_POKEMON"))
		key = caught_pokemon;
	if(!strcmp(parametro1, "GET_POKEMON"))
		key = get_pokemon;

	switch(key){
		case new_pokemon:
			gamecard_new_pokemon(parametro2,parametro3,parametro4,parametro5,parametro6);
			break;
		case appeared_pokemon:
			mostrar_mensaje_de_error();
			break;
		case catch_pokemon:
			gamecard_catch_pokemon(parametro2,parametro3,parametro4,parametro5);
			break;
		case caught_pokemon:
			mostrar_mensaje_de_error();
			break;
		case get_pokemon:
			gamecard_get_pokemon(parametro2,parametro3);
			break;

		default:
			break;
	}
}

void consola_suscriptor(char* cola_de_mensajes, char* tiempo){

	validar_parametros_dos_argumentos(cola_de_mensajes, tiempo);
	log_info(gameboy_logger," Envio a broker la solicitud de suscripcion para la cola %s y el tiempo %s",cola_de_mensajes, tiempo);

	t_servidor * servidor = malloc(sizeof(t_servidor));
	servidor->ip = ip_broker;
	servidor->puerto = puerto_broker;

	t_suscripcion * suscripcion = malloc(sizeof(t_suscripcion));
	suscripcion->tipo_suscripcion = SUSCRIPCION_TEMPORAL;
	suscripcion->minutos_suscripcion = (uint32_t)atoi(tiempo);

	if(string_equals_ignore_case(cola_de_mensajes, "COLA_NEW_POKEMON")){
		int solicitud = enviar_solicitud_suscripcion(servidor,COLA_NEW_POKEMON,suscripcion);
		if(solicitud < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de solicitud de suscripcion a la cola NEW_POKEMON enviado correctamente");
		}
	}

	if(string_equals_ignore_case(cola_de_mensajes, "COLA_APPEARED_POKEMON")){
		int solicitud = enviar_solicitud_suscripcion(servidor,COLA_APPEARED_POKEMON,suscripcion);
		if(solicitud < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de solicitud de suscripcion a la cola APPEARED_POKEMON enviado correctamente");
		}
	}

	if(string_equals_ignore_case(cola_de_mensajes, "COLA_CATCH_POKEMON")){
		int solicitud = enviar_solicitud_suscripcion(servidor,COLA_CATCH_POKEMON,suscripcion);
		if(solicitud < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de solicitud de suscripcion a la cola CATCH_POKEMON enviado correctamente");
		}
	}

	if(string_equals_ignore_case(cola_de_mensajes, "COLA_CAUGHT_POKEMON")){
		int solicitud = enviar_solicitud_suscripcion(servidor,COLA_CAUGHT_POKEMON,suscripcion);
		if(solicitud < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de solicitud de suscripcion a la cola CAUGHT_POKEMON enviado correctamente");
		}
	}

	if(string_equals_ignore_case(cola_de_mensajes, "COLA_GET_POKEMON")){
		int solicitud = enviar_solicitud_suscripcion(servidor,COLA_GET_POKEMON,suscripcion);
		if(solicitud < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de solicitud de suscripcion a la cola GET_POKEMON enviado correctamente");
		}
	}

	if(string_equals_ignore_case(cola_de_mensajes, "COLA_LOCALIZED_POKEMON")){
		int solicitud = enviar_solicitud_suscripcion(servidor,COLA_LOCALIZED_POKEMON,suscripcion);
		if(solicitud < 0){
			log_info(gameboy_logger,"Broker caído");
		}else{
			log_info(gameboy_logger,"Pedido de solicitud de suscripcion a la cola LOCALIZED_POKEMON enviado correctamente");
		}
	}

	free(servidor);
	free(suscripcion);

	terminar_gameboy_correctamente();
}

void consola_derivar_comando(char* parametro0, char* parametro1, char* parametro2, char* parametro3, char* parametro4, char* parametro5, char* parametro6){

	int comando_key;

	// Obtiene la clave del comando a ejecutar para el switch
	comando_key = consola_obtener_key_comando(parametro0);

	switch(comando_key){
		case broker:
			consola_broker(parametro1,parametro2,parametro3,parametro4,parametro5);
			break;
		case team:
			consola_team(parametro1,parametro2,parametro3,parametro4);
			break;
		case gamecard:
			consola_gamecard(parametro1,parametro2,parametro3,parametro4,parametro5, parametro6);
			break;
		case suscriptor:
			consola_suscriptor(parametro1, parametro2);
			break;
		default:
			mostrar_mensaje_de_error();
			break;
	}

}

int consola_obtener_key_comando(char* comando){
	int key = -1;

	if(comando == NULL)
		return key;

	if(!strcmp(comando, "BROKER"))
		key = broker;

	if(!strcmp(comando, "TEAM"))
		key = team;

	if(!strcmp(comando, "GAMECARD"))
		key = gamecard;

	if(!strcmp(comando, "SUSCRIPTOR"))
		key = suscriptor;

	return key;
}

void ver_que_me_llego_por_argumento(char* parametro0, char* parametro1, char* parametro2, char* parametro3, char* parametro4, char* parametro5, char* parametro6){
	if(parametro0!=NULL){
		consola_derivar_comando(parametro0, parametro1, parametro2, parametro3, parametro4, parametro5, parametro6);
	}else{
		log_info(gameboy_logger,"No me pediste nada...");
	}
}

int main(int argc, char **argv){

	inicializar_logger();
	inicializar_archivo_de_configuracion();
	configurar_signals();

	ver_que_me_llego_por_argumento(argv[1],argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
}
