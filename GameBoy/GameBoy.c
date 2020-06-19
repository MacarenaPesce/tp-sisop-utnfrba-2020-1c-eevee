/*
 * GameBoy.c
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#include "GameBoy.h"

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

	log_warning(gameboy_logger, "ATENCIOOOOOOOOOON");
	log_warning(gameboy_logger, "Para permitir la verificación/validación del módulo se exigirá tener un archivo de log específico e independiente que contenga la información");
	log_warning(gameboy_logger, "indicada en esta sección. ​ No se permite la inclusión de otros mensajes y exclusión de ninguno de los mismos (la falta o agregado de alguno puede");
	log_warning(gameboy_logger, "implicar la desaprobación del grupo). Dado que el log será el medio de validación del trabajo práctico ​ se exige no utilizar la consola como medio");
	log_warning(gameboy_logger, "de logueo ​ . Cada operación debe loguearse en una única línea indicando la misma y sus datos, en caso que el grupo desee loguear contenido extra ");
	log_warning(gameboy_logger, "deberá realizarlo en un archivo independiente. Las acciones a loguear en este archivo son:");
	log_warning(gameboy_logger, "1. Conexión a cualquier proceso.");
	log_warning(gameboy_logger, "2. Suscripción a una cola de mensajes.");
	log_warning(gameboy_logger, "3. Llegada de un nuevo mensaje a una cola de mensajes.\n");

	ver_que_me_llego_por_argumento(argv[1],argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
}
