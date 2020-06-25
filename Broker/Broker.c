#include "Broker.h"


int main(){

	inicializar_logger();

	pthread_t hilo_sender[COLA_LOCALIZED_POKEMON];
	
	debug_broker = true;

	asignar_memoria_inicial(8000);

	if(debug_broker) log_debug(broker_logger, "1) Inicializando cache de mensajes...", NULL);

	//printf("\n1) Inicializando cache de mensajes... \n");

	pthread_mutex_lock(&mutex_queue_mensajes);

	cache_mensajes = (t_cache_colas*)malloc(sizeof(t_cache_colas));
	cache_mensajes->mensajes = list_create();
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

	pthread_mutex_unlock(&mutex_queue_mensajes);

	if(debug_broker) log_debug(broker_logger, "2) Cache de mensajes lista!!!", NULL);
	
	if(debug_broker) log_debug(broker_logger, "3) Inicializando archivo de configuracion...", NULL);

	inicializar_archivo_de_configuracion();

	if(debug_broker) log_debug(broker_logger, "4) Configuraciones cargadas correctamente!!!", NULL);

	if(debug_broker) log_debug(broker_logger, "5) Configurando signals...", NULL);

	//configurar_signals();

	if(debug_broker) log_debug(broker_logger, "6) Signals configuradas correctamente!!!", NULL);

	iniciar_servidor();

	//primer_bloque = AsignarMemoriaInicial(tamanio_memoria_inicial,lista_memoria);
	while(server_status != ENDING){
	
	}


}