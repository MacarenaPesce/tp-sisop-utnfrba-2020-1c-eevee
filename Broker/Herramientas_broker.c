#include "Herramientas_broker.h"

extern int tamanio_memoria;
extern int tamanio_minimo_particion;
extern char* algoritmo_memoria;
extern char* algoritmo_reemplazo;
extern char* algoritmo_particion_libre;
extern char* ip_broker;
extern int frecuencia_compactacion;
extern char* puerto_broker;
extern char* log_file;

extern t_config* config;
extern t_log* broker_logger;

extern enum SERVER_STATUS server_status; 


//Logger inicializado
void inicializar_logger(){
	broker_logger = log_create("../logs/broker.log", "Broker", 0, LOG_LEVEL_DEBUG);
	broker_debug_logger = log_create("../logs/broker_debug.log", "Broker", 0, LOG_LEVEL_DEBUG);
	log_info(broker_debug_logger, "*************** INICIANDO EJECUCION DE BROKER ***************", NULL);
}

//Archivo de configuracion
void inicializar_archivo_de_configuracion(){
	config = config_create("../Broker.config");
	if(config == NULL){
		log_info(broker_debug_logger,"El archivo de configuracion no existe. Fijate en la carpeta Debug.");
		terminar_broker_correctamente();
	}else{

		log_info(broker_debug_logger,"Cargando el archivo de configuracion...");
		obtener_valor_config(KEY_CONFIG_TAMANIO_MEMORIA, config, obtener_tamanio_memoria);
		obtener_valor_config(KEY_CONFIG_TAMANIO_MINIMO_PARTICION, config, obtener_tamanio_minimo_particion);
		obtener_valor_config(KEY_CONFIG_ALGORITMO_MEMORIA, config, obtener_algoritmo_memoria);
		obtener_valor_config(KEY_CONFIG_ALGORITMO_REEMPLAZO, config, obtener_algoritmo_reemplazo);
		obtener_valor_config(KEY_CONFIG_ALGORITMO_PARTICION_LIBRE, config, obtener_algoritmo_particion_libre);
		obtener_valor_config(KEY_CONFIG_IP_BROKER, config, obtener_la_ip_del_broker);
		obtener_valor_config(KEY_CONFIG_PUERTO_BROKER, config, obtener_el_puerto_del_broker);
		obtener_valor_config(KEY_CONFIG_FRECUENCIA_COMPACTACION, config, obtener_frecuencia_compactacion);
		obtener_valor_config(KEY_CONFIG_LOG_FILE, config, obtener_el_log_file);

		log_info(broker_debug_logger, "Archivo de configuracion cargado...");
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
	if(debug_broker) log_debug(broker_debug_logger,"El tamaño inicial de memoria: %d",tamanio_memoria);
}

void obtener_tamanio_minimo_particion(){
	tamanio_minimo_particion = config_get_int_value(config, KEY_CONFIG_TAMANIO_MINIMO_PARTICION);
	if(debug_broker) log_debug(broker_debug_logger,"El tamaño minimo de particion: %d",tamanio_minimo_particion);
}

void obtener_algoritmo_memoria(){
	algoritmo_memoria = strdup(config_get_string_value(config, KEY_CONFIG_ALGORITMO_MEMORIA));
	if(debug_broker) log_debug(broker_debug_logger,"El algoritmo de memoria es: %s",algoritmo_memoria);
}

void obtener_algoritmo_reemplazo(){
	algoritmo_reemplazo =strdup( config_get_string_value(config, KEY_CONFIG_ALGORITMO_REEMPLAZO));
	if(debug_broker) log_debug(broker_debug_logger,"El algoritmo de reemplazo es: %s",algoritmo_reemplazo);
}

void obtener_algoritmo_particion_libre(){
    algoritmo_particion_libre = strdup(config_get_string_value(config, KEY_CONFIG_ALGORITMO_PARTICION_LIBRE));
	if(debug_broker) log_debug(broker_debug_logger,"El algoritmo para particiones libre es: %s",algoritmo_particion_libre);
}

void obtener_la_ip_del_broker(){
	ip_broker = strdup(config_get_string_value(config, KEY_CONFIG_IP_BROKER));
	if(debug_broker) log_debug(broker_debug_logger,"la ip del broker es: %s",ip_broker);
}

void obtener_el_puerto_del_broker(){
	puerto_broker = strdup(config_get_string_value(config, KEY_CONFIG_PUERTO_BROKER));
	if(debug_broker) log_debug(broker_debug_logger,"el puerto del broker es: %s",puerto_broker);
}

void obtener_frecuencia_compactacion(){
	frecuencia_compactacion = config_get_int_value(config, KEY_CONFIG_FRECUENCIA_COMPACTACION);
	if(debug_broker) log_debug(broker_debug_logger,"La frecuencia de compactacion es: %d",frecuencia_compactacion);
}

void obtener_el_log_file(){
	log_file = strdup(config_get_string_value(config, KEY_CONFIG_LOG_FILE));
	if(debug_broker) log_debug(broker_debug_logger,"El log file es: %s",log_file);	
}

void configurar_signals(void){
	struct sigaction signal_struct;
	signal_struct.sa_handler = capturar_signal;
	signal_struct.sa_flags = 0;

	sigemptyset(&signal_struct.sa_mask);

	sigaddset(&signal_struct.sa_mask, SIGPIPE);
	if (sigaction(SIGPIPE, &signal_struct, NULL) < 0) {
		log_error(broker_debug_logger, " SIGACTION error ");
	}

	sigaddset(&signal_struct.sa_mask, SIGINT);
	if (sigaction(SIGINT, &signal_struct, NULL) < 0) {
		log_error(broker_debug_logger, " SIGACTION error ");
	}
	sigaddset(&signal_struct.sa_mask, SIGSEGV);
	if (sigaction(SIGSEGV, &signal_struct, NULL) < 0) {
		log_error(broker_debug_logger, " SIGACTION error ");
	}
	sigaddset(&signal_struct.sa_mask, SIGUSR1);
	if (sigaction(SIGUSR1, &signal_struct, NULL) < 0) {
		dump_memoria();
		log_info(broker_debug_logger, " Dump de memoria ");
	}
}

void capturar_signal(int signo){

    if(signo == SIGINT)
    {
    	log_info(broker_debug_logger," Broker DEJA DE FUNCIONAR ");
    	terminar_broker_correctamente();

    }
    else if(signo == SIGPIPE)
    {
    	log_info(broker_debug_logger,"Desconectado");
    }
    else if(signo == SIGSEGV)
	{
	//	log_info(broker_debug_logger,"SEGMENTATION FAULT");
	}
	else if(signo == SIGUSR1){
		log_info(broker_debug_logger, "DUMP");
		pthread_mutex_lock(&mutex_queue_mensajes);
		dump_memoria();
		pthread_mutex_unlock(&mutex_queue_mensajes);
	}

}

void terminar_broker_correctamente(){

	pthread_mutex_lock(&mutex_server_status);

	server_status = ENDING;
	
	pthread_mutex_unlock(&mutex_server_status);

	sem_post(&transaccionar_paquetes_pendientes);

 	pthread_mutex_lock(&mutex_queue_mensajes);

	cerrar_senders();

	vaciar_sockets_de_clientes();

	vaciar_memoria();

	pthread_mutex_unlock(&mutex_queue_mensajes); 	

	sleep(2);
		
	log_info(broker_debug_logger,"Chau!");
	
	exit(EXIT_SUCCESS);
}

void vaciar_sockets_de_clientes(){

	void eliminar_sockets_cliente(void* _cliente){

		t_cliente* cliente = (t_cliente*) _cliente;

		list_destroy_and_destroy_elements(cliente->sockets,free);
	}

	list_iterate(cache_mensajes->clientes,eliminar_sockets_cliente);
}

void cerrar_senders(){

	void despertar_senders(void* _cola){

		t_cola_mensajes* cola = (t_cola_mensajes*) _cola;

		sem_post(cola->producciones);
	}

	list_iterate(cache_mensajes->colas,despertar_senders);

}

void vaciar_memoria(){

	void eliminar_bloque_memoria(void* _bloque){

		t_bloque_memoria* bloque = (t_bloque_memoria*) _bloque;

		if(bloque->esta_vacio == false) liberar_bloque_memoria(bloque);

		free(bloque);
	}

	list_iterate(cache_mensajes->memoria,eliminar_bloque_memoria);

	free(memoria_inicial);

}