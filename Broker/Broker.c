#include "Broker.h"

int main(){

	pthread_t hilo_sender[COLA_LOCALIZED_POKEMON];

	printf("\n1) Inicializando cache de mensajes... \n");

	pthread_mutex_lock(&mutex_queue_mensajes);

	cache_mensajes = (t_cache_colas*)malloc(sizeof(t_cache_colas));
	cache_mensajes->mensajes = list_create();
	cache_mensajes->colas = list_create();
	cache_mensajes->proximo_id_mensaje = 0;

	t_cola_mensajes* aux_crear_cola_mensajes; 

	for(int i = COLA_APPEARED_POKEMON; i <= COLA_LOCALIZED_POKEMON; i++){

		aux_crear_cola_mensajes = crear_cola_mensajes(i);

		list_add(cache_mensajes->colas,aux_crear_cola_mensajes);

		pthread_create(&hilo_sender[i],NULL,sender_suscriptores,aux_crear_cola_mensajes);	

		pthread_detach(hilo_sender[i]);	
		
	}

	pthread_mutex_unlock(&mutex_queue_mensajes);

	printf("\n2) Cache de mensajes lista!!! ");

	printf("\n\n3) Inicializando archivo de configuracion...\n\n");

	inicializar_archivo_de_configuracion();

	printf("\n4) Configuraciones cargadas correctamente!!!");
	
	printf("\n\n5) Configurando signals...");

	configurar_signals();

	printf("\n\n6) Signals configuradas correctamente!!!");

	iniciar_servidor();

	/* TODO: Obtener este valor desde la config */
	int tamanio_memoria_inicial = 8000;

	//primer_bloque = AsignarMemoriaInicial(tamanio_memoria_inicial,lista_memoria);
	while(server_status != ENDING){
	
	}

}