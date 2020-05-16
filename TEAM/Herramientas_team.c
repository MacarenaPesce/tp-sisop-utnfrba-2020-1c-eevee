/*
 * Herramientas_team.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Herramientas_team.h"

void inicializar_logger(){
	team_logger = log_create("team.log", "Team", 1, LOG_LEVEL_DEBUG);
	log_info(team_logger,"Hi, bienvenido a Team");

	team_logger_oficial = log_create("/home/utnso/log_team1", "Team", 0, LOG_LEVEL_DEBUG);
	log_info(team_logger_oficial,"Bienvenido a Team, este es el archivo de log oficial");
}

void inicializar_archivo_de_configuracion(){
	config = config_create("team.config");
	if(config == NULL){
		log_info(team_logger,"El archivo de configuracion no existe. Fijate en la carpeta Debug.");
		terminar_team_correctamente();
	}else{
		log_info(team_logger,"Cargando el archivo de configuracion...");
		obtener_valor_config(KEY_CONFIG_POSICIONES_ENTRENADORES, config, obtener_las_posiciones_de_entrenadores);
		obtener_valor_config(KEY_CONFIG_POKEMON_ENTRENADORES, config, obtener_los_pokemon_de_entrenadores);
		obtener_valor_config(KEY_CONFIG_OBJETIVOS_ENTRENADORES, config, obtener_los_objetivos_de_entrenadores);
		obtener_valor_config(KEY_CONFIG_TIEMPO_RECONEXION, config, obtener_el_tiempo_de_reconexion);
		obtener_valor_config(KEY_CONFIG_RETARDO_CICLO_CPU, config, obtener_el_retardo_de_ciclo_de_cpu);
		obtener_valor_config(KEY_CONFIG_ALGORITMO_PLANIFICACION, config, obtener_el_algoritmo_de_planificacion);
		obtener_valor_config(KEY_CONFIG_QUANTUM, config, obtener_el_quantum);
		obtener_valor_config(KEY_CONFIG_ESTIMACION_INICIAL, config, obtener_la_estimacion_inicial);
		obtener_valor_config(KEY_CONFIG_IP_BROKER, config, obtener_la_ip_del_broker);
		obtener_valor_config(KEY_CONFIG_PUERTO_BROKER, config, obtener_el_puerto_del_broker);
		obtener_valor_config(KEY_CONFIG_LOG_FILE, config, obtener_el_log_file);


		log_info(team_logger,"\nArchivo de configuracion cargado correctamente :)");
		config_destroy(config);

	}

}

void obtener_valor_config(char* key, t_config* file, void(*obtener)(void)){
	if (config_has_property(file, key)){
		obtener();
	}
}

void obtener_las_posiciones_de_entrenadores(){
	posiciones_entrenadores = config_get_array_value(config, KEY_CONFIG_POSICIONES_ENTRENADORES);
	log_info(team_logger,"Las posiciones de entrenadores recuperadas");
}

void obtener_los_pokemon_de_entrenadores(){
	pokemon_entrenadores = config_get_array_value(config, KEY_CONFIG_POKEMON_ENTRENADORES);
	log_info(team_logger,"Los pokemon de los entrenadores recuperados");
}

void obtener_los_objetivos_de_entrenadores(){
	objetivos_entrenadores = config_get_array_value(config, KEY_CONFIG_OBJETIVOS_ENTRENADORES);
	log_info(team_logger,"Los objetivos de los entrenadores recuperados");
}

void obtener_el_tiempo_de_reconexion(){
	tiempo_reconexion = config_get_int_value(config, KEY_CONFIG_TIEMPO_RECONEXION);
	log_info(team_logger,"El tiempo de reconexion es: %d",tiempo_reconexion);
}

void obtener_el_retardo_de_ciclo_de_cpu(){
    retardo_ciclo_cpu = config_get_int_value(config, KEY_CONFIG_RETARDO_CICLO_CPU);
	log_info(team_logger,"El retardo de ciclo de cpu es: %d",retardo_ciclo_cpu);
}

void obtener_el_algoritmo_de_planificacion(){
	algoritmo_planificacion = strdup(config_get_string_value(config, KEY_CONFIG_ALGORITMO_PLANIFICACION));
	log_info(team_logger,"El algoritmo de planificacion es: %s",algoritmo_planificacion);
}

void obtener_el_quantum(){
	quantum = config_get_int_value(config, KEY_CONFIG_QUANTUM);
	log_info(team_logger,"El quantum es: %d",quantum);
}

void obtener_la_estimacion_inicial(){
	estimacion_inicial = config_get_int_value(config, KEY_CONFIG_ESTIMACION_INICIAL);
	log_info(team_logger,"La estimacion_inicial es: %d",estimacion_inicial);
}

void obtener_la_ip_del_broker(){
	ip_broker = strdup(config_get_string_value(config, KEY_CONFIG_IP_BROKER));
	log_info(team_logger,"La ip del broker es: %s",ip_broker);
}

void obtener_el_puerto_del_broker(){
	puerto_broker = strdup(config_get_string_value(config, KEY_CONFIG_PUERTO_BROKER));
	log_info(team_logger,"El puerto del broker es: %s",puerto_broker);
}

void obtener_el_log_file(){
	log_file = strdup(config_get_string_value(config, KEY_CONFIG_LOG_FILE));
	log_info(team_logger,"El log file es: %s",log_file);
}

void configurar_signals(void){
	struct sigaction signal_struct;
	signal_struct.sa_handler = capturar_signal;
	signal_struct.sa_flags = 0;

	sigemptyset(&signal_struct.sa_mask);

	sigaddset(&signal_struct.sa_mask, SIGPIPE);
	if (sigaction(SIGPIPE, &signal_struct, NULL) < 0) {
		log_info(team_logger, " SIGACTION error ");
	}

	sigaddset(&signal_struct.sa_mask, SIGINT);
	if (sigaction(SIGINT, &signal_struct, NULL) < 0) {
		log_info(team_logger, " SIGACTION error ");
	}
	sigaddset(&signal_struct.sa_mask, SIGSEGV);
	if (sigaction(SIGINT, &signal_struct, NULL) < 0) {
		log_info(team_logger, " SIGACTION error ");
	}
}

void capturar_signal(int signo){

    if(signo == SIGINT)
    {
    	log_info(team_logger,"\n TEAM DEJA DE FUNCIONAR");
    	terminar_team_correctamente();

    }
    else if(signo == SIGPIPE)
    {
    	log_info(team_logger,"Desconectado");
    }
    else if(signo == SIGSEGV)
	{
		log_info(team_logger,"SEGMENTATION FAULT");
	}

}

void terminar_team_correctamente(){
	log_info(team_logger,"Chau!");
	log_destroy(team_logger);
	exit(EXIT_SUCCESS);
}
