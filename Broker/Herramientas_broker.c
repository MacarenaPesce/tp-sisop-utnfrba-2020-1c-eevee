#include "Herramientas_broker.h"


extern t_log* broker_logger;

extern enum SERVER_STATUS server_status; 


//Logger inicializado
void inicializar_logger(){
	broker_logger = log_create("../Broker.log", "Broker", 1, LOG_LEVEL_DEBUG);
	log_info(broker_logger, "*************** INICIANDO EJECUCION DE BROKER ***************", NULL);
}

//Archivo de configuracion
void inicializar_archivo_de_configuracion(){
	config = config_create("../Broker.config");
	if(config == NULL){
		log_info(broker_logger,"El archivo de configuracion no existe. Fijate en la carpeta Debug.");
		terminar_broker_correctamente();
	}else{

		log_info(broker_logger,"Cargando el archivo de configuracion...");
		obtener_valor_config(KEY_CONFIG_TAMANIO_MEMORIA, config, obtener_tamanio_memoria);
		obtener_valor_config(KEY_CONFIG_TAMANIO_MINIMO_PARTICION, config, obtener_tamanio_minimo_particion);
		obtener_valor_config(KEY_CONFIG_ALGORITMO_MEMORIA, config, obtener_algoritmo_memoria);
		obtener_valor_config(KEY_CONFIG_ALGORITMO_REEMPLAZO, config, obtener_algoritmo_reemplazo);
		obtener_valor_config(KEY_CONFIG_ALGORITMO_PARTICION_LIBRE, config, obtener_algoritmo_particion_libre);
		obtener_valor_config(KEY_CONFIG_IP_BROKER, config, obtener_la_ip_del_broker);
		obtener_valor_config(KEY_CONFIG_PUERTO_BROKER, config, obtener_el_puerto_del_broker);
		obtener_valor_config(KEY_CONFIG_FRECUENCIA_COMPACTACION, config, obtener_frecuencia_compactacion);
		obtener_valor_config(KEY_CONFIG_LOG_FILE, config, obtener_el_log_file);

		config_destroy(config);

	}

}

void obtener_valor_config(char* key, t_config* file, void(*obtener)(void)){
	if (config_has_property(file, key)){
		obtener();
	}
}

void obtener_tamanio_memoria(){
	tamanio_memoria = config_get_int_value(config, KEY_CONFIG_TAMANIO_MEMORIA);
	if(debug_broker) log_debug(broker_logger,"El tamaño inicial de memoria: %d",tamanio_memoria);
}

void obtener_tamanio_minimo_particion(){
	tamanio_minimo_particion = config_get_int_value(config, KEY_CONFIG_TAMANIO_MINIMO_PARTICION);
	if(debug_broker) log_debug(broker_logger,"El tamaño minimo de particion: %d",tamanio_minimo_particion);
}

void obtener_algoritmo_memoria(){
	algoritmo_memoria = strdup(config_get_string_value(config, KEY_CONFIG_ALGORITMO_MEMORIA));
	if(debug_broker) log_debug(broker_logger,"El algoritmo de memoria es: %s",algoritmo_memoria);
}

void obtener_algoritmo_reemplazo(){
	algoritmo_reemplazo =strdup( config_get_string_value(config, KEY_CONFIG_ALGORITMO_REEMPLAZO));
	if(debug_broker) log_debug(broker_logger,"El algoritmo de reemplazo es: %s",algoritmo_reemplazo);
}

void obtener_algoritmo_particion_libre(){
    algoritmo_particion_libre = strdup(config_get_string_value(config, KEY_CONFIG_ALGORITMO_PARTICION_LIBRE));
	if(debug_broker) log_debug(broker_logger,"El algoritmo para particiones libre es: %s",algoritmo_particion_libre);
}

void obtener_la_ip_del_broker(){
	ip_broker = strdup(config_get_string_value(config, KEY_CONFIG_IP_BROKER));
}

void obtener_el_puerto_del_broker(){
	puerto_broker = strdup(config_get_string_value(config, KEY_CONFIG_PUERTO_BROKER));
}

void obtener_frecuencia_compactacion(){
	frecuencia_compactacion = config_get_int_value(config, KEY_CONFIG_PUERTO_BROKER);
	if(debug_broker) log_debug(broker_logger,"La frecuencia de compactacion es: %d",frecuencia_compactacion);
}

void obtener_el_log_file(){
	log_file = strdup(config_get_string_value(config, KEY_CONFIG_LOG_FILE));
}

void configurar_signals(void){
	struct sigaction signal_struct;
	signal_struct.sa_handler = capturar_signal;
	signal_struct.sa_flags = 0;

	sigemptyset(&signal_struct.sa_mask);

	sigaddset(&signal_struct.sa_mask, SIGPIPE);
	if (sigaction(SIGPIPE, &signal_struct, NULL) < 0) {
		log_error(broker_logger, " SIGACTION error ");
	}

	sigaddset(&signal_struct.sa_mask, SIGINT);
	if (sigaction(SIGINT, &signal_struct, NULL) < 0) {
		log_error(broker_logger, " SIGACTION error ");
	}
	sigaddset(&signal_struct.sa_mask, SIGSEGV);
	if (sigaction(SIGINT, &signal_struct, NULL) < 0) {
		log_error(broker_logger, " SIGACTION error ");
	}
}

void capturar_signal(int signo){

    if(signo == SIGINT)
    {
		
		server_status = ENDING;

    	log_info(broker_logger," Broker DEJA DE FUNCIONAR ");
    	terminar_broker_correctamente();

    }
    else if(signo == SIGPIPE)
    {
    	log_info(broker_logger,"Desconectado");
    }
    else if(signo == SIGSEGV)
	{
		log_info(broker_logger,"SEGMENTATION FAULT");
	}

}

void terminar_broker_correctamente(){
	log_info(broker_logger,"Chau!");
	exit(EXIT_SUCCESS);
}

