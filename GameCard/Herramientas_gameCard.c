#include "Herramientas_gameCard.h"
#include<commons/log.h>

void inicializar_logger(){
	//creando el log , modo o level trace o debug
	gameCard_logger = log_create("gamecard.log", "gameCard", 1, LOG_LEVEL_TRACE);
	log_info(gameCard_logger, "Bienvenido al proceso Game Card");
	//logger(escribir_loguear,l_info,"Hi, bienvenido a GameCard!");
}

void inicializar_archivo_de_configuracion(){
	config_game_card = config_create("gamecard.config");
	if(config_game_card == NULL){
		//logger(escribir_loguear,l_error,"El archivo de configuracion no existe. Fijate en la carpeta Debug.");
		terminar_game_card();
	}else{
		//logger(escribir_loguear,l_info,"Cargando el archivo de configuracion...");
		log_info(gameCard_logger, "cargando los datos del archivo de configuración...");

		obtener_valor_config(KEY_IP_BROKER, config_game_card, obtener_ip_broker);
		obtener_valor_config(KEY_PUERTO_BROKER, config_game_card,obtener_puerto_broker);
		obtener_valor_config(KEY_PUNTO_MONTAJE_TALLGRASS, config_game_card, obtener_punto_montaje_tallgrass);
		obtener_valor_config(KEY_TIEMPO_DE_REINTENTO_CONEXION,config_game_card,obtener_tiempo_reintento_conexion);
		obtener_valor_config(KEY_TIEMPO_DE_REINTENTO_OPERACION , config_game_card, obtener_tiempo_reintento_operacion);
		obtener_valor_config(KEY_TIEMPO_RETARDO_OPERACION,config_game_card,obtener_tiempo_retardo_operacion);
		//logger(escribir_loguear,l_info,"\nArchivo de configuracion cargado correctamente");
		config_destroy(config_game_card);

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
	tiempo_reintento_conexion= config_get_int_value(config_game_card,KEY_TIEMPO_DE_REINTENTO_CONEXION);
	log_info(gameCard_logger,"se cargó el tiempo de reintento de conexion" );
	// logger(escribir_loguear,l_debug,"El tiempo de reconexion es %d segundos",tiempo_reintento_conexion);
};

//TIEMPO DE ESPERA POR UN ARCHIVO QUE YA ESTA ABIERTO Y YO LO NECESITO
void obtener_tiempo_reintento_operacion(){
	tiempo_reintento_operacion=config_get_int_value(config_game_card,KEY_TIEMPO_DE_REINTENTO_OPERACION);
	log_info(gameCard_logger," se cargó el tiempo de reintento de operacion");
	//logger(escribir_loguear,l_debug,"El tiempo de reconexion es %d segundos",tiempo_reintento_conexion);
}


void obtener_punto_montaje_tallgrass(){
	punto_montaje_fs = strdup(config_get_string_value(config_game_card, KEY_PUNTO_MONTAJE_TALLGRASS));
	log_info(gameCard_logger," se cargo el punto de montaje de game Card");
	//logger(escribir_loguear,l_debug,"El punto de montaje del FS es: %s",punto_montaje_fs);
}

void obtener_ip_broker(){
	ip_broker = strdup(config_get_string_value(config_game_card, KEY_IP_BROKER));
	//logger(escribir_loguear,l_debug,"La ip del broker es: %s",ip_broker);
	log_info(gameCard_logger,"se cargó el ip del broker");
}

void obtener_tiempo_retardo_operacion(){
	tiempo_retardo_operacion = config_get_int_value(config_game_card,KEY_TIEMPO_RETARDO_OPERACION);;
	//logger(escribir_loguear,l_debug,"El puerto del broker es: %s",puerto_broker);
	log_info(gameCard_logger,"se cargó el tiempo de ratardo de operacion");
}


void obtener_puerto_broker(){
	puerto_broker = strdup(config_get_string_value(config_game_card, KEY_PUERTO_BROKER));
	//logger(escribir_loguear,l_debug,"El puerto del broker es: %s",puerto_broker);
	log_info(gameCard_logger," se cargó el puerto del broker");
}
//TIEMPO_RETARDO_OPERACION​ =5

void configurar_signals_gc(void){
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
    	logger(escribir_loguear, l_warning,"\n GAMECARD DEJA DE FUNCIONAR, CHAU");
    	terminar_game_card();

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

void terminar_game_card(){
	logger(escribir_loguear,l_info,"Chau!");
	log_destroy(gameCard_logger);
	exit(EXIT_SUCCESS);
}

