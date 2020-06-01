/*
 * GameBoy.c
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#include "GameBoy.h"

void broker_new_pokemon(char * pokemon, char * posx, char * posy, char * cantidad){
	validar_parametros_cuatro_argumentos(pokemon, posx, posy, cantidad);
	log_info(gameboy_logger,"\nLe voy a mandar a broker los parametros para un nuevo pokemon %s",pokemon);

	t_new_pokemon* new_pokemon = malloc(sizeof(t_new_pokemon));
	new_pokemon->_tamanio_string_pokemon = 0;
	new_pokemon->cantidad = cantidad;
	new_pokemon->coordenadas.posx = atoi(posx);
	new_pokemon->coordenadas.posy = atoi(posy);
	new_pokemon->pokemon = pokemon;

	server_broker = conectar_a_server(ip_broker, puerto_broker);
	enviar_new_pokemon(server_broker, 0, 0, pokemon, new_pokemon);
	log_info(gameboy_logger,"\nYa lo envie");

	free(new_pokemon);

	terminar_gameboy_correctamente();
}

void broker_appeared_pokemon(char* pokemon, char * posx, char * posy, char * id_mensaje){
	validar_parametros_cuatro_argumentos(pokemon, posx, posy, id_mensaje);
	log_info(gameboy_logger,"\nLe voy a mandar a broker los parametros de un appeared pokemon %s",pokemon);

	t_appeared_pokemon* appeared_pokemon = malloc(sizeof(t_appeared_pokemon));
	appeared_pokemon->pokemon = pokemon;
	appeared_pokemon->coordenadas.posx = atoi(posx);
	appeared_pokemon->coordenadas.posy = atoi(posy);
	appeared_pokemon->_tamanio_string_pokemon = 0;

	server_broker = conectar_a_server(ip_broker, puerto_broker);
	enviar_appeared_pokemon(server_broker, atoi(id_mensaje), 0, appeared_pokemon);
	log_info(gameboy_logger,"\nYa lo envie");

	free(appeared_pokemon);

	terminar_gameboy_correctamente();
}

void broker_catch_pokemon(char * pokemon, char* posx, char* posy){
	validar_parametros_tres_argumentos(pokemon, posx, posy);
	log_info(gameboy_logger,"\nLe voy a mandar a broker los parametros para un catch pokemon %s",pokemon);

	t_catch_pokemon* appeared_pokemon = malloc(sizeof(t_catch_pokemon));
	appeared_pokemon->pokemon = pokemon;
	appeared_pokemon->coordenadas.posx = atoi(posx);
	appeared_pokemon->coordenadas.posy = atoi(posy);
	appeared_pokemon->_tamanio_string_pokemon = 0;

	server_broker = conectar_a_server(ip_broker, puerto_broker);
	enviar_catch_pokemon(server_broker, 0, 0, appeared_pokemon);

	free(appeared_pokemon);

	log_info(gameboy_logger,"\nYa lo envie");
	terminar_gameboy_correctamente();
}

void broker_caught_pokemon(char * id_mensaje, char *ok_or_fail){
	validar_parametros_dos_argumentos(id_mensaje, ok_or_fail);
	log_info(gameboy_logger,"\nLe voy a mandar a broker los parametros para un caught pokemon %s");

	t_caught_pokemon* caught_pokemon = malloc(sizeof(t_caught_pokemon));
	caught_pokemon->status = atoi(ok_or_fail);

	server_broker = conectar_a_server(ip_broker, puerto_broker);
	enviar_caught_pokemon(server_broker, atoi(id_mensaje), 0, caught_pokemon);
	log_info(gameboy_logger,"\nYa lo envie");

	free(caught_pokemon);

	terminar_gameboy_correctamente();
}

void broker_get_pokemon(char * pokemon){
	if(pokemon==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}else{
		log_info(gameboy_logger,"\n Envio a broker el pokemon %s", pokemon);

		t_get_pokemon * get_pokemon = malloc(sizeof(t_get_pokemon));
		get_pokemon->pokemon = pokemon;
		get_pokemon->_tamanio_string_pokemon = 0;

		server_broker= conectar_a_server(ip_broker, puerto_broker);
		enviar_get_pokemon(server_broker, 0, 0, get_pokemon);
		log_info(gameboy_logger,"\nYa lo envie");

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
	log_info(gameboy_logger,"\nLe voy a mandar a team las coordenadas del pokemon %s",pokemon);

	t_appeared_pokemon* appeared_pokemon = malloc(sizeof(t_appeared_pokemon));
	appeared_pokemon->pokemon = pokemon;
	appeared_pokemon->coordenadas.posx = atoi(posx);
	appeared_pokemon->coordenadas.posy = atoi(posy);
	appeared_pokemon->_tamanio_string_pokemon = 0;

	server_team = conectar_a_server(ip_team, puerto_team);
	enviar_appeared_pokemon(server_team, 0, 0, appeared_pokemon);
	log_info(gameboy_logger,"\nYa lo envie");

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

void gamecard_new_pokemon(char * pokemon, char * posx, char * posy, char * cantidad){

	validar_parametros_cuatro_argumentos(pokemon,posx,posy,cantidad);
	log_info(gameboy_logger,"\n Envio a gamecard la posicion en x -> %s, la posicion en y -> %s, con la cantidad %s del pokemon %s a crear en el mapa",
			posx,posy,cantidad,pokemon);

	t_new_pokemon* new_pokemon = malloc(sizeof(t_new_pokemon));
	new_pokemon->_tamanio_string_pokemon = 0;
	new_pokemon->cantidad = cantidad;
	new_pokemon->coordenadas.posx = atoi(posx);
	new_pokemon->coordenadas.posy = atoi(posy);
	new_pokemon->pokemon = pokemon;

	server_gamecard= conectar_a_server(ip_gamecard, puerto_gamecard);
	enviar_new_pokemon(server_gamecard, 0, 0, pokemon, new_pokemon);
	log_info(gameboy_logger,"\nYa lo envie");

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

void gamecard_catch_pokemon(char * pokemon, char* posx, char* posy){

	validar_parametros_tres_argumentos(pokemon,posx,posy);
	log_info(gameboy_logger,"\n Envio a gamecard la posicion en x -> %s, la posicion en y -> %s, del pokemon %s a crear en el mapa",posx,posy,pokemon);

	t_catch_pokemon* appeared_pokemon = malloc(sizeof(t_catch_pokemon));
	appeared_pokemon->pokemon = pokemon;
	appeared_pokemon->coordenadas.posx = atoi(posx);
	appeared_pokemon->coordenadas.posy = atoi(posy);
	appeared_pokemon->_tamanio_string_pokemon = 0;

	server_gamecard= conectar_a_server(ip_gamecard, puerto_gamecard);
	enviar_catch_pokemon(server_gamecard, 0, 0, appeared_pokemon);
	log_info(gameboy_logger,"\nYa lo envie");

	free(appeared_pokemon);

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


void gamecard_get_pokemon(char * pokemon){
	if(pokemon==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}else{
		log_info(gameboy_logger,"\n Envio a gamecard el pokemon %s", pokemon);

		t_get_pokemon * get_pokemon = malloc(sizeof(t_get_pokemon));
		get_pokemon->pokemon = pokemon;
		get_pokemon->_tamanio_string_pokemon = 0;

		server_gamecard= conectar_a_server(ip_gamecard, puerto_gamecard);
		enviar_get_pokemon(server_gamecard, 0, 0, get_pokemon);
		log_info(gameboy_logger,"\nYa lo envie");

		free(get_pokemon);

		log_info(gameboy_logger,"\nYa lo envie");
		terminar_gameboy_correctamente();
	}
}

void consola_gamecard(char * parametro1, char * parametro2, char * parametro3, char * parametro4, char * parametro5){
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
			gamecard_new_pokemon(parametro2,parametro3,parametro4,parametro5);
			break;
		case appeared_pokemon:
			mostrar_mensaje_de_error();
			break;
		case catch_pokemon:
			gamecard_catch_pokemon(parametro2,parametro3,parametro4);
			break;
		case caught_pokemon:
			mostrar_mensaje_de_error();
			break;
		case get_pokemon:
			gamecard_get_pokemon(parametro2);
			break;

		default:
			break;
	}
}

void consola_suscriptor(char* cola_de_mensajes, char* tiempo){
/*

	 TODO: MODIFICAR

	server_gamecard= conectar_a_server(ip_gamecard, puerto_gamecard);
	int key = -1;

	if(!strcmp(cola_de_mensajes, "COLA_NEW_POKEMON"))
		key = new_pokemon;
	if(!strcmp(cola_de_mensajes, "COLA_APPEARED_POKEMON"))
		key = appeared_pokemon;
	if(!strcmp(cola_de_mensajes, "COLA_CATCH_POKEMON"))
		key = catch_pokemon;
	if(!strcmp(cola_de_mensajes, "COLA_CAUGHT_POKEMON"))
		key = caught_pokemon;
	if(!strcmp(cola_de_mensajes, "COLA_GET_POKEMON"))
		key = get_pokemon;
	if(!strcmp(cola_de_mensajes, "COLA_LOCALIZED_POKEMON"))
		key = localized_pokemon;

	switch(key){
		case new_pokemon:
			enviar_modo_suscriptor(COLA_NEW_POKEMON, atoi(tiempo), server_broker);
			break;
		case appeared_pokemon:
			enviar_modo_suscriptor(COLA_APPEARED_POKEMON, atoi(tiempo), server_broker);
			break;
		case catch_pokemon:
			enviar_modo_suscriptor(COLA_CATCH_POKEMON, atoi(tiempo), server_broker);
			break;
		case caught_pokemon:
			enviar_modo_suscriptor(COLA_CAUGHT_POKEMON, atoi(tiempo), server_broker);
			break;
		case get_pokemon:
			enviar_modo_suscriptor(COLA_GET_POKEMON, atoi(tiempo), server_broker);
			break;
		case localized_pokemon:
			enviar_modo_suscriptor(COLA_LOCALIZED_POKEMON, atoi(tiempo), server_broker);
			break;

		default:
			break;
	}

	log_info(gameboy_logger,"\nYa lo envie");
	terminar_gameboy_correctamente();
*/
}

void consola_modo_suscriptor(char* parametro1, char* parametro2){
	if(parametro2==NULL){
		mostrar_mensaje_de_error();
	}else{
		consola_suscriptor(parametro1,parametro2);
	}
	terminar_gameboy_correctamente();
}

void consola_derivar_comando(char* parametro0, char* parametro1, char* parametro2, char* parametro3, char* parametro4, char* parametro5){

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
			consola_gamecard(parametro1,parametro2,parametro3,parametro4,parametro5);
			break;
		case suscriptor:
			consola_modo_suscriptor(parametro1, parametro2);
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


void ver_que_me_llego_por_argumento(char* parametro0, char* parametro1, char* parametro2, char* parametro3, char* parametro4, char* parametro5){
	if(parametro0!=NULL){
		consola_derivar_comando(parametro0, parametro1, parametro2, parametro3, parametro4, parametro5);
	}else{
		log_info(gameboy_logger,"No me pediste nada...");
	}
}

int main(int argc, char **argv){

	inicializar_logger();
	inicializar_archivo_de_configuracion();
	configurar_signals();

	ver_que_me_llego_por_argumento(argv[1],argv[2], argv[3], argv[4], argv[5], argv[6]);
}
