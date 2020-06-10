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
		2. En caso de que nunca lo haya recibido, realiza las mismas operatorias que para APPEARED_POKEMON por cada brokerenada del pokemon.
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
	log_info(team_logger,"La coordenada Y es: %d\n", mensaje->coordenadas.posy);

	//operar_con_appeared_pokemon(mensaje);

	//free(mensaje);
}

void mostrar_lo_que_hay_en_la_lista_de_objetivos(){
	int k = 0;
	t_objetivo * objetivo = malloc(sizeof(t_objetivo));
	while(!list_is_empty(lista_objetivos)){
		objetivo = list_get(lista_objetivos, k);
		if(objetivo == NULL){
			break;
		}
		log_info(team_logger,"Un objetivo es de la especie = %s, cantidad %i", objetivo->especie, objetivo->cantidad);
		k++;
	}
	free(objetivo);
}

void definir_objetivo_global(){

	/*Leo del archivo de config y guardo los pokemon en la lista pokemones*/

	lista_config = list_create();
	pokemones_ordenada = list_create();
	string_iterate_lines(objetivos_entrenadores, _imprimir);

	/*Creo una nueva lista con los pokemon agrupados por especie*/

	pokemones_ordenada = list_sorted(lista_config, (void*)ordenar);
	char* unPokemon;
	unPokemon = list_get(pokemones_ordenada, 0);
	uint32_t contador = 0;

	log_info(team_logger,"Cargando el objetivo global...");
	cargar_objetivos(pokemones_ordenada, lista_objetivos);
	log_info(team_logger,"Objetivo global cargado\n");
	list_remove(lista_objetivos, list_size(lista_objetivos)-1);
	//mostrar_lo_que_hay_en_la_lista_de_objetivos();
	list_clean(lista_config);
}


void cargar_objetivos(t_list* pokemones_ordenada, t_list* lista){
	char* unPokemon;
		unPokemon = list_get(pokemones_ordenada, 0);
		uint32_t contador = 0;
		printf("El tamanio de pokemones ordenada es %i", list_size(pokemones_ordenada));
		char* un_char = "Ultimo poke\0";
		char* ultimo_poke = string_from_format("%s\0", un_char);
		list_add(pokemones_ordenada, ultimo_poke);

		/*Empiezo a cargar a lista de objetivos, con tipo y cantidad de cada uno*/

		char* especiePokemon;
		char* otroPokemon;
		int i = 0;
		while(pokemones_ordenada != NULL){
			especiePokemon = unPokemon;
			otroPokemon = list_get(pokemones_ordenada, i);
			if(otroPokemon == NULL){
				//agregar_objetivo("Este pokemon es de mentira", 0, lista);
				agregar_objetivo(especiePokemon, contador, lista);

				break;
			}
			if(string_equals_ignore_case(unPokemon,otroPokemon)){
				contador++;
				i++;
			}else{
				agregar_objetivo(especiePokemon, contador, lista);
				unPokemon = otroPokemon;
				contador = 1;
				i++;
			}
		}
		//mostrar_lo_que_hay_en_la_lista_de_objetivos();
		free(unPokemon);
}

void agregar_objetivo(char* especie, uint32_t cantidad, t_list* lista){
	t_objetivo* objetivo = malloc(sizeof(t_objetivo));
	objetivo->especie = especie;
	objetivo->cantidad = cantidad;
	list_add(lista, (void*)objetivo);

}


void mostrar_lo_que_hay_en_lista_entrenadores(){
	int l = 0;
	t_entrenador * entrenador;
	while(!list_is_empty(lista_entrenadores)){
		entrenador = list_get(lista_entrenadores, l);
		if(entrenador == NULL){
			break;
		}

		log_info(team_logger,"Un entrenador tiene id = %i, pos x = %i, y = %i", entrenador->id, entrenador->posx, entrenador->posy);
		l++;
	}
	free(entrenador);
}

void localizar_entrenadores_en_mapa(){

	lista_global_objetivos = list_create();
	lista_objetivos_de_entrenador = list_create();
	string_iterate_lines(objetivos_entrenadores, separar_listas_objetivos);

	lista_global_pokemones = list_create();
	lista_pokemones_de_entrenador = list_create();
	string_iterate_lines(pokemon_entrenadores, separar_listas_pokemones);

	string_iterate_lines(posiciones_entrenadores, _imprimir);

	uint32_t i = 0;
	uint32_t pos_entrenador_en_lista_objetivos = 0;
	t_entrenador * entrenador = malloc(sizeof(t_entrenador));
	uint32_t posx;
	uint32_t posy;
	bool hay_que_agregar_entrenador = false;
	for(uint32_t i = 0; i < list_size(lista_config); i++){
		if(!hay_que_agregar_entrenador){
			char* brokerenada_char;
			brokerenada_char = list_get(lista_config, i);
			if(brokerenada_char != NULL) {
					int brokerenada = atoi(brokerenada_char);
					if(i == 0 || i % 2 == 0) {
						posx = brokerenada;
					}else{
						posy = brokerenada;
						hay_que_agregar_entrenador = true;
					}
					if(hay_que_agregar_entrenador){
						lista_objetivos_de_entrenador = obtener_objetivos_de_entrenador(lista_global_objetivos, pos_entrenador_en_lista_objetivos);
						lista_pokemones_de_entrenador = obtener_pokemones_de_entrenador(lista_global_pokemones, pos_entrenador_en_lista_objetivos);
						pos_entrenador_en_lista_objetivos++;
						agregar_entrenador(posx, posy, i, lista_objetivos_de_entrenador, lista_pokemones_de_entrenador);
						hay_que_agregar_entrenador = false;
						list_clean(lista_objetivos_de_entrenador);
						list_clean(lista_pokemones_de_entrenador);
					}
			}else{
				break;
			}
		}
	}

	//mostrar_lo_que_hay_en_lista_entrenadores();
	free(entrenador);
	list_destroy_and_destroy_elements(lista_objetivos_de_entrenador, (void*)free);
	list_destroy_and_destroy_elements(lista_config, (void*)free);
	log_info(team_logger,"Entrenadores ubicados\n");

}

t_list* obtener_objetivos_de_entrenador(t_list* lista_global_objetivos, uint32_t posicion){
	char* objetivos_de_entrenador;
	objetivos_de_entrenador = list_get(lista_global_objetivos, posicion);
	separar_pokemones_de_objetivo(objetivos_de_entrenador);
	for(uint32_t i = 0; i < list_size(lista_objetivos_de_entrenador); i++){
		char* objetivo;
		objetivo = list_get(lista_objetivos_de_entrenador, i);
		if(objetivo == NULL){
			printf("El pokemon de la lista de objetivos de entrenador es nulo\n");
			break;
		}

		//log_info(team_logger, "Un pokemon que necesita el entrenador %i es: %s \n", posicion, objetivo);
		free(objetivo);
	}
	return lista_objetivos_de_entrenador;
}

t_list* obtener_pokemones_de_entrenador(t_list* lista_global_de_pokemones, uint32_t posicion){
	char* pokemones_de_entrenador;
	pokemones_de_entrenador = list_get(lista_global_de_pokemones, posicion);
	separar_pokemones_de_entrenador(pokemones_de_entrenador);
	for(uint32_t i = 0; i < list_size(lista_pokemones_de_entrenador); i++){
			char* pokemon;
			pokemon = list_get(lista_pokemones_de_entrenador, i);
			if(pokemon == NULL){
				printf("El pokemon de la lista de pokemones de entrenador es nulo\n");
				break;
			}

			//log_info(team_logger, "Un pokemon que tiene el entrenador %i es: %s \n", posicion, pokemon);
			free(pokemon);
	}
	//free(pokemones_de_entrenador);
	return lista_pokemones_de_entrenador;
}

void agregar_entrenador(uint32_t posx, uint32_t posy, uint32_t id, t_list* lista_objetivos_de_entrenador, t_list* lista_pokemones_de_entrenador){
	t_entrenador* entrenador = malloc(sizeof(t_entrenador));
	entrenador->posx = posx;
	entrenador->posy = posy;
	entrenador->id = id;
	entrenador->pokemones = lista_pokemones_de_entrenador;//chequear como se agrega esta lista tambien
	entrenador->objetivo = lista_objetivos_de_entrenador; //chequear como se agrega esta lista
	list_add(lista_entrenadores, (void*)entrenador);
	/*CREO UN HILO POR ENTRENADOR*/
	//crear_hilo_entrenador(entrenador, jugar_con_el_entrenador(entrenador));
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
	int broker_socket = conectar_a_server(ip_broker, "6009");
	if(broker_socket < 0){// || broker_socket == 0){
		int h = 0;
		t_objetivo* objetivo = malloc(sizeof(t_objetivo));
		while(lista_objetivos != NULL){
			objetivo = list_get(lista_objetivos, h);
			if(objetivo == NULL){
				break;
			}
			log_info(team_logger, "No existen locaciones para esta especie: %s, cant %i", objetivo->especie, objetivo->cantidad);
			h++;
			free(objetivo);
		}
		//free(objetivo);
		//close(broker_socket);
	}else{
		enviar_mensaje_por_cada_pokemon_requerido(broker_socket);
		close(broker_socket);
	}
}

