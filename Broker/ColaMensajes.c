#include "ColaMensajes.h"

//pthread_mutex_t mutex_queue_mensajes;

int traducir_mensaje(t_packed *paquete,int socket_cliente){

	//enviar_ack(socket_cliente,123,-1);
	
	switch(paquete->cola_de_mensajes){

		case COLA_NEW_POKEMON:;
			/* Genero un puntero de ese tipo y lo inicializo */
			t_new_pokemon* new_pokemon;
			new_pokemon =(t_new_pokemon*)malloc(sizeof(t_new_pokemon));

			/* Apunto a los datos del mensaje */
			new_pokemon = paquete->mensaje;

			/* Ya puedo usar mi copia de la estructura enviada*/
		//	int id_mensaje = list_add(lista_mensajes, void *element);

			//enviar_ack(socket_cliente,id_mensaje,-1);

			break;

		case COLA_GET_POKEMON:;
			/* Genero un puntero de ese tipo y lo inicializo */
			t_get_pokemon* get_pokemon;
			get_pokemon =(t_get_pokemon*)malloc(sizeof(t_get_pokemon));

			/* Apunto a los datos del mensaje */
			get_pokemon = paquete->mensaje;

			/* Ya puedo usar mi copia de la estructura enviada*/
			if(paquete->tamanio_payload > 0){
				printf("pokemon: %s \n",get_pokemon->pokemon);
			}

			break;

		case COLA_CAUGHT_POKEMON:;
			/* Genero un puntero de ese tipo y lo inicializo */
			t_caught_pokemon* caught_pokemon;
			caught_pokemon =(t_caught_pokemon*)malloc(sizeof(t_caught_pokemon));

			/* Apunto a los datos del mensaje */
			caught_pokemon = paquete->mensaje;

			/* Ya puedo usar mi copia de la estructura enviada*/
			if(paquete->tamanio_payload > 0){
				printf("status: %d \n",caught_pokemon->status);
			}

			break;

		case COLA_APPEARED_POKEMON:;
			/* Genero un puntero de ese tipo y lo inicializo */
			t_appeared_pokemon* appeared_pokemon;
			appeared_pokemon =(t_appeared_pokemon*)malloc(sizeof(t_appeared_pokemon));

			/* Apunto a los datos del mensaje */
			appeared_pokemon = paquete->mensaje;

			/* Ya puedo usar mi copia de la estructura enviada*/
			if(paquete->tamanio_payload > 0){
				printf("posx: %d \n",appeared_pokemon->coordenadas.posx);
				printf("posy: %d \n",appeared_pokemon->coordenadas.posy);
				printf("pokemon: %s \n",appeared_pokemon->pokemon);
			}

			break;

		case COLA_CATCH_POKEMON:;
			/* Genero un puntero de ese tipo y lo inicializo */
			t_catch_pokemon* catch_pokemon;
			catch_pokemon =(t_catch_pokemon*)malloc(sizeof(t_catch_pokemon));

			/* Apunto a los datos del mensaje */
			catch_pokemon = paquete->mensaje;

			/* Ya puedo usar mi copia de la estructura enviada*/
			if(paquete->tamanio_payload > 0){
				printf("posx: %d \n",catch_pokemon->coordenadas.posx);
				printf("posy: %d \n",catch_pokemon->coordenadas.posy);
				printf("pokemon: %s \n",catch_pokemon->pokemon);
			}

			break;

		case COLA_LOCALIZED_POKEMON:;
			//TODO
			break;

	}

	/*Libero la memoria del paquete*/
	eliminar_mensaje(paquete);
	
}

int agregar_mensaje_a_cola(t_packed* paquete){

	t_mensaje_cola* mensaje;
	mensaje = (t_mensaje_cola*)malloc(sizeof(t_mensaje_cola));

	void* mensaje_cola;
	mensaje_cola = malloc(sizeof(t_mensaje_cola));

	mensaje->cola_de_mensajes = paquete->cola_de_mensajes;
	mensaje->id_correlacional = paquete->id_correlacional;
	mensaje->mensaje = paquete->mensaje;

	mensaje_cola = mensaje;
	
	return list_add(&cola_mensajes,mensaje_cola);

}