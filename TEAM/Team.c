/*
 * Team.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Team.h"

void operar_con_appeared_pokemon(t_appeared_pokemon * mensaje){
	/*Este mensaje permitirá la inclusión en el proceso Team de un nuevo Pokémon en el mapa.
	Al llegar este mensaje, el proceso Team deberá verificar si requiere atrapar el mismo controlando los
	Pokemon globales necesarios y los ya atrapados. No se debe poder atrapar mas Pokemon de una especie de los requeridos globalmente.
	En caso que se requiera el mismo, se debe agregar a la lista de Pokémon requeridos y en el momento
	que un entrenador se encuentre en estado “Dormido” o “Libre” debe planificarlo para ir a atraparlo.
	En este mensaje se recibirán los siguientes parámetros:
	●Especie de Pokemon.
	●Posición del Pokemon.*/

	t_pokemon * pokemon = malloc(sizeof(t_pokemon));
	pokemon->especie = mensaje->pokemon;
	pokemon->posx = mensaje->coordenadas.posx;
	pokemon->posy = mensaje->coordenadas.posy;

	agregar_pokemon_a_mapa(pokemon);

	//planificar();
}

void operar_con_localized_pokemon(t_localized_pokemon * mensaje){
	/*
	 * El proceso Team se suscribirá de manera global a esta cola de mensajes. Al recibir uno de los mismos deberá realizar los siguientes pasos:
		1. Verificar si ya recibió en algún momento un mensaje de la especie del Pokémon asociado al mensaje. Si es así, descarta el mensaje (ya sea Appeared o Localized).
		2. En caso de que nunca lo haya recibido, realiza las mismas operatorias que para APPEARED_POKEMON por cada coordenada del pokemon.
	 */

	//planificar();


}

void operar_con_caught_pokemon(t_caught_pokemon * mensaje){
	/*
	 * El proceso Team se suscribirá de manera global a esta cola de mensajes. Al recibir uno de los mismos deberá realizar los siguientes pasos:
		1. Validar si el id de mensaje correlativo del mensaje corresponde a uno pendiente de respuesta generado por la la instrucción CATCH_POKEMON antes descrita.
		Si no corresponde  ninguno, ignorar el mensaje.
		2. En caso que corresponda se deberá validar si el resultado del mensaje es afirmativo (se trapó el Pokémon).
		Si es así se debe asignar al entrenador bloqueado el Pokémon y habilitarlo a poder volver operar.
	 */

	//planificar();

}

void agregar_pokemon_a_mapa(t_pokemon * pokemon){

	///ya tengo al pokemon???? NO
	list_add(lista_mapa, (void*)pokemon);

	///ya tengo al pokemon???? SI
	///ver que miercoles hacemos
}

void recibir_appeared_pokemon_desde_gameboy(t_appeared_pokemon * mensaje){
	log_info(team_logger,"Voy a recibir un pokemon y coordenadas desde gameboy");

	log_info(team_logger,"Me llego este pokemon: %s", mensaje->pokemon);
	log_info(team_logger,"La coordenada X es: %d", mensaje->coordenadas.posx);
	log_info(team_logger,"La coordenada Y es: %d", mensaje->coordenadas.posy);

	operar_con_appeared_pokemon(mensaje);

	free(mensaje->pokemon);
	free(mensaje);
}

