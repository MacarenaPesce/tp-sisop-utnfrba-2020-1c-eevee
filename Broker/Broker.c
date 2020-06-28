#include "Broker.h"
extern int tamanio_memoria;


int main(){

	inicializar_logger();

	pthread_t hilo_sender[COLA_LOCALIZED_POKEMON];
	
	debug_broker = true;

	if(debug_broker) log_debug(broker_logger, "1) Cache de mensajes lista!!!", NULL);
	
	if(debug_broker) log_debug(broker_logger, "2) Inicializando archivo de configuracion...", NULL);

	inicializar_archivo_de_configuracion();

	if(debug_broker) log_debug(broker_logger, "3) Configuraciones cargadas correctamente!!!", NULL);

	if(debug_broker) log_debug(broker_logger, "4) Configurando signals...", NULL);

	configurar_signals();

	if(debug_broker) log_debug(broker_logger, "5) Signals configuradas correctamente!!!", NULL);

	if(debug_broker) log_debug(broker_logger, "6) Inicializando cache de mensajes...", NULL);

	pthread_mutex_lock(&mutex_queue_mensajes);

	//TODO: levantame de config

	cache_mensajes = (t_cache_colas*)malloc(sizeof(t_cache_colas));
	cache_mensajes->colas = list_create();
	cache_mensajes->proximo_id_mensaje = 0;
	cache_mensajes->clientes = list_create();

	t_cola_mensajes* aux_crear_cola_mensajes; 

	for(int i = COLA_APPEARED_POKEMON; i <= COLA_LOCALIZED_POKEMON; i++){

		aux_crear_cola_mensajes = crear_cola_mensajes(i);

		list_add(cache_mensajes->colas,aux_crear_cola_mensajes);

		pthread_create(&hilo_sender[i],NULL,sender_suscriptores,aux_crear_cola_mensajes);	

		pthread_detach(hilo_sender[i]);	
		
	}

	//if(debug_broker) log_debug(broker_logger,"El espacio de memoria principal es: %d",tamanio_memoria);

	asignar_memoria_inicial(2048);

	pthread_mutex_unlock(&mutex_queue_mensajes);	

	iniciar_servidor();

	//primer_bloque = AsignarMemoriaInicial(tamanio_memoria_inicial,lista_memoria);
	while(server_status != ENDING){
	
	}


}