#include "Cliente.h"

t_servidor servidor;


int main(){

/*
	Creo un server donde voy a mandar mensajes (Broker)
*/

	servidor.ip = "127.0.0.1";
	servidor.puerto = "5003";
	servidor.id_cliente = 12332;

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
	get_pokemon.pokemon = "Pikachu";

	//t_get_pokemon get_pokemon2;
	//get_pokemon.pokemon = "Charmander";

	t_packed* ack;

	/*int socket_get_pokemon = enviar_solicitud_suscripcion(&servidor,COLA_LOCALIZED_POKEMON);
	int socket_get_pokemon2 = enviar_solicitud_suscripcion(&servidor,COLA_APPEARED_POKEMON);
	int socket_get_pokemon3 = enviar_solicitud_suscripcion(&servidor,COLA_CAUGHT_POKEMON);
	socket_get_pokemon3 = enviar_solicitud_suscripcion(&servidor,COLA_CAUGHT_POKEMON); */

/* 	sleep(2); */
/* 
	esperar_mensajes(&socket_get_pokemon);  */


/* 	pthread_t hilo_espera_mensajes;
	pthread_create(&hilo_espera_mensajes,NULL,esperar_mensajes,(void*)&socket_get_pokemon);
 */

 	/*ack = enviar_get_pokemon(&servidor,-1, &get_pokemon);
	free(ack); 

	ack = enviar_appeared_pokemon(&servidor,-1, &appeared_pokemon);
	free(ack);

	ack = enviar_new_pokemon(&servidor,-1,&new_pokemon);
	free(ack);

	ack = enviar_catch_pokemon(&servidor,-1, &appeared_pokemon);
	free(ack);

	ack = enviar_get_pokemon(&servidor,-1, &get_pokemon);
	free(ack);

	ack = enviar_caught_pokemon(&servidor,-1, &caught_pokemon);
	free(ack);*/

	/*Pokemons de prueba para Macaa*/
	t_new_pokemon new_pokemon_maca;
	new_pokemon.coordenadas.posx = 5;
	new_pokemon.coordenadas.posy = 10;
	new_pokemon.cantidad = 2;
	new_pokemon.pokemon = "pikachu";

	t_get_pokemon get_pokemon_maca;
	get_pokemon.pokemon = "pikachu";

	t_catch_pokemon appeared_pokemon_maca;
	appeared_pokemon_maca.coordenadas.posx = 1;
	appeared_pokemon_maca.coordenadas.posy = 5;
	appeared_pokemon_maca.pokemon = "pikachu";

	t_caught_pokemon caught_pokemon_maca;
	caught_pokemon.status = 0;

	ack = enviar_new_pokemon(&servidor,-1,&new_pokemon_maca);
	free(ack);

 	ack = enviar_get_pokemon(&servidor,-1, &get_pokemon_maca);
	free(ack); 

	ack = enviar_appeared_pokemon(&servidor,-1, &appeared_pokemon_maca);
	free(ack);

	ack = enviar_caught_pokemon(&servidor,-1, &caught_pokemon_maca);
	free(ack);

	//enviar_mensaje_string(socket, "hola_broker");

	/*esperar_mensajes(socket_get_pokemon);*/



}


void* esperar_mensajes(void* socket){

	int socket_server = *((int*) socket);

	while(1){
		//Creo un paquete y recibo el mensaje
		t_packed* paquete;
		paquete = recibir_mensaje(socket_server);
		if(paquete == (t_packed*)-2) break;


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
