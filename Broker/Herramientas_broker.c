#include "includes.h"

//Logger inicializado
void inicializar_logger(){
	broker_logger = log_create("broker.log", "Broker", 1, LOG_LEVEL_DEBUG);
	logger(escribir_loguear,l_info,"Hi, bienvenido a Broker ");
}

//Archivo de configuracion
void inicializar_archivo_de_configuracion(){
	config = config_create("broker.config");
	if(config == NULL){
		logger(escribir_loguear,l_error,"El archivo de configuracion no existe. Fijate en la carpeta Debug.");
		terminar_broker_correctamente();
	}else{
		logger(escribir_loguear,l_info,"Cargando el archivo de configuracion...");
		obtener_valor_config(KEY_CONFIG_TAMANIO_MEMORIA, config, obtener_tamanio_memoria);
		obtener_valor_config(KEY_CONFIG_TAMANIO_MINIMO_PARTICION, config, obtener_tamanio_minimo_particion);
		obtener_valor_config(KEY_CONFIG_ALGORITMO_MEMORIA, config, obtener_algoritmo_memoria);
		obtener_valor_config(KEY_CONFIG_ALGORITMO_REEMPLAZO, config, obtener_algoritmo_reemplazo);
		obtener_valor_config(KEY_CONFIG_ALGORITMO_PARTICION_LIBRE, config, obtener_algoritmo_particion_libre);
		obtener_valor_config(KEY_CONFIG_IP_BROKER, config, obtener_la_ip_del_broker);
		obtener_valor_config(KEY_CONFIG_PUERTO_BROKER, config, obtener_el_puerto_del_broker);
		obtener_valor_config(KEY_CONFIG_FRECUENCIA_COMPACTACION, config, obtener_frecuencia_compactacion);
		obtener_valor_config(KEY_CONFIG_LOG_FILE, config, obtener_el_log_file);


		logger(escribir_loguear,l_info,"\nArchivo de configuracion cargado correctamente :)");
		config_destroy(config);

	}

}

void obtener_valor_config(char* key, t_config* file, void(*obtener)(void)){
	if (config_has_property(file, key)){
		obtener();
	}
}

void obtener_tamanio_memoria(){
	tamanio_memoria = config_get_array_value(config, KEY_CONFIG_TAMANIO_MEMORIA);
	logger(escribir_loguear,l_debug,"El tamaño inicial de memoria: %d",tamanio_memoria);
}

void obtener_tamanio_minimo_particion(){
	tamanio_minimo_particion = config_get_array_value(config, KEY_CONFIG_TAMANIO_MINIMO_PARTICION);
	logger(escribir_loguear,l_debug,"El tamaño minimo de particion: %d",tamanio_minimo_particion);
}

void obtener_algoritmo_memoria(){
	algoritmo_memoria = config_get_array_value(config, KEY_CONFIG_ALGORITMO_MEMORIA);
	logger(escribir_loguear,l_debug,"El algoritmo de memoria es: %s",algoritmo_memoria);
}

void obtener_algoritmo_reemplazo(){
	algoritmo_reemplazo = config_get_int_value(config, KEY_CONFIG_ALGORITMO_REEMPLAZO);
	logger(escribir_loguear,l_debug,"El algoritmo de reemplazo es: %s",algoritmo_reemplazo);
}

void obtener_algoritmo_particion_libre(){
    algoritmo_particion_libre = config_get_int_value(config, KEY_CONFIG_ALGORITMO_PARTICION_LIBRE);
	logger(escribir_loguear,l_debug,"El algoritmo para particiones libre es: %s",algoritmo_particion_libre);
}

void obtener_la_ip_del_broker(){
	ip_broker = strdup(config_get_string_value(config, KEY_CONFIG_IP_BROKER));
	logger(escribir_loguear,l_debug,"La ip del broker es: %s",ip_broker);
}

void obtener_el_puerto_del_broker(){
	puerto_broker = strdup(config_get_string_value(config, KEY_CONFIG_PUERTO_BROKER));
	logger(escribir_loguear,l_debug,"El puerto del broker es: %d",puerto_broker);
}

void obtener_frecuencia_compactacion(){
	frecuencia_compactacion = strdup(config_get_string_value(config, KEY_CONFIG_PUERTO_BROKER));
	logger(escribir_loguear,l_debug,"La frecuencia de compactacion es: %d",frecuencia_compactacion);
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
    	terminar_broker_correctamente();

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

void terminar_broker_correctamente(){
	logger(escribir_loguear,l_info,"Chau!");
	log_destroy(broker_logger);
	exit(EXIT_SUCCESS);
}