void definir_objetivo_global(){

	/*Leo del archivo de config y guardo los pokemon en la lista pokemones*/

	lista_config = list_create();
	pokemones_ordenada = list_create();
	string_iterate_lines(objetivos_entrenadores, _imprimir);

	/*Creo una nueva lista con los pokemon agrupados por especie*/

	pokemones_ordenada = list_sorted(lista_config, (void*)ordenar);
	//list_iterate(pokemones_ordenada, mostrar);
	t_objetivo* objetivo = malloc(sizeof(t_objetivo));
	char* unPokemon;
	unPokemon = list_get(pokemones_ordenada, 0);
	uint32_t contador = 0;

	/*Empiezo a cargar a lista de objetivo global, con tipo y cantidad de cada uno*/
	log_info(team_logger,"Cargando el objetivo global... \n");
	char* especiePokemon;
	char* otroPokemon;
	int i = 0;
	while(pokemones_ordenada != NULL){
		especiePokemon = unPokemon;
		otroPokemon = list_get(pokemones_ordenada, i);
		if(otroPokemon == NULL){
			agregar_objetivo(especiePokemon, contador);
			break;
		}
		if(string_equals_ignore_case(unPokemon,otroPokemon)){
			contador++;
			i++;
		}else{
			agregar_objetivo(especiePokemon, contador);
			unPokemon = otroPokemon;
			contador = 1;
			i++;
		}
	}
	log_info(team_logger,"Objetivo global cargado\n");
	list_clean(lista_config);
	int k = 0;
			while(!list_is_empty(lista_objetivos)){
				objetivo = list_get(lista_objetivos, k);
				if(objetivo == NULL){
					break;
				}
				log_info(team_logger,"Un objetivo es de la especie = %s, cantidad %i\n", objetivo->especie, objetivo->cantidad);
				k++;
			}
	//free(objetivo);
}

void agregar_objetivo(char* especie, uint32_t cantidad){
	t_objetivo* objetivo = malloc(sizeof(t_objetivo));
	objetivo->especie = especie;
	objetivo->cantidad = cantidad;
	list_add(lista_objetivos, (void*)objetivo);

}


void localizar_entrenadores_en_mapa(){

	string_iterate_lines(posiciones_entrenadores, _imprimir);

	uint32_t i = 0;
	t_entrenador* entrenador = malloc(sizeof(t_entrenador));
	uint32_t posx;
	uint32_t posy;
	bool hay_que_agregar_entrenador = false;
	for(uint32_t i = 0; i < list_size(lista_config); i++){
		if(!hay_que_agregar_entrenador){
			char* coordenada_char;
			coordenada_char = list_get(lista_config, i);
			if(coordenada_char != NULL) {
					int coordenada = atoi(coordenada_char);
					if(i == 0 || i % 2 == 0) {
						posx = coordenada;
					}else{
						posy = coordenada;
						hay_que_agregar_entrenador = true;
					}
					if(hay_que_agregar_entrenador){
						agregar_entrenador(posx, posy, i);
						hay_que_agregar_entrenador = false;

					}

			}else{
				break;
			}
		}
	}
	int l = 0;
		while(!list_is_empty(lista_entrenadores)){
			entrenador = list_get(lista_entrenadores, l);
			if(entrenador == NULL){
				break;
			}
			log_info(team_logger,"Un entrenador tiene id = %i, pos x = %i, y = %i\n", entrenador->id, entrenador->posx, entrenador->posy);
			l++;
		}

	list_clean(lista_config);
	list_destroy(lista_config);

}

void agregar_entrenador(uint32_t posx, uint32_t posy, uint32_t id){
	t_entrenador* entrenador = malloc(sizeof(t_entrenador));
	entrenador->posx = posx;
	entrenador->posy = posy;
	entrenador->id = id;
	list_add(lista_entrenadores, (void*)entrenador);
	/*CREO UN HILO POR ENTRENADOR*/
	crear_hilo_entrenador(entrenador, jugar_con_el_entrenador(entrenador));
}

void crear_hilo_entrenador(t_entrenador * entrenador, void*funcion_a_ejecutar(t_entrenador*)){
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)funcion_a_ejecutar,(void*)entrenador);
}

void * jugar_con_el_entrenador(t_entrenador * entrenador){
	printf("Soy un hilo para el entrenador %d \n", entrenador->id);
	//TODO MOVERLO EN EL MAPA / INTERCAMBIAR / ATRAPAR UN POKEMON..

	/**
	 * SI EL ESTADO ES EJECUTANDO, SIGNIFICA QUE HAY QUE HACER ALGUNA DE LAS FUNCIONES DE ARRIBA, USAR EL RETARDO DE EJECUCION Y USAR EL MAPA
	 */

}