void enviar_mensaje_por_cada_pokemon_requerido(int broker_socket){

	int h = 0;
	t_objetivo * objetivo;
	while(lista_objetivos != NULL){
		objetivo = list_get(lista_objetivos, h);
		if(objetivo == NULL){
			break;
		}

		t_get_pokemon* get_pokemon = malloc(sizeof(t_get_pokemon));
		get_pokemon->pokemon = objetivo->especie;
		enviar_get_pokemon(broker_socket, -1, -1, get_pokemon);
		log_info(team_logger, "Enviado pedido de get pokemon para esta especie: %s", objetivo->especie);
		free(get_pokemon);
		h++;

		//Recibo ACK
		t_packed * paquete1 = recibir_mensaje(broker_socket);

		if(paquete1->operacion == ACK){
			log_info(team_logger, "Confirmada recepcion del pedido get para el pokemon: %s\n", objetivo->especie);
		}

		//TODO GUARDAR EL ID
	}

}

void convertirse_en_suscriptor_global_del_broker(){

	enviar_mensaje_de_suscripcion_a_cola_appeared_pokemon();
	//enviar_mensaje_de_suscripcion_a_cola_caught_pokemon();
	//enviar_mensaje_de_suscripcion_a_cola_localized_pokemon();

}

void * reconexion(){
	sleep(tiempo_reconexion);
}

void hacer_intento_de_reconexion(){
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)reconexion,NULL);
	pthread_detach(hilo);
}

void enviar_mensaje_de_suscripcion_a_cola_appeared_pokemon(){
	int broker_socket = conectar_a_server(ip_broker, "6009");

	t_suscripcion * suscripcion = malloc(sizeof(t_suscripcion));
	suscripcion->minutos_suscripcion = -1;
	suscripcion->tipo_suscripcion = SUSCRIPCION_GLOBAL;

	while(broker_socket > 0){
		enviar_solicitud_suscripcion(broker_socket,COLA_APPEARED_POKEMON,suscripcion);
		log_info(team_logger, "Solicitud de suscripcion a COLA_APPEARED_POKEMON enviada al broker");

		//Recibo ACK
		t_packed * paquete1 = recibir_mensaje(broker_socket);

		if(paquete1->operacion == ACK){
			log_info(team_logger, "Confirmada recepcion del mensaje de suscripcion a la cola_appeaded_pokemon\n");
		}

		//Quedo a la espera de recibir notificaciones
		t_packed * paquete2 = recibir_mensaje(broker_socket);
		if(paquete2->operacion == ENVIAR_MENSAJE){
			if(paquete2->cola_de_mensajes){
				operar_con_appeared_pokemon(paquete2->mensaje);
			}
		}
	}

	free(suscripcion);
	/*if(broker_socket <= 0){
		hacer_intento_de_reconexion();
		enviar_mensaje_de_suscripcion_a_cola_appeared_pokemon();
	}*/
	log_info(team_logger, "NO se pudo enviar la solicitud de suscripcion a COLA_APPEARED_POKEMON al broker");

}

void * escuchar_mensajes_entrantes(int new_client_sock){

	log_info(team_logger, "Gameboy conectado, esperando mensajes...");

	t_packed * paquete = recibir_mensaje(new_client_sock);

	if(paquete == 0) {
	   log_info(team_logger, "\nDesconectado \n");
	}

	if(paquete->cola_de_mensajes == COLA_APPEARED_POKEMON){
		recibir_appeared_pokemon_desde_gameboy(paquete->mensaje);
	}

		close(new_client_sock);
		free(paquete);
}

void crear_hilo_de_escucha_para_gameboy(int socket, void*funcion_a_ejecutar(int)){
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)funcion_a_ejecutar,(void*)socket);
	pthread_detach(hilo);
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

	//Crea el socket servidor para recibir mensajes de gameboy
	int serv_socket = iniciar_servidor(PUERTO);

	//Crea el socket cliente para conectarse al broker
	enviar_get();

	convertirse_en_suscriptor_global_del_broker();

	while(GLOBAL_SEGUIR){
		struct sockaddr_in client_addr;

		//Setea la direccion en 0
		memset(&client_addr, 0, sizeof(client_addr));
		socklen_t client_len = sizeof(client_addr);

		//Acepta la nueva conexion
		int new_client_sock = accept(serv_socket, (struct sockaddr *)&client_addr, &client_len);

		log_info(team_logger, "Se aceptó un nuevo gameboy");

		crear_hilo_de_escucha_para_gameboy(new_client_sock, escuchar_mensajes_entrantes);
	}

	close(serv_socket);

	terminar_team_correctamente();
	return 0;
}
