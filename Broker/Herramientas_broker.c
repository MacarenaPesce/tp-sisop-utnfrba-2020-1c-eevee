#include "Herramientas_broker.h"

extern t_log* broker_logger;

extern enum SERVER_STATUS server_status; 

void inicializar_logger(){
	broker_logger = log_create("../Broker.log", "Broker", 1, LOG_LEVEL_DEBUG);
	log_info(broker_logger, "*************** INICIANDO EJECUCION DE BROKER ***************", NULL);
}

void inicializar_archivo_de_configuracion(){
	config = config_create("../Broker.config");
	if(config == NULL){
		logger(escribir_loguear,l_error,"El archivo de configuracion no existe. Fijate en la carpeta Debug.");
		terminar_broker_correctamente();
	}else{
		obtener_valor_config(KEY_CONFIG_IP_BROKER, config, obtener_la_ip_del_broker);
		obtener_valor_config(KEY_CONFIG_PUERTO_BROKER, config, obtener_el_puerto_del_broker);
		obtener_valor_config(KEY_CONFIG_LOG_FILE, config, obtener_el_log_file);

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
}

void obtener_el_puerto_del_broker(){
	puerto_broker = strdup(config_get_string_value(config, KEY_CONFIG_PUERTO_BROKER));
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
		
		server_status = ENDING;

    	printf("\n Broker DEJA DE FUNCIONAR \n\n");
    	terminar_broker_correctamente();

    }
    else if(signo == SIGPIPE)
    {
    	printf("Desconectado");
    }
    else if(signo == SIGSEGV)
	{
		printf("SEGMENTATION FAULT");
	}

}

void terminar_broker_correctamente(){
	printf("Chau!\n\n");
	exit(EXIT_SUCCESS);
}