void seleccionar_el_entrenador_mas_cercano_al_pokemon(){
	/*
	 * Para poder planificar un entrenador, se seleccionará el hilo del entrenador más cercano al Pokémon.
Cada movimiento en el mapa responderá a un ciclo de CPU, y este NO realizará movimientos
diagonales para llegar a la posición deseada. Para simular más a la realidad esta funcionalidad, se
deberá agregar un retardo de X segundos configurado por archivo de configuración.
	 */

	//TODO
}

void meter_entrenadores_en_ready(){
	//TODO ACA ES CUANDO ENTRAN EN JUEGO LOS HILOS, SE PONE CADA ENTRENADOR EN LA LISTA_LISTOS

}

void chequear_si_hay_pokemones_nuevos(){
	//Aca hay que ver si llegaron solicitudes desde gameboy, o si el broker dice que llegó un pokemon -- con appeared o localized pokemon, para eso miramos el mapa
	if(lista_mapa != NULL){
		meter_entrenadores_en_ready();
	}
}

void planificar(){
	/* Cada entrenador al iniciar en el sistema entrará en estado New. ------- ESTO YA ESTA! CUANDO SE ORDENA LA LISTA DE ENTRENADORES */

	/*A medida que el Team empiece a recibir distintos Pokémon en el mapa despertará a los distintos entrenadores en estado New o en
	Blocked (que estén esperando para procesar) pasandolos a Ready. */
	chequear_si_hay_pokemones_nuevos();

	/*Siempre se planificará aquel entrenador que se encuentre sin estar realizando ninguna operación activamente y, en caso de
existir más de uno, sea el que más cerca se encuentre del objetivo. A medida que cada entrenador se planifique (ya sea para moverse, intercambiar o atrapar un
Pokémon) entrarán en estado exec. En el contexto de nuestro trabajo practico no contemplaremos el multiprocesamiento, esto implica que solo UN entrenador
podrá estar en estado Exec en determinado tiempo. Cuando un entrenador en estado Exec finalice su recorrido y su ejecución planificada entrará en un estado bloqueados.
Este estado implica que el entrenador no tiene más tareas para realizar momentáneamente. Cuando un entrenador en estado Exec cumpla todos sus objetivos, pasará a estado Exit.
Cuando todos los entrenadores dentro de un Team se encuentren en Exit, se considera que el proceso Team cumplió el objetivo global.
	 */
	seleccionar_el_entrenador_mas_cercano_al_pokemon();
	obtener_proximo_ejecucion();
}

void enviar_get(){
	int i = 0;
	int h = 0;
	t_objetivo* objetivo = malloc(sizeof(t_objetivo));
	while(lista_objetivos != NULL){
		objetivo = list_get(lista_objetivos, h);
		if(objetivo == NULL){
			break;
		}
		log_info(team_logger, "No existen locaciones para esta especie: %s, cant %i\n", objetivo->especie, objetivo->cantidad);
		h++;
	}
	//free(objetivo);
	if(broker_socket < 0 || broker_socket == 0){
		//int largo_lista = list_size(lista_objetivos);

		while(lista_objetivos != NULL){

			i++;
			if(objetivo == NULL){
				break;
			} else{
				log_info(team_logger, "No existen locaciones para la especie requerida: %s, cant %i\n", objetivo->especie, objetivo->cantidad);
			}

		}
		free(objetivo);
	}else{
		enviar_mensaje_por_cada_pokemon_requerido();
	}
}

t_objetivo * sacar_objetivo_de_la_lista(t_list* lista, char* especie){
	bool is_especie(t_objetivo * objetivo){
		return !strcmp(objetivo->especie, especie);
	}
	return (list_remove_by_condition(lista,(void*)is_especie));
}

