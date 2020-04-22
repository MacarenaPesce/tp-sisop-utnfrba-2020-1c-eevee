#include "Herramientas_gameCard.h"

void inicializar_logger(){
	//creando el log
	gameCard_logger = log_create("gameCard.log", "gameCard", 1, LOG_LEVEL_DEBUG);
	logger(escribir_loguear,l_info,"Hi, bienvenido a GameCard!");
}

void inicializar_archivo_de_configuracion(){
	config = config_create("gamecard.config");
	if(config == NULL){
		logger(escribir_loguear,l_error,"El archivo de configuracion no existe. Fijate en la carpeta Debug.");
		terminar_team_correctamente();
	}else{
		logger(escribir_loguear,l_info,"Cargando el archivo de configuracion...");

		obtener_valor_config();


		/*
		 * KEY_TIEMPO_DE_REINTENTO_CONEXION "TIEMPO_DE_REINTENTO_CONEXION"
#define KEY_TIEMPO_DE_REINTENTO_OPERACION "TIEMPO_DE_REINTENTO_OPERACION"
#define KEY_PUNTO_MONTAJE_TALLGRASS "PUNTO_MONTAJE_TALLGRASS"
#define KEY_IP_BROKER "IP_BROKER"
#define KEY_PUERTO_BROKER "PUERTO BROKER"
		 *
		 */
		//verifica si la clave esta en el arch de configuracion
		//y si es verdad aplica la funcion pasada en ele tercer parametro
		obtener_valor_config(KEY_TIEMPO_DE_REINTENTO_CONEXION , config, obtener_tiempo_reintento_conexion);
		obtener_valor_config(KEY_TIEMPO_DE_REINTENTO_OPERACION , config, obtener_tiempo_reintento_operacion);
	/*obtener_valor_config(KEY_CONFIG_OBJETIVOS_ENTRENADORES, config, obtener_los_objetivos_de_entrenadores);
		obtener_valor_config(KEY_CONFIG_TIEMPO_RECONEXION, config, obtener_el_tiempo_de_reconexion);
		obtener_valor_config(KEY_CONFIG_RETARDO_CICLO_CPU, config, obtener_el_retardo_de_ciclo_de_cpu);
		obtener_valor_config(KEY_CONFIG_ALGORITMO_PLANIFICACION, config, obtener_el_algoritmo_de_planificacion);
		obtener_valor_config(KEY_CONFIG_QUANTUM, config, obtener_el_quantum);
		obtener_valor_config(KEY_CONFIG_ESTIMACION_INICIAL, config, obtener_la_estimacion_inicial);
		obtener_valor_config(KEY_CONFIG_IP_BROKER, config, obtener_la_ip_del_broker);
		obtener_valor_config(KEY_CONFIG_PUERTO_BROKER, config, obtener_el_puerto_del_broker);
		obtener_valor_config(KEY_CONFIG_LOG_FILE, config, obtener_el_log_file);
*/
		logger(escribir_loguear,l_info,"\nArchivo de configuracion cargado correctamente :)");
		config_destroy(config);


	}

}

// a partir de la clave del archivo de configuracion
void obtener_valor_config(char* key, t_config* file, void(*obtener)(void)){
	//si la clave se encuentra en el archivo de configuracion
	if (config_has_property(file, key)){
		//aplicar la funcion que se pasa por el tercer parametro
		obtener();
	}
}

//EXISTE POR SISTEMA DE RECONEXION POR CAIDA DEL BROKER
void obtener_tiempo_reintento_conexion(){
	tiempo_reconexion = config_get_int_value(config,KEY_TIEMPO_DE_REINTENTO_CONEXION);
		logger(escribir_loguear,l_debug,"El tiempo de reconexion es %d segundos",tiempo_reconexion);
};

//TIEMPO DE ESPERA POR UN ARCHIVO QUE YA ESTA ABIERTO Y YO LO NECESITO
void obtener_tiempo_reintento_operacion(){
	//tiempo_espera_operacion=config_get_int_value(config,)
}
/*
void obtener_las_posiciones_de_entrenadores(){
	posiciones_entrenadores = config_get_array_value(config, KEY_CONFIG_POSICIONES_ENTRENADORES);
	logger(escribir_loguear,l_debug,"Las posiciones de entrenadores recuperadas");
}

void obtener_los_pokemon_de_entrenadores(){
	pokemon_entrenadores = config_get_array_value(config, KEY_CONFIG_POKEMON_ENTRENADORES);
	logger(escribir_loguear,l_debug,"Los pokemon de los entrenadores recuperados");
}

void obtener_los_objetivos_de_entrenadores(){
	objetivos_entrenadores = config_get_array_value(config, KEY_CONFIG_OBJETIVOS_ENTRENADORES);
	logger(escribir_loguear,l_debug,"Los objetivos de los entrenadores recuperados");
}

void obtener_el_tiempo_de_reconexion(){
	tiempo_reconexion = config_get_int_value(config, KEY_CONFIG_TIEMPO_RECONEXION);
	logger(escribir_loguear,l_debug,"El tiempo de reconexion es: %d",tiempo_reconexion);
}

void obtener_el_retardo_de_ciclo_de_cpu(){
    retardo_ciclo_cpu = config_get_int_value(config, KEY_CONFIG_RETARDO_CICLO_CPU);
	logger(escribir_loguear,l_debug,"El retardo de ciclo de cpu es: %d",retardo_ciclo_cpu);
}

void obtener_el_algoritmo_de_planificacion(){
	algoritmo_planificacion = strdup(config_get_string_value(config, KEY_CONFIG_ALGORITMO_PLANIFICACION));
	logger(escribir_loguear,l_debug,"El algoritmo de planificacion es: %s",algoritmo_planificacion);
}

void obtener_el_quantum(){
	quantum = config_get_int_value(config, KEY_CONFIG_QUANTUM);
	logger(escribir_loguear,l_debug,"El quantum es: %d",quantum);
}

void obtener_la_estimacion_inicial(){
	estimacion_inicial = config_get_int_value(config, KEY_CONFIG_ESTIMACION_INICIAL);
	logger(escribir_loguear,l_debug,"La estimacion_inicial es: %d",estimacion_inicial);
}

void obtener_la_ip_del_broker(){
	ip_broker = strdup(config_get_string_value(config, KEY_CONFIG_IP_BROKER));
	logger(escribir_loguear,l_debug,"La ip del broker es: %s",ip_broker);
}

void obtener_el_puerto_del_broker(){
	puerto_broker = strdup(config_get_string_value(config, KEY_CONFIG_PUERTO_BROKER));
	logger(escribir_loguear,l_debug,"El puerto del broker es: %s",puerto_broker);
}

void obtener_el_log_file(){
	log_file = strdup(config_get_string_value(config, KEY_CONFIG_LOG_FILE));
	logger(escribir_loguear,l_debug,"El log file es: %s",log_file);
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

    if(signo == SIGINT)
    {
    	logger(escribir_loguear, l_warning,"\n TEAM DEJA DE FUNCIONAR, CHAU");
    	terminar_team_correctamente();

    }
    else if(signo == SIGPIPE)
    {
    	logger(escribir_loguear, l_error,"Desconectado");
    }
    else if(signo == SIGSEGV)
	{
		logger(escribir_loguear, l_error,"SEGMENTATION FAULT");
	}

}

void terminar_team_correctamente(){
	logger(escribir_loguear,l_info,"Chau!");
	log_destroy(team_logger);
	exit(EXIT_SUCCESS);
}

*/
