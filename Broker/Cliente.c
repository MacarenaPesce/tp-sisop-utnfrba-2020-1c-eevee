#include "Cliente.h"

t_servidor servidor;

/*  
	-Primera prueba con pokemons_maca
	1)
	Orden: 		new, 	get, 	apparead, 	caugth
	Resultado:	23		11		19			4
	Esperado:	23		11		19			4


	*/

int main(){

/*
	Creo un server donde voy a mandar mensajes (Broker)
*/

	t_packed* ack;

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

	/* Localized Pokemon*/
	t_localized_pokemon* localized_pokemon = generar_localized("Pikachu");

	//crear coordenadas
	t_coordenadas coordenadas;
	coordenadas.posx = 21;
	coordenadas.posy = 22;

	agregar_coordenadas_a_localized(localized_pokemon,&coordenadas);

/*	
	int socket_get_pokemon = enviar_solicitud_suscripcion(&servidor,COLA_LOCALIZED_POKEMON);
	int socket_get_pokemon2 = enviar_solicitud_suscripcion(&servidor,COLA_APPEARED_POKEMON);
	int socket_get_pokemon3 = enviar_solicitud_suscripcion(&servidor,COLA_CAUGHT_POKEMON);
	socket_get_pokemon3 = enviar_solicitud_suscripcion(&servidor,COLA_CAUGHT_POKEMON); 

	esperar_mensajes(&socket_get_pokemon);  

	pthread_t hilo_espera_mensajes;
	pthread_create(&hilo_espera_mensajes,NULL,esperar_mensajes,(void*)&socket_get_pokemon);
 */

	ack = enviar_get_pokemon(&servidor,-1, &get_pokemon);
	free(ack);

/* 
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

	ack = enviar_localized_pokemon(&servidor,-1,localized_pokemon);
	free(ack);
*/

	/*Pokemons de prueba para Macaa*/
	t_new_pokemon new_pokemon_maca;
	new_pokemon_maca.coordenadas.posx = 5;
	new_pokemon_maca.coordenadas.posy = 10;
	new_pokemon_maca.cantidad = 2;
	new_pokemon_maca.pokemon = "Pikachu";

	t_get_pokemon get_pokemon_maca;
	get_pokemon_maca.pokemon = "Pikachu";

	t_catch_pokemon appeared_pokemon_maca;
	appeared_pokemon_maca.coordenadas.posx = 1;
	appeared_pokemon_maca.coordenadas.posy = 5;
	appeared_pokemon_maca.pokemon = "Pikachu";

	t_caught_pokemon caught_pokemon_maca;
	caught_pokemon_maca.status = 0;

	t_catch_pokemon appeared_pokemon_maca2;
	appeared_pokemon_maca2.coordenadas.posx = 1;
	appeared_pokemon_maca2.coordenadas.posy = 5;
	appeared_pokemon_maca2.pokemon = "Tengotreintabytes";

	t_new_pokemon new_pokemon_maca2;
	new_pokemon_maca2.coordenadas.posx = 5;
	new_pokemon_maca2.coordenadas.posy = 10;
	new_pokemon_maca2.cantidad = 2;
	new_pokemon_maca2.pokemon = "TengounnombremuylargoparaochentabytesysigosinllegarAAAAAAAAAAAA";

/* 	ack = enviar_new_pokemon(&servidor,-1,&new_pokemon_maca);
	free(ack);

 	ack = enviar_get_pokemon(&servidor,-1, &get_pokemon_maca);
	free(ack); 

	ack = enviar_appeared_pokemon(&servidor,-1, &appeared_pokemon_maca);
	free(ack);

	ack = enviar_caught_pokemon(&servidor,-1, &caught_pokemon_maca);
	free(ack);

	ack = enviar_appeared_pokemon(&servidor,-1, &appeared_pokemon_maca2);
	free(ack);

	ack = enviar_new_pokemon(&servidor,-1, &new_pokemon_maca2);
	free(ack); */

	printf("TERMINE DE PROBAR LISTO   \n");


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
