#include "Broker.h"

int main(){

	cache_mensajes = (t_cache_colas*)malloc(sizeof(t_cache_colas));
	cache_mensajes->mensajes = list_create();
	cache_mensajes->colas = list_create();

	t_cola_mensajes* aux_crear_cola_mensajes; 

	pthread_t hilo_sender[COLA_LOCALIZED_POKEMON];

	for(int i = COLA_APPEARED_POKEMON; i <= COLA_LOCALIZED_POKEMON; i++){

		aux_crear_cola_mensajes = crear_cola_mensajes(i);

		list_add(cache_mensajes->colas,aux_crear_cola_mensajes);

		pthread_create(&hilo_sender[i],NULL,sender_suscriptores,aux_crear_cola_mensajes);
		
	}

	/*for(int i = COLA_APPEARED_POKEMON; i <= COLA_LOCALIZED_POKEMON; i++){

		pthread_join(&hilo_sender[i],NULL);
		
	}*/

	inicializar_archivo_de_configuracion();
	
	configurar_signals();

	iniciar_servidor();

	/* TODO: Obtener este valor desde la config */
	int tamanio_memoria_inicial = 8000;

	//primer_bloque = AsignarMemoriaInicial(tamanio_memoria_inicial,lista_memoria);
	while(true){
	
	}
}


t_cola_mensajes* crear_cola_mensajes(int cola_mensajes){

	t_cola_mensajes* cola_mensaje; 
	cola_mensaje = (t_cola_mensajes*)malloc(sizeof(t_cola_mensajes));

	sem_t* producciones;
	producciones = (sem_t*)malloc(sizeof(sem_t));

	sem_init(producciones, 0, 0); 	

	cola_mensaje->cola_de_mensajes = cola_mensajes;
	cola_mensaje->envios_pendientes = list_create();
	cola_mensaje->suscriptores = list_create();
	cola_mensaje->producciones = producciones;


	list_add(cola_mensaje->suscriptores,123);
	list_add(cola_mensaje->suscriptores,234);

	return cola_mensaje;

}

void* sender_suscriptores(t_cola_mensajes* cola){

	t_mensaje_cola* mensaje;

	t_envio_pendiente* envio_pendiente;

	while(1){

		sem_wait(cola->producciones);	

		pthread_mutex_lock(&mutex_queue_mensajes);	

		printf("\n\nLos pendientes tienen %d mensajes",(cola->envios_pendientes)->elements_count);
		envio_pendiente = list_take_and_remove(cola->envios_pendientes,1)->head->data;

		printf("\nEnviando mensaje de id %d de cola %d al cliente %d!!!",(int)envio_pendiente->id,cola->cola_de_mensajes,envio_pendiente->cliente);
		printf("\nLos pendientes quedaron con %d mensajes",(cola->envios_pendientes)->elements_count);

		mensaje = list_get(cache_mensajes->mensajes,envio_pendiente->id);

		//send

		pthread_mutex_unlock(&mutex_queue_mensajes);
	
	}

	return NULL;
}