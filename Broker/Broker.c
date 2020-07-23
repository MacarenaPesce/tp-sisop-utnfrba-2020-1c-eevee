#include "Broker.h"
extern int tamanio_memoria;

/*
	Resultados de las pruebas: 
		1- Consolidacion -> con FIFO -> OK -> 40 bytes de leaks
		2- Consolidacion -> con LRU  ->	OK -> 48 bytes
*/

int main(){

	pthread_mutex_lock(&mutex_server_status);
	server_status = STARTING;
	pthread_mutex_unlock(&mutex_server_status);

	//inicializo los logs
	inicializar_logger();
	//activo los logs de debug
	debug_broker = false;
	warn_broker = true;

	if(debug_broker) log_debug(broker_logger, "1) Cache de mensajes lista!!!");
	
	if(debug_broker) log_debug(broker_logger, "2) Inicializando archivo de configuracion...");

	inicializar_archivo_de_configuracion();

	if(debug_broker) log_debug(broker_logger, "3) Configuraciones cargadas correctamente!!!");

	if(debug_broker) log_debug(broker_logger, "4) Configurando signals...");

	configurar_signals();

	if(debug_broker) log_debug(broker_logger, "5) Signals configuradas correctamente!!!");

	if(debug_broker) log_debug(broker_logger, "6) Inicializando cache de mensajes...");

	pthread_mutex_lock(&mutex_recepcion_mensajes);
	
	paquetes_pendientes = list_create();

	sem_init(&transaccionar_paquetes_pendientes, 0, 0); 

	pthread_mutex_unlock(&mutex_recepcion_mensajes);

	pthread_mutex_lock(&mutex_queue_mensajes);


	//TODO: levantame de config

	cache_mensajes = (t_cache_colas*)malloc(sizeof(t_cache_colas));
	cache_mensajes->colas = list_create();
	cache_mensajes->proximo_id_mensaje = 0;
	cache_mensajes->clientes = list_create();
	t_cola_mensajes* aux_crear_cola_mensajes; 

	pthread_create(&hilo_transacciones,NULL,transaccionar_paquete_recibido,NULL);

	pthread_detach(hilo_transacciones);

	for(int i = COLA_APPEARED_POKEMON; i <= COLA_LOCALIZED_POKEMON; i++){

		aux_crear_cola_mensajes = crear_cola_mensajes(i);

		list_add(cache_mensajes->colas,aux_crear_cola_mensajes);

		pthread_create(&hilo_sender[i],NULL,sender_suscriptores,aux_crear_cola_mensajes);	

		pthread_detach(hilo_sender[i]);	
	}

	//asigno memoria inicial seteado en archivo config
	asignar_memoria_inicial(tamanio_memoria);

	pthread_mutex_unlock(&mutex_queue_mensajes);	

	iniciar_servidor();

}