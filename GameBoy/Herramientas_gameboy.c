/*
 * Herramientas_gameboy.c
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#include "Herramientas_gameboy.h"

void inicializar_logger(){
	gameboy_logger = log_create("../logs/gameboy.log", "Gameboy", 1, LOG_LEVEL_DEBUG);
}

void inicializar_archivo_de_configuracion(){
	config = config_create("../gameboy.config");
	if(config == NULL){
		log_info(gameboy_logger,"El archivo de configuracion no existe");
		terminar_gameboy_correctamente();
	}else{
		//log_info(gameboy_logger,"Cargando el archivo de configuracion...");
		obtener_valor_config(KEY_CONFIG_IP_BROKER, config, obtener_la_ip_del_broker);
		obtener_valor_config(KEY_CONFIG_IP_TEAM, config, obtener_la_ip_de_team);
		obtener_valor_config(KEY_CONFIG_IP_GAMECARD, config, obtener_la_ip_de_gamecard);
		obtener_valor_config(KEY_CONFIG_PUERTO_BROKER, config, obtener_el_puerto_del_broker);
		obtener_valor_config(KEY_CONFIG_PUERTO_TEAM, config, obtener_el_puerto_de_team);
		obtener_valor_config(KEY_CONFIG_PUERTO_GAMECARD, config, obtener_el_puerto_de_gamecard);
		obtener_valor_config(KEY_CONFIG_ID, config, obtener_el_id);

		//log_info(gameboy_logger,"Archivo de configuracion cargado correctamente :)\n");
		config_destroy(config);
	}
}

void obtener_valor_config(char* key, t_config* file, void(*obtener)(void)){
	if (config_has_property(file, key)){
		obtener();
	}
}

void obtener_la_ip_del_broker(){
	ip_broker = strdup(config_get_string_value(config, KEY_CONFIG_IP_BROKER));
	//log_info(gameboy_logger,"La ip del broker es: %s",ip_broker);
}

void obtener_la_ip_de_team(){
	ip_team = strdup(config_get_string_value(config, KEY_CONFIG_IP_TEAM));
	//log_info(gameboy_logger,"La ip de team es: %s",ip_team);
}

void obtener_la_ip_de_gamecard(){
	ip_gamecard = strdup(config_get_string_value(config, KEY_CONFIG_IP_GAMECARD));
	//log_info(gameboy_logger,"La ip del gamecard es: %s",ip_gamecard);
}

void obtener_el_puerto_del_broker(){
	puerto_broker = strdup(config_get_string_value(config, KEY_CONFIG_PUERTO_BROKER));
	//log_info(gameboy_logger,"El puerto del broker es: %s",puerto_broker);
}

void obtener_el_puerto_de_team(){
	puerto_team = strdup(config_get_string_value(config, KEY_CONFIG_PUERTO_TEAM));
	//log_info(gameboy_logger,"El puerto de team es: %s",puerto_team);
}

void obtener_el_puerto_de_gamecard(){
	puerto_gamecard = strdup(config_get_string_value(config, KEY_CONFIG_PUERTO_GAMECARD));
	//log_info(gameboy_logger,"El puerto del gamecard es: %s",puerto_gamecard);
}

void obtener_el_id(){
	id = config_get_int_value(config, KEY_CONFIG_ID);
	//log_info(gameboy_logger,"Mi ID es: %d",id);
}


void configurar_signals(void){
	struct sigaction signal_struct;
	signal_struct.sa_handler = capturar_signal;
	signal_struct.sa_flags = 0;

	sigemptyset(&signal_struct.sa_mask);

	sigaddset(&signal_struct.sa_mask, SIGPIPE);
	if (sigaction(SIGPIPE, &signal_struct, NULL) < 0) {
		//log_info(gameboy_logger," SIGACTION error ");
	}

	sigaddset(&signal_struct.sa_mask, SIGINT);
	if (sigaction(SIGINT, &signal_struct, NULL) < 0) {
		//log_info(gameboy_logger, " SIGACTION error ");
	}
	sigaddset(&signal_struct.sa_mask, SIGSEGV);
	if (sigaction(SIGINT, &signal_struct, NULL) < 0) {
		//log_info(gameboy_logger," SIGACTION error ");
	}
}

void capturar_signal(int signo){

    if(signo == SIGINT){
    	terminar_gameboy_correctamente();
    }
    else if(signo == SIGPIPE){
    	log_info(gameboy_logger,"Desconectado");
    }

}

void terminar_gameboy_correctamente(){
	log_destroy(gameboy_logger);
	exit(EXIT_SUCCESS);
}
