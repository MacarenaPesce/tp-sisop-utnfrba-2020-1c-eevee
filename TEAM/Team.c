/*
 * Team.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Team.h"

void recibir_appeared_pokemon_desde_gameboy(t_appeared_pokemon * mensaje){
	log_info(team_logger,"Voy a recibir un pokemon y coordenadas");

	log_info(team_logger,"Me llego este pokemon: %s", mensaje->pokemon);
	log_info(team_logger,"La coordenada X es: %d", mensaje->coordenadas.posx);
	log_info(team_logger,"La coordenada Y es: %d", mensaje->coordenadas.posy);
	free(mensaje->pokemon);
	free(mensaje);
}

void definir_objetivo_global(){

	/*Leo del archivo de config y guardo los pokemon en la lista pokemones*/

	char **read_array = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");
	lista_config = list_create();
	pokemones_ordenada = list_create();
	string_iterate_lines(read_array, _imprimir);

	/*Creo una nueva lista con los pokemon agrupados por especie*/

	pokemones_ordenada = list_sorted(lista_config, (void*)ordenar);
	//list_iterate(pokemones_ordenada, mostrar);
	t_objetivo* objetivo = malloc(sizeof(t_objetivo));
	char* unPokemon;
	unPokemon = list_get(pokemones_ordenada, 0);
	uint32_t contador = 0;
	lista_objetivos = malloc(sizeof(t_list));

	/*Empiezo a cargar a lista de objetivo global, con tipo y cantidad de cada uno*/
	log_info(team_logger,"Cargando el objetivo global... \n");
	char* especiePokemon;
	char* otroPokemon;
	int i = 0;
	while(pokemones_ordenada != NULL){
			especiePokemon = unPokemon;
			otroPokemon = list_get(pokemones_ordenada, i);
			if(otroPokemon == NULL){
				objetivo->especie = string_from_format("%s\0",especiePokemon);
				objetivo->cantidad = contador;
				log_info(team_logger,"Un tipo de pokemon es: %s y la cantidad es %i", objetivo->especie, objetivo->cantidad);
				list_add(lista_objetivos, (void*)objetivo);
				break;
			}
			if(string_equals_ignore_case(unPokemon,otroPokemon)){
				contador++;
				i++;
			}else{
				objetivo->especie = string_from_format("%s\0",especiePokemon);
				objetivo->cantidad = contador;
				log_info(team_logger,"Un tipo de pokemon es: %s y la cantidad es %i", objetivo->especie, objetivo->cantidad);
				list_add(lista_objetivos, (void*)objetivo);
				unPokemon = otroPokemon;
				contador = 1;
				i++;
			}
	}
	log_info(team_logger,"Objetivo global cargado\n");
	list_clean(lista_config);
}

void localizar_entrenadores_en_mapa(){
	char **read_array_posiciones = config_get_array_value(config, "POSICIONES_ENTRENADORES");
	char **read_array_objetivos = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");
	lista_entrenadores = list_create();

	string_iterate_lines(read_array_posiciones, _imprimir);
	//list_iterate(lista_config, mostrar);

	uint32_t i = 0;
	t_entrenador* entrenador = malloc(sizeof(t_entrenador));
	while(lista_config != NULL){
		char* coordenada_char;
		coordenada_char = list_get(lista_config, i);
		if(coordenada_char != NULL) {
			int coordenada = atoi(coordenada_char);
			if(i == 0 || i % 2 == 0) {
			entrenador->posx = coordenada;
			i++;

		}else{
			entrenador->posy = coordenada;
			entrenador->estado = NUEVO;
			entrenador->objetivo = NULL;
			entrenador->id = i;
			list_add(lista_entrenadores, entrenador); //esta seria la lista de new

			/*CREO UN HILO POR ENTRENADOR*/
			pthread_t hilo;
			pthread_create(&hilo,NULL,(void*)jugar_con_el_entrenador,(void*)entrenador);

			printf("Se agrego un entrenador con id %i, pos x = %i, pos y = %i\n",entrenador->id, entrenador->posx, entrenador->posy);
			i++;
		}

		} else{
			break;
		}
	}
	list_clean(lista_config);

}

void * jugar_con_el_entrenador(t_entrenador * entrenador){
	printf("Soy un hilo para el entrenador \n");
	//TODO MOVERLO EN EL MAPA / INTERCAMBIAR / ATRAPAR UN POKEMON..

	/**
	 * SI EL ESTADO ES EJECUTANDO, SIGNIFICA QUE HAY QUE HACER ALGUNA DE LAS FUNCIONES DE ARRIBA, USAR EL RETARDO DE EJECUCION Y USAR EL MAPA
	 */

}

