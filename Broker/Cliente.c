#include "Cliente.h"

t_servidor servidor;


int main(){

/*
	Creo un server donde voy a mandar mensajes (Broker)
*/

	servidor.ip = "127.0.0.1";
	servidor.puerto = "5003";
	servidor.id_cliente = 123332;

/*
	Para enviar solamente tengo que crear la estructura
	y llamar a la funcion de envio correspondiente pasandole
	un puntero a la misma
*/

	/* Appeared/Catch Pokemon */
	t_catch_pokemon appeared_pokemon;

	appeared_pokemon.coordenadas.posx = 10;
	appeared_pokemon.coordenadas.posy = 20;
	appeared_pokemon.pokemon = "pikachu";

	/* New Pokemon*/
	t_new_pokemon new_pokemon;

	new_pokemon.coordenadas.posx = 10;
	new_pokemon.coordenadas.posy = 20;
	new_pokemon.cantidad = 7;
	new_pokemon.pokemon = "pikachu";

	/* Caught Pokemon*/
	t_caught_pokemon caught_pokemon;
	caught_pokemon.status = 1;

	/* Get Pokemon*/
	t_get_pokemon get_pokemon;
	get_pokemon.pokemon = "pepe_el_dino";

	/* Suscripcion */
	t_suscripcion suscripcion;
	suscripcion.minutos_suscripcion = -1;
	suscripcion.tipo_suscripcion = SUSCRIPCION_GLOBAL;

	t_packed* ack;

	int socket_get_pokemon = enviar_solicitud_suscripcion(&servidor,COLA_GET_POKEMON,&suscripcion);

	pthread_t hilo_espera_mensajes;
	pthread_create(&hilo_espera_mensajes,NULL,esperar_mensajes,(void*)&socket_get_pokemon);

/* 	ack = enviar_get_pokemon(&servidor,-1, &get_pokemon);
	free(ack); */

	ack = enviar_appeared_pokemon(&servidor,-1, &appeared_pokemon);
	free(ack);
	ack = enviar_new_pokemon(&servidor,-1,&new_pokemon);
	free(ack);
	ack = enviar_catch_pokemon(&servidor,-1, &appeared_pokemon);
	free(ack);
	ack = enviar_get_pokemon(&servidor,-1, &get_pokemon);
	free(ack);
	ack = enviar_caught_pokemon(&servidor,-1, &caught_pokemon);
	free(ack);

	while(1){};





	//enviar_mensaje_string(socket, "hola_broker");

	/*esperar_mensajes(socket_get_pokemon);*/



}


void* esperar_mensajes(void* socket){

	int socket_server = *((int*) socket);

	while(1){
		//Creo un paquete y recibo el mensaje
		t_packed* paquete;
		paquete = recibir_mensaje(socket_server);

		if(paquete != (t_packed*)-1){
			//Esto me devuelve el paquete con todos los datos
			/* El nro de operacion y cola de mensajes indican el
			tipo de estructura que contiene el paquete */
			printf("\n\nMensaje Recibido: %d \n",paquete->operacion);
			printf("operacion: %d \n",paquete->operacion);
			printf("cola_de_mensajes: %d \n",paquete->cola_de_mensajes);
			printf("id_correlacional: %d  \n",paquete->id_correlacional);
			printf("id_mensaje: %d \n",paquete->id_mensaje);
			printf("tamanio_payload: %d \n",paquete->tamanio_payload);
			if(paquete->operacion == 0) {
				enviar_ack(&servidor,paquete->id_mensaje);
				if(paquete->cola_de_mensajes == COLA_GET_POKEMON){
					t_get_pokemon* get_pokemon = (t_get_pokemon*) paquete->mensaje;
					printf("recibi el pokemon %s que me envió el broker \n",get_pokemon->pokemon);
				}
			}
			free(paquete);
 		}

	}

	return NULL;
}
