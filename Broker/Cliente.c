#include <Pokebola.h>

int main(){

	int socket =  conectar_a_server("127.0.0.1","6009");

	//enviar_mensaje_string(socket, "hola_broker");

	/* Appeared/Catch Pokemon*/
	/*t_catch_pokemon appeared_pokemon;

	appeared_pokemon.coordenadas.posx = 10;
	appeared_pokemon.coordenadas.posy = 20;
	appeared_pokemon.pokemon = "pikachu";

	enviar_catch_pokemon(socket, -1, -1, &appeared_pokemon);*/

	/* New Pokemon*/
	
	/* Para enviar solamente tengo que crear la estructura
	   y llamar a la funcion de envio correspondiente pasandole
	   un puntero a la misma*/
	t_new_pokemon new_pokemon;

	new_pokemon.coordenadas.posx = 10;
	new_pokemon.coordenadas.posy = 20;
	new_pokemon.cantidad = 7;
	new_pokemon.pokemon = "pikachu";

	enviar_new_pokemon(socket,-1,-1,&new_pokemon);

	/* Caught Pokemon*/
	/*t_caught_pokemon caught_pokemon;

	caught_pokemon.status = 1;

	enviar_caught_pokemon(socket,-1,-1,&caught_pokemon);*/
	
	/* 
	t_get_pokemon get_pokemon;
	get_pokemon.pokemon = "pepe_el_dino";*/
	

/*
	t_suscripcion suscripcion;
	suscripcion.minutos_suscripcion = -1;
	suscripcion.tipo_suscripcion = SUSCRIPCION_GLOBAL;
	enviar_solicitud_suscripcion(socket,COLA_CATCH_POKEMON,&suscripcion);*/
	
}