void iniciar_servidor(void){
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP, PUERTO, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next){
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    while(1)
    	esperar_cliente(socket_servidor);
}

void esperar_cliente(int socket_servidor){
	struct sockaddr_in dir_cliente;
	int tam_direccion = sizeof(struct sockaddr_in);
	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
	log_info(team_logger,"\nSe aceptó un nuevo cliente");

	t_packed * paquete = recibir_mensaje(socket_cliente);

	switch(paquete->cola_de_mensajes){
		case COLA_APPEARED_POKEMON:
			recibir_appeared_pokemon_desde_gameboy(paquete->mensaje);
			break;
		default:
			printf("Error\n");
	}

}

void seleccionar_el_entrenador_mas_cercano_al_pokemon(){
	/*
	 * Para poder planificar un entrenador, se seleccionará el hilo del entrenador más cercano al Pokémon.
Cada movimiento en el mapa responderá a un ciclo de CPU, y este NO realizará movimientos
diagonales para llegar a la posición deseada. Para simular más a la realidad esta funcionalidad, se
deberá agregar un retardo de X segundos configurado por archivo de configuración.
	 */

	//TODO

	//AAUUUUXXXIIIILLLLLIIIIIOOOO
	/////////////////??? SI VAS A AGARRAR SIEMPRE AL MAS CERCANO PARA QUE NECESITAS LOS ALGORITMOS???? PREGUNTAR ESTO URGENTE
}

void meter_entrenadores_en_ready(){
	//TODO ACA ES CUANDO ENTRAN EN JUEGO LOS HILOS, SE PONE CADA ENTRENADOR EN LA LISTA_LISTOS


}

void chequear_si_hay_pokemones_nuevos(){
	//TODO
	//aca hay que ver si llegaron solicitudes desde gameboy, o si el broker dice que llegó un pokemon
	meter_entrenadores_en_ready();
}

void planificar(){
	/* Cada entrenador al iniciar en el sistema entrará en estado New. ------- ESTO YA ESTA! CUANDO SE ORDENA LA LISTA DE ENTRENADORES */

	/*A medida que el Team empiece a recibir distintos Pokémon en el mapa despertará a los distintos entrenadores en estado New o en
Blocked (que estén esperando para procesar) pasandolos a Ready. */
	chequear_si_hay_pokemones_nuevos();

	/*Siempre se planificará aquel entrenador que se encuentre sin estar realizando ninguna operación activamente y, en caso de
existir más de uno, sea el que más cerca se encuentre del objetivo.
A medida que cada entrenador se planifique (ya sea para moverse, intercambiar o atrapar un
Pokémon) entrarán en estado exec. En el contexto de nuestro trabajo practico no contemplaremos el
multiprocesamiento, esto implica que solo UN entrenador podrá estar en estado Exec en
determinado tiempo.
Cuando un entrenador en estado Exec finalice su recorrido y su ejecución planificada entrará en un
estado bloqueados. Este estado implica que el entrenador no tiene más tareas para realizar
momentáneamente.
Cuando un entrenador en estado Exec cumpla todos sus objetivos, pasará a estado Exit. Cuando
todos los entrenadores dentro de un Team se encuentren en Exit, se considera que el proceso Team
cumplió el objetivo global.
	 */
	seleccionar_el_entrenador_mas_cercano_al_pokemon(); /// ??? SI VAS A AGARRAR SIEMPRE AL MAS CERCANO PARA QUE NECESITAS LOS ALGORITMOS???? PREGUNTAR ESTO URGENTE
	obtener_proximo_ejecucion();
}


int hacer_intento_de_reconexion(){
	sleep(tiempo_reconexion);
	int broker_socket = conectar_broker();
	return broker_socket;
}

int conectar_broker() {
	/*
	Cabe aclarar que el Proceso Team debe poder ejecutarse sin haber establecido la conexión con el	Broker.
	Es decir, si el broker se encuentra sin funcionar o se cae durante la ejecución, el proceso Team debe seguir procesando sus funciones sin el mismo.
	Para esto, se contarán con funciones 	default para aquellos mensajes que el Proceso Team envíe directamente al Broker.
	En caso que la conexión no llegue a realizarse o se caiga, el proceso Team deberá contar con un sistema de reintento de conexión cada X segundos
	configurado desde archivo de configuración.
		 */

	int broker_socket = conectar_a_server(ip_broker, puerto_broker);
	if (broker_socket < 0){
		log_info(team_logger, "Error al intentar conectar al broker\n");
		broker_socket = hacer_intento_de_reconexion();
	}
	else{
		log_info(team_logger, "Conectado con el broker! (%d)",broker_socket);
	}

	return broker_socket;
}


