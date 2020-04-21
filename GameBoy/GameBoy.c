/*
 * GameBoy.c
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#include "GameBoy.h"

void broker_new_pokemon(char * pokemon, char * posx, char * posy, char * cantidad){
	/*
	 *aca se envia el mensaje de new pokemon al broker con sus respectivos argumentos
	 */
}

void broker_appeared_pokemon(char* pokemon, char * posx, char * posy, char * id_mensaje){
	/*
	 *aca se envia el mensaje de appeared pokemon al broker con sus respectivos argumentos
	 */
}

void broker_catch_pokemon(char * pokemon, char* posx, char* posy){
	/*
	 *aca se envia el mensaje de catch pokemon al broker con sus respectivos argumentos
	 */
}

void broker_caught_pokemon(char * id_mensaje, char *ok_or_fail){
	/*
	 *aca se envia el mensaje de caught pokemon al broker con sus respectivos argumentos
	 */
}

void broker_get_pokemon(char * pokemon){
	/*
	 *aca se envia el mensaje de get pokemon al broker con el pokemon
	 */
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
	logger(escribir_loguear,l_error,"Los argumentos ingresados no son validos o te faltan parametros, volve a intentar");
}

void team_appeared_pokemon(char* pokemon, char* posx, char* posy){
	chequear_que_los_argumentos_son_validos_para_team(pokemon, posx, posy);

	logger(escribir_loguear,l_info,"\nLe voy a mandar a team las coordenadas del pokemon %s",pokemon);
	server_team = conectar_a_server(ip_team, puerto_team);
	enviar_appeared_pokemon(pokemon, atoi(posx), atoi(posy), server_team);
	logger(escribir_loguear,l_info,"\nYa lo envie");
	terminar_gameboy_correctamente();
}

void chequear_que_los_argumentos_son_validos_para_team(char * parametro1, char * parametro2, char * parametro3){
	if(parametro1==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}
	if(parametro2==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}
	if(parametro3==NULL){
		mostrar_mensaje_de_error();
		terminar_gameboy_correctamente();
	}
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
	/*
	 *aca se envia el mensaje de new pokemon a gamecard con sus respectivos argumentos
	 */
}

void gamecard_catch_pokemon(char * pokemon, char* posx, char* posy){
	/*
	 *aca se envia el mensaje de catch pokemon a gamecard con sus respectivos argumentos
	 */
}

void gamecard_get_pokemon(char * pokemon){
	/*
	 *aca se envia el mensaje de get pokemon a gamecard con el pokemon
	 */
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
	 * hacer que haga de suscriptor
	 */
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
			consola_suscriptor(parametro1,parametro2);
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
		logger(escribir_loguear,l_warning,"No me pediste nada...");
	}
}

int main(int argc, char **argv){

	inicializar_logger();
	inicializar_archivo_de_configuracion();
	configurar_signals();


	ver_que_me_llego_por_argumento(argv[1],argv[2], argv[3], argv[4], argv[5], argv[6]);

}
