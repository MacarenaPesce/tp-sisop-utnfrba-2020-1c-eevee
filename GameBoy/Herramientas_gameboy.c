/*
 * Herramientas_gameboy.c
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#include "Herramientas_gameboy.h"

void inicializar_logger(){
	gameboy_logger = log_create("gameboy.log", "Gameboy", 1, LOG_LEVEL_DEBUG);
	logger(escribir_loguear,l_info,"Hi, bienvenido a Gameboy");
}

void inicializar_archivo_de_configuracion(){
	config = config_create("gameboy.config");
	if(config == NULL){
		logger(escribir_loguear,l_error,"El archivo de configuracion no existe. Fijate en la carpeta Debug.");
		terminar_gameboy_correctamente();
	}else{
		logger(escribir_loguear,l_info,"Cargando el archivo de configuracion...");
		obtener_valor_config(KEY_CONFIG_IP_BROKER, config, obtener_la_ip_del_broker);
		obtener_valor_config(KEY_CONFIG_IP_TEAM, config, obtener_la_ip_de_team);
		obtener_valor_config(KEY_CONFIG_IP_GAMECARD, config, obtener_la_ip_de_gamecard);
		obtener_valor_config(KEY_CONFIG_PUERTO_BROKER, config, obtener_el_puerto_del_broker);
		obtener_valor_config(KEY_CONFIG_PUERTO_TEAM, config, obtener_el_puerto_de_team);
		obtener_valor_config(KEY_CONFIG_PUERTO_GAMECARD, config, obtener_el_puerto_de_gamecard);

		logger(escribir_loguear,l_info,"\nArchivo de configuracion cargado correctamente :)");
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
	logger(escribir_loguear,l_debug,"La ip del broker es: %s",ip_broker);
}

void obtener_la_ip_de_team(){
	ip_team = strdup(config_get_string_value(config, KEY_CONFIG_IP_TEAM));
	logger(escribir_loguear,l_debug,"La ip de team es: %s",ip_team);
}

void obtener_la_ip_de_gamecard(){
	ip_gamecard = strdup(config_get_string_value(config, KEY_CONFIG_IP_GAMECARD));
	logger(escribir_loguear,l_debug,"La ip del gamecard es: %s",ip_gamecard);
}

void obtener_el_puerto_del_broker(){
	puerto_broker = strdup(config_get_string_value(config, KEY_CONFIG_PUERTO_BROKER));
	logger(escribir_loguear,l_debug,"El puerto del broker es: %s",puerto_broker);
}

void obtener_el_puerto_de_team(){
	puerto_team = strdup(config_get_string_value(config, KEY_CONFIG_PUERTO_TEAM));
	logger(escribir_loguear,l_debug,"El puerto de team es: %s",puerto_team);
}

void obtener_el_puerto_de_gamecard(){
	puerto_gamecard = strdup(config_get_string_value(config, KEY_CONFIG_PUERTO_GAMECARD));
	logger(escribir_loguear,l_debug,"El puerto del gamecard es: %s",puerto_gamecard);
}


void configurar_signals(void){
	struct sigaction signal_struct;
	signal_struct.sa_handler = capturar_signal;
	signal_struct.sa_flags = 0;

	sigemptyset(&signal_struct.sa_mask);

	sigaddset(&signal_struct.sa_mask, SIGPIPE);
	if (sigaction(SIGPIPE, &signal_struct, NULL) < 0) {
		logger(escribir_loguear, l_error, " SIGACTION error ");
	}

	sigaddset(&signal_struct.sa_mask, SIGINT);
	if (sigaction(SIGINT, &signal_struct, NULL) < 0) {
		logger(escribir_loguear, l_error, " SIGACTION error ");
	}
	sigaddset(&signal_struct.sa_mask, SIGSEGV);
	if (sigaction(SIGINT, &signal_struct, NULL) < 0) {
		logger(escribir_loguear, l_error, " SIGACTION error ");
	}
}

void capturar_signal(int signo){

    if(signo == SIGINT){
    	logger(escribir_loguear, l_warning,"\n Gameboy murioU");
    	terminar_gameboy_correctamente();
    }
    else if(signo == SIGPIPE){
    	logger(escribir_loguear, l_error,"Desconectado");
    }
    else if(signo == SIGSEGV){
		logger(escribir_loguear, l_error,"SEGMENTATION FAULT");
	}

}

void terminar_gameboy_correctamente(){
	logger(escribir_loguear,l_info,"Chau!");
	log_destroy(gameboy_logger);
	exit(EXIT_SUCCESS);
}