void conectarse_a_colas_de_broker(){
	/*
	Cabe aclarar que el Proceso Team debe poder ejecutarse sin haber establecido la conexión con el	Broker.
	Es decir, si el broker se encuentra sin funcionar o se cae durante la ejecución, el proceso Team debe seguir procesando sus funciones sin el mismo.
	Para esto, se contarán con funciones 	default para aquellos mensajes que el Proceso Team envíe directamente al Broker.
	En caso que la conexión no llegue a realizarse o se caiga, el proceso Team deberá contar con un sistema de reintento de conexión cada X segundos
	configurado desde archivo de configuración.
		 */

	int broker_socket = conectar_a_server(ip_broker, puerto_broker);
	if (broker_socket < 0){
		log_info(team_logger, "Error al intentar conectar al broker\n");
		broker_socket = hacer_intento_de_reconexion();
	}
	else{
		log_info(team_logger, "Conectado con el broker! (%d)",broker_socket);
	}

	return broker_socket;
}

void enviar_get(int socket_broker){
	/*
Este mensaje se ejecutará al iniciar el proceso Team. El objetivo del mismo es obtener todas las locaciones de una especie de Pokemon.
De esta manera, al iniciar el proceso, por cada especie de Pokémon requerido se debe enviar un mensaje a la cola de mensajes GET_POKEMON del Broker.
Para esto se deben ejecutar los siguientes pasos:
1. Enviar el mensaje a la cola de mensajes GET_POKEMON indicando cual es la especie del Pokemon.
2. Obtener el ID del mensaje anterior desde el Broker.
En caso que el Broker no se encuentre funcionando o la conexión inicial falle, se deberá tomar como comportamiento Default que no existen locaciones para la especie requerida.
	 */
	if (socket_broker < 0){
		log_info(team_logger, "NO EXISTEN LOCACIONES PARA LA ESPECIE REQUERIDA\n");
	}
	else{
		enviar_mensaje_por_cada_pokemon_requerido(socket_broker);
	}

}

t_objetivo * sacar_objetivo_de_la_lista(t_list* lista, char* especie){
	bool is_especie(t_objetivo * objetivo){
		return !strcmp(objetivo->especie, especie);
	}
	return (list_remove_by_condition(lista,(void*)is_especie));
}

void enviar_mensaje_por_cada_pokemon_requerido(int socket_broker){
	t_list * lista_aux;
	t_objetivo * objetivo;
	lista_aux = list_duplicate(lista_objetivos);

	if(!list_is_empty(lista_objetivos)){

		while(!list_is_empty(lista_aux)){
			objetivo = sacar_objetivo_de_la_lista(lista_objetivos, objetivo->especie);

			t_get_pokemon* get_pokemon = malloc(sizeof(t_get_pokemon));
			get_pokemon->pokemon = objetivo->especie;
			enviar_get_pokemon(socket_broker, -1, -1, get_pokemon);
			free(get_pokemon);

			//RECIBIR ACK
		}
	}
	else{
		log_info(team_logger, "ERROR! No hay OBJETIVOS");
	}

	list_destroy(lista_aux);
}

void convertirse_en_suscriptor_global_del_broker(int socket_broker){
	enviar_solicitud_suscripcion(socket_broker, COLA_APPEARED_POKEMON, SUSCRIPCION_GLOBAL);
	//recibir ACK
	enviar_solicitud_suscripcion(socket_broker, COLA_CAUGHT_POKEMON, SUSCRIPCION_GLOBAL);
	//recibir ACK
	enviar_solicitud_suscripcion(socket_broker, COLA_LOCALIZED_POKEMON, SUSCRIPCION_GLOBAL);
	//recibir ACK
}


int main(){

	inicializar_logger();
	inicializar_archivo_de_configuracion();
	config = config_create("team.config");
	configurar_signals();
	inicializar_listas();//sacar los leaks
	definir_objetivo_global();//sacar los leaks
	localizar_entrenadores_en_mapa();

	int broker = conectar_broker();
	enviar_get(broker);
	convertirse_en_suscriptor_global_del_broker(broker);

	iniciar_servidor();

	planificar();

	//liberar el config de arriba
}
