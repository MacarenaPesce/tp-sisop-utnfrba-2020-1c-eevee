#include "Cliente.h"

t_servidor servidor;


/*
	Prueba base funca bien 

	Prueba compleja  base, funca bien con 4 y 16 


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
	/*
	ack = enviar_get_pokemon(&servidor,-1, &get_pokemon);
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
	free(ack); 

	ack = enviar_localized_pokemon(&servidor,-1,localized_pokemon);
	free(ack);
*/

	/* Cargar memoria simple */
	/*
	t_get_pokemon get_pokemon1;
	get_pokemon1.pokemon = "Pikachu";
	t_get_pokemon get_pokemon2;
	get_pokemon2.pokemon = "Charmander";
	ack = enviar_get_pokemon(&servidor,-1, &get_pokemon1);
	free(ack);
	ack = enviar_get_pokemon(&servidor,-1, &get_pokemon2);
	free(ack);
	*/


	/* Cargar memoria complejo */
	/*
	t_catch_pokemon catch_pokemon1;
	catch_pokemon1.coordenadas.posx = 9;
	catch_pokemon1.coordenadas.posy = 3;
	catch_pokemon1.pokemon = "Pikachu";
	ack = enviar_catch_pokemon(&servidor,-1, &catch_pokemon1);
	free(ack);
	t_catch_pokemon catch_pokemon2;
	catch_pokemon2.coordenadas.posx = 9;
	catch_pokemon2.coordenadas.posy = 3;
	catch_pokemon2.pokemon = "Squirtle";
	ack = enviar_catch_pokemon(&servidor,-1, &catch_pokemon2);
	free(ack);

	t_caught_pokemon caught_pokemon1;
	caught_pokemon1.status = 10;
	ack = enviar_caught_pokemon(&servidor,-1, &caught_pokemon1);
	free(ack); 	

	t_caught_pokemon caught_pokemon2;
	caught_pokemon2.status = 11;
	ack = enviar_caught_pokemon(&servidor,-1, &caught_pokemon2);
	free(ack); 

	t_catch_pokemon catch_pokemon3;
	catch_pokemon3.coordenadas.posx = 1;
	catch_pokemon3.coordenadas.posy = 7;
	catch_pokemon3.pokemon = "Bulbasaur";
	ack = enviar_catch_pokemon(&servidor,-1, &catch_pokemon3);
	free(ack);

	t_catch_pokemon catch_pokemon4;
	catch_pokemon4.coordenadas.posx = 1;
	catch_pokemon4.coordenadas.posy = 7;
	catch_pokemon4.pokemon = "Charmander";
	ack = enviar_catch_pokemon(&servidor,-1, &catch_pokemon4);
	free(ack);

	t_get_pokemon get_pokemon3;
	get_pokemon3.pokemon = "Pichu";
	t_get_pokemon get_pokemon4;
	get_pokemon4.pokemon = "Raichu";
	ack = enviar_get_pokemon(&servidor,-1, &get_pokemon3);
	free(ack);
	ack = enviar_get_pokemon(&servidor,-1, &get_pokemon4);
	free(ack);
	*/	


	/* Prueba base consolidacion */
	/* t_caught_pokemon caught_pokemon1;
	caught_pokemon1.status = 10;
	ack = enviar_caught_pokemon(&servidor,-1, &caught_pokemon1);
	free(ack); 	

	t_caught_pokemon caught_pokemon2;
	caught_pokemon2.status = 11;
	ack = enviar_caught_pokemon(&servidor,-1, &caught_pokemon2);
	free(ack); 

	t_catch_pokemon catch_pokemon3;
	catch_pokemon3.coordenadas.posx = 1;
	catch_pokemon3.coordenadas.posy = 7;
	catch_pokemon3.pokemon = "Pikachu";
	ack = enviar_catch_pokemon(&servidor,-1, &catch_pokemon3);
	free(ack);

	t_catch_pokemon catch_pokemon4;
	catch_pokemon4.coordenadas.posx = 1;
	catch_pokemon4.coordenadas.posy = 7;
	catch_pokemon4.pokemon = "Squirtle";
	ack = enviar_catch_pokemon(&servidor,-1, &catch_pokemon4);
	free(ack);

	t_catch_pokemon catch_pokemon5;
	catch_pokemon5.coordenadas.posx = 4;
	catch_pokemon5.coordenadas.posy = 5;
	catch_pokemon5.pokemon = "Onyx";
	ack = enviar_catch_pokemon(&servidor,-1, &catch_pokemon5);
	free(ack);


	int socket_suscrip = enviar_solicitud_suscripcion(&servidor,COLA_CAUGHT_POKEMON);

	t_catch_pokemon catch_pokemon6;
	catch_pokemon6.coordenadas.posx = 9;
	catch_pokemon6.coordenadas.posy = 3;
	catch_pokemon6.pokemon = "Charmander";
	ack = enviar_catch_pokemon(&servidor,-1, &catch_pokemon6);
	free(ack);*/


	/*Prueba compactacion basico*/
	t_caught_pokemon caught_pokemon1;
	caught_pokemon1.status = 1;
	ack = enviar_caught_pokemon(&servidor,-1, &caught_pokemon1);
	free(ack); 	

	t_caught_pokemon caught_pokemon2;
	caught_pokemon2.status = 2;
	ack = enviar_caught_pokemon(&servidor,-1, &caught_pokemon2);
	free(ack); 

	t_catch_pokemon catch_pokemon3;
	catch_pokemon3.coordenadas.posx = 1;
	catch_pokemon3.coordenadas.posy = 7;
	catch_pokemon3.pokemon = "Pikachu";
	ack = enviar_catch_pokemon(&servidor,-1, &catch_pokemon3);
	free(ack);

	t_catch_pokemon catch_pokemon4;
	catch_pokemon4.coordenadas.posx = 1;
	catch_pokemon4.coordenadas.posy = 7;
	catch_pokemon4.pokemon = "Squirtle";
	ack = enviar_catch_pokemon(&servidor,-1, &catch_pokemon4);
	free(ack);

	t_catch_pokemon catch_pokemon5;
	catch_pokemon5.coordenadas.posx = 4;
	catch_pokemon5.coordenadas.posy = 5;
	catch_pokemon5.pokemon = "Onyx";
	ack = enviar_catch_pokemon(&servidor,-1, &catch_pokemon5);
	free(ack);


	int socket_suscrip = enviar_solicitud_suscripcion(&servidor,COLA_CAUGHT_POKEMON);


	t_catch_pokemon catch_pokemon6;
	catch_pokemon6.coordenadas.posx = 9;
	catch_pokemon6.coordenadas.posy = 3;
	catch_pokemon6.pokemon = "Vaporeon";
	ack = enviar_catch_pokemon(&servidor,-1, &catch_pokemon6);
	free(ack);

	eliminar_localized_pokemon(localized_pokemon);



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