void enviar_mensaje_por_cada_pokemon_requerido(){
	t_list * lista_aux;
	t_objetivo * objetivo;
	lista_aux = list_duplicate(lista_objetivos);

	if(!list_is_empty(lista_objetivos)){

		while(!list_is_empty(lista_aux)){
			objetivo = sacar_objetivo_de_la_lista(lista_objetivos, objetivo->especie);

			t_get_pokemon* get_pokemon = malloc(sizeof(t_get_pokemon));
			get_pokemon->pokemon = objetivo->especie;
			enviar_get_pokemon(broker_socket, -1, -1, get_pokemon);
			free(get_pokemon);
			//RECIBIR ACK
		}
	}else{
		log_info(team_logger, "ERROR! No hay OBJETIVOS");
	}

	list_destroy(lista_aux);
}

void convertirse_en_suscriptor_global_del_broker(){
	if(broker_socket < 0 || broker_socket == 0){
		log_info(team_logger, "EL BROKER ESTA CAIDO, NO SE PUEDE MANDAR EL MENSAJE DE SUSCRIPCION AHORA");
	}else{
		enviar_solicitud_suscripcion(broker_socket, COLA_APPEARED_POKEMON, SUSCRIPCION_GLOBAL);
		//recibir ACK
		enviar_solicitud_suscripcion(broker_socket, COLA_CAUGHT_POKEMON, SUSCRIPCION_GLOBAL);
		//recibir ACK
		enviar_solicitud_suscripcion(broker_socket, COLA_LOCALIZED_POKEMON, SUSCRIPCION_GLOBAL);
		//recibir ACK

	}
}

int recibir_mensaje_broker(){
	int read_size;

	t_packed * paquete = recibir_mensaje(broker_socket);
	read_size = paquete->tamanio_payload; //para chequear que no se cayó el broker

	log_info(team_logger, "Llegó un nuevo mensaje desde el broker!");

	if(paquete->cola_de_mensajes == COLA_APPEARED_POKEMON){
		operar_con_appeared_pokemon(paquete->mensaje);
	}

	if(paquete->cola_de_mensajes == COLA_LOCALIZED_POKEMON){
		operar_con_localized_pokemon(paquete->mensaje);
	}

	if(paquete->cola_de_mensajes == COLA_CAUGHT_POKEMON){
		operar_con_caught_pokemon(paquete->mensaje);
	}

	return read_size;
}

void recibir_mensaje_gameboy(int socket_cliente){
	t_packed * paquete = recibir_mensaje(socket_cliente);

	switch(paquete->cola_de_mensajes){
		case COLA_APPEARED_POKEMON:
			recibir_appeared_pokemon_desde_gameboy(paquete->mensaje);
			break;
		default:
			printf("Error\n");
	}

}

void crear_hilo_para_gameboy(int socket_servidor){
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)atender_nuevo_gameboy,(void*)socket_servidor);
}

void * interactuar_con_broker(){
	broker_socket = conectar_broker();
}

void crear_hilo_para_broker(){
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)interactuar_con_broker,NULL);
}

int main(){

	inicializar_logger();
	inicializar_archivo_de_configuracion();
	configurar_signals();
	inicializar_listas();//sacar los leaks
	definir_objetivo_global();//sacar los leaks
	localizar_entrenadores_en_mapa();

	/*CREO UN HILO POR ENTRENADOR*/
	/*Ya esta hecho en la funcion localizar_entrenadores_en_el_mapa(), cuando llama a la funcion agregar_entrenador()*/

	/*CREO UN HILO QUE SE DEDIQUE AL BROKER*/
	crear_hilo_para_broker();
	enviar_get();

	//Crea el socket servidor para recibir mensajes de gameboy
	int serv_socket = iniciar_servidor(PUERTO);

	while(1){
		/*CREO UN HILO QUE SE DEDIQUE A ESCUCHAR A GAMEBOY*/
		crear_hilo_para_gameboy(serv_socket);
	}

}
