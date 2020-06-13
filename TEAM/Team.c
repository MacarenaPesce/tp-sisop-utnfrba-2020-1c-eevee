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
	log_info(team_logger, "Agregue el pokemon al mapa\n");

	planificar();

	//free(pokemon);
}

void operar_con_localized_pokemon(t_localized_pokemon * mensaje){
	/*
	 * El proceso Team se suscribirá de manera global a esta cola de mensajes. Al recibir uno de los mismos deberá realizar los siguientes pasos:
		1. Verificar si ya recibió en algún momento un mensaje de la especie del Pokémon asociado al mensaje. Si es así, descarta el mensaje (ya sea Appeared o Localized).
		2. En caso de que nunca lo haya recibido, realiza las mismas operatorias que para APPEARED_POKEMON por cada coordenada del pokemon.
	 */

	char* especie = mensaje->pokemon;
	/*verificar aca si ya se recibio en algun momento un mensaje de esta especie de pokemon asociado al mensaje*/
	/*t_list* lista_coordenadas = mensaje->lista_coordenadas;
	for(int i = 0; i < list_size(lista_coordenadas); i++){
		t_pokemon * pokemon = malloc(sizeof(t_pokemon));
		t_coordenadas* coordenadas = list_get(lista_coordenadas, i);
		pokemon->especie = mensaje->pokemon;
		pokemon->posx = coordenadas->posx;
		pokemon->posy = coordenadas->posy;
		agregar_pokemon_a_mapa(pokemon);
		free(pokemon);
	}

	planificar();*/


}

void operar_con_caught_pokemon(t_caught_pokemon * mensaje){
	/*
	 * El proceso Team se suscribirá de manera global a esta cola de mensajes. Al recibir uno de los mismos deberá realizar los siguientes pasos:
		1. Validar si el id de mensaje correlativo del mensaje corresponde a uno pendiente de respuesta generado por la la instrucción CATCH_POKEMON antes descrita.
		Si no corresponde  ninguno, ignorar el mensaje.
		2. En caso que corresponda se deberá validar si el resultado del mensaje es afirmativo (se trapó el Pokémon).
		Si es así se debe asignar al entrenador bloqueado el Pokémon y habilitarlo a poder volver operar.
	 */
	t_entrenador* entrenador_bloqueado; //ver como recupero a este entrenador con lo del catch
	if(mensaje->status == OK){
		entrenador_bloqueado->cant_maxima_objetivos--;
		t_objetivo_entrenador* pokemon_objetivo_actual = entrenador_bloqueado->objetivo_actual;
		t_objetivo_entrenador* objetivo_encontrado = buscar_pokemon_por_especie(entrenador_bloqueado->pokemones, pokemon_objetivo_actual->especie);
		if(objetivo_encontrado == NULL){
			list_add(entrenador_bloqueado->pokemones, pokemon_objetivo_actual);
		} else {
			for(int i = 0; i < list_size(entrenador_bloqueado->pokemones); i++){
				t_objetivo_entrenador* pokemon = list_get(entrenador_bloqueado->pokemones, i);
				if(pokemon->especie == objetivo_encontrado->especie){
					objetivo_encontrado->cantidad++;
					/*No se de que forma puedo actualizar el campo cantidad de la lista de pokemones del entrenador, por eso uso list_replace*/
					list_replace_and_destroy_element(entrenador_bloqueado->pokemones, i, objetivo_encontrado, free);
				}
			}

		}
		entrenador_bloqueado->objetivo_actual = NULL;

	}
	/*if(entrenador_bloqueado->cant_maxima_objetivos == 0){//chequear si esta validacion se hace acá o en otro lado
		//tambien tengo que validad si ya agarro todos los que necesita. Por que si es asi el estado es finalizado
		//y tengo que ver que pasa con el hilo
		entrenador_bloqueado->razon_de_bloqueo == CANTIDAD_MAXIMA_ALCANZADA;
		if(cumplio_sus_objetivos(entrenador_bloqueado)){
			entrenador_bloqueado->estado == FINALIZANDO; //ver que onda el hilo
		} else {
			entrenador_bloqueado->estado = ESPERANDO_DEADLOCK;
		}
	} else {
		planificar();
	}*/


}

void agregar_pokemon_a_mapa(t_pokemon * pokemon){

	t_objetivo* un_objetivo;
	un_objetivo = buscar_pokemon_por_especie(lista_objetivos, pokemon->especie);
	if(un_objetivo == NULL){
		log_info(team_logger, "El objetivo es nulo");
	}
	if(un_objetivo->cantidad_necesitada > un_objetivo->cantidad_atrapada){
		list_add(lista_mapa, (void*)pokemon);

	}else{
		log_info(team_logger,"Ya tenemos la cantidad necesaria de la especie %s\n", pokemon->especie);
	}
}

t_objetivo * buscar_pokemon_por_especie(t_list* lista, char* especie){
	bool es_la_especie_buscada(t_objetivo* pokemon){
		return (string_equals_ignore_case(pokemon->especie, especie));
	}
	return (list_find(lista,(void*)es_la_especie_buscada));
}

t_entrenador * buscar_entrenador_por_id(t_list* lista, int id){
	bool es_el_buscado(t_entrenador* entrenador){
		return entrenador->id == id;
	}
	return (list_find(lista,(void*)es_el_buscado));
}

void mostrar_lo_que_hay_en_la_lista_de_objetivos(){
	int k = 0;
	t_objetivo * objetivo = malloc(sizeof(t_objetivo));
	while(!list_is_empty(lista_objetivos)){
		objetivo = list_get(lista_objetivos, k);
		if(objetivo == NULL){
			break;
		}
		log_info(team_logger,"Un objetivo es de la especie = %s, cantidad necesitada %i, cantidad atrapada %i", objetivo->especie, objetivo->cantidad_necesitada, objetivo->cantidad_atrapada);
		k++;
	}
	free(objetivo);
}

void definir_objetivo_global(){

	/*Leo del archivo de config y guardo los pokemon en la lista pokemones*/

	string_iterate_lines_with_list(objetivos_entrenadores, pokemones_ordenada, separar_pokemones_de_entrenador);

	/*Creo una nueva lista con los pokemon agrupados por especie*/

	list_sort(pokemones_ordenada, (void*)ordenar);
	log_info(team_logger,"Cargando el objetivo global...");
	cargar_objetivos(pokemones_ordenada, lista_objetivos);
	log_info(team_logger,"Objetivo global cargado\n");
	list_remove(lista_objetivos, list_size(lista_objetivos)-1);
	//mostrar_lo_que_hay_en_la_lista_de_objetivos();

	list_clean(pokemones_ordenada);


}

void mostrar_lo_que_hay_en_lista_entrenadores(){
	int l = 0;
	t_entrenador * entrenador;
	while(!list_is_empty(lista_entrenadores)){
		entrenador = list_get(lista_entrenadores, l);
		if(entrenador == NULL){
			break;
		}
		log_info(team_logger,"Un entrenador tiene id = %i, pos x = %i, y = %i, y puede atrapar %i pokemones\n", entrenador->id, entrenador->posx, entrenador->posy, entrenador->cant_maxima_objetivos);
		l++;
	}
	//free(entrenador);
}

void localizar_entrenadores_en_mapa(){

	lista_global_objetivos = list_create();
	lista_objetivos_de_entrenador = list_create();
	string_iterate_lines_with_list(objetivos_entrenadores,lista_global_objetivos, separar_listas_pokemones);

	lista_global_pokemones = list_create();
	lista_pokemones_de_entrenador = list_create();
	string_iterate_lines_with_list(pokemon_entrenadores,lista_global_pokemones, separar_listas_pokemones);

	string_iterate_lines_with_list(posiciones_entrenadores, pokemones_ordenada, separar_pokemones_de_entrenador);

	uint32_t pos_entrenador_en_lista_objetivos = 0;
	uint32_t posx;
	uint32_t posy;
	bool hay_que_agregar_entrenador = false;
	uint32_t id = 0;

	for(uint32_t i = 0; i < list_size(pokemones_ordenada); i++){
		if(!hay_que_agregar_entrenador){
			char* brokerenada_char;
			brokerenada_char = list_get(pokemones_ordenada, i);
			if(brokerenada_char != NULL) {
				int brokerenada = atoi(brokerenada_char);
				if(i == 0 || i % 2 == 0) {
					posx = brokerenada;
				}else{
					posy = brokerenada;
					hay_que_agregar_entrenador = true;
				}
				if(hay_que_agregar_entrenador){
					lista_pokemones_de_entrenador = obtener_pokemones(lista_global_pokemones, lista_pokemones_de_entrenador, pos_entrenador_en_lista_objetivos);
					lista_objetivos_de_entrenador = obtener_pokemones(lista_global_objetivos, lista_objetivos_de_entrenador, pos_entrenador_en_lista_objetivos);
					pos_entrenador_en_lista_objetivos++;
					agregar_entrenador(posx, posy, id, lista_pokemones_de_entrenador, lista_objetivos_de_entrenador);
					id++;
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
	log_info(team_logger,"Entrenadores ubicados\n");


}

t_list* obtener_pokemones(t_list* lista_global,t_list* lista, uint32_t posicion){

	char* pokemones_de_entrenador;
	lista_pokemones_objetivos = list_create();//este list_create es el culpable de la mayoria de los leaks
	pokemones_de_entrenador = list_get(lista_global, posicion);

	separar_pokemones_de_entrenador(pokemones_de_entrenador, lista);
	list_sort(lista, (void*)ordenar);
	cargar_objetivos(lista, lista_pokemones_objetivos);
	list_remove(lista_pokemones_objetivos, list_size(lista_pokemones_objetivos)-1);

	return lista_pokemones_objetivos;

}

uint32_t obtener_cantidad_maxima(t_list* lista){
	uint32_t contador = 0;
	t_objetivo_entrenador* un_objetivo;
	for(int i = 0; i < list_size(lista); i++){
		un_objetivo = list_get(lista, i);
		contador += un_objetivo->cantidad;
	}
	return contador;

}

void agregar_entrenador(uint32_t posx, uint32_t posy, uint32_t id, t_list* lista_pokemones_de_entrenador, t_list* lista_objetivos_de_entrenador){
	uint32_t cantidad_maxima = obtener_cantidad_maxima(lista_objetivos_de_entrenador);
	t_entrenador* entrenador = malloc(sizeof(t_entrenador));
	entrenador->posx = posx;
	entrenador->posy = posy;
	entrenador->id = id;
	entrenador->cant_maxima_objetivos = cantidad_maxima;
	entrenador->pokemones = lista_pokemones_de_entrenador;//chequear como se agrega esta lista tambien
	entrenador->objetivo = lista_objetivos_de_entrenador; //chequear como se agrega esta lista
	list_add(lista_entrenadores, (void*)entrenador);
	list_clean(lista_pokemones_de_entrenador);

	/*CREO UN HILO POR ENTRENADOR*/
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)jugar_con_el_entrenador,(void*)entrenador);
}

void * jugar_con_el_entrenador(t_entrenador * entrenador){

	sem_wait(&array_semaforos[entrenador->id]);
	log_info(team_logger, "Soy el entrenador que va a ejecutar, mi id es: %d.", entrenador->id);

	llegar_a_el_pokemon(entrenador);
	atrapar(entrenador);

}

void atrapar(t_entrenador * entrenador){
	/*
	 * Para esto, se deben ejecutar los siguientes pasos:
1. Enviar el mensaje a la cola de mensajes CATCH_POKEMON indicando cual es la especie del Pokémon y la posición del mismo.
2. Obtener el ID del mensaje anterior desde el Broker y guardarlo a la espera de la llegada de la respuesta en CAUGHT_POKEMON.
3. Bloquear al entrenador en cuestión a la espera del resultado del mensaje. Este entrenador no podrá volver a ejecutar hasta que se reciba el resultado.
En caso que el Broker no se encuentre funcionando o la conexión inicial falle, se deberá tomar como comportamiento Default que el Pokémon ha sido atrapado con éxito.
	 */

	t_catch_pokemon* catch_pokemon = malloc(sizeof(t_catch_pokemon));
	catch_pokemon->pokemon = entrenador->objetivo_actual->especie;
	catch_pokemon->coordenadas.posx = entrenador->objetivo_actual->posx;
	catch_pokemon->coordenadas.posy = entrenador->objetivo_actual->posy;

	int broker_socket = conectar_a_server(ip_broker, "6009");

	if(broker_socket < 0){
		//TODO
		/*
		 * Sacar el pokemon del mapa y moverlo a la lista de pokemones atrapados. Esa lista de pokemones atrapados tiene que ser comparada constantemente con el objetivo global.
		 * Si las listas coinciden, significa que llegamos al objetivo global. Tendriamos que poner un semaforo ahí, para avisar que ya se cumplio el objetivo global y terminar
		 * TEAM.
		 *
		 * QUE HACEMOS CON EL ENTRENADOR??
		 */

		log_info(team_logger, "El pokemon %s ha sido atrapado con exito", entrenador->objetivo_actual->especie);

	}else{
		enviar_catch_pokemon(broker_socket, -1, -1, catch_pokemon);
		log_info(team_logger, "Enviado pedido de catch pokemon para esta especie: %s", entrenador->objetivo_actual->especie);

		//Recibo ACK
		t_packed * paquete1 = recibir_mensaje(broker_socket);

		if(paquete1->operacion == ACK){
			log_info(team_logger, "Confirmada recepcion del pedido de catch para el pokemon: %s\n", entrenador->objetivo_actual->especie);
		}

		//TODO RECIBIR ID Y GUARDARLO
		//TODO BLOQUEAR AL ENTRENADOR CON SEMAFOROS Y CAMBIARLE EL ESTADO A BLOQUEADO
	}

	free(catch_pokemon);

}

void consumir_un_ciclo_de_cpu(){
	ciclos_de_cpu++;
	sleep(retardo_ciclo_cpu);
}

void llegar_a_el_pokemon(t_entrenador * entrenador){
	/*
	 * Cada movimiento en el mapa responderá a un ciclo de CPU, y este NO realizará movimientos diagonales para llegar a la posición deseada.
	 * Para simular más a la realidad esta funcionalidad, se deberá agregar un retardo de X segundos configurado por archivo de configuración.
	 */

	//Primero me muevo por izq
	while(entrenador->posx < entrenador->objetivo_actual->posx){
		consumir_un_ciclo_de_cpu();
		entrenador->posx = entrenador->posx + 1;
	}

	//Despues me muevo por derecha
	while(entrenador->posx > entrenador->objetivo_actual->posx){
		consumir_un_ciclo_de_cpu();
		entrenador->posx = entrenador->posx - 1;
	}

	//Despues me muevo por arriba
	while(entrenador->posy > entrenador->objetivo_actual->posy){
		consumir_un_ciclo_de_cpu();
		entrenador->posy = entrenador->posy - 1;
	}

	//Despues me muevo por abajo
	while(entrenador->posy < entrenador->objetivo_actual->posy){
		consumir_un_ciclo_de_cpu();
		entrenador->posy = entrenador->posy + 1;
	}

	if(entrenador->posy == entrenador->objetivo_actual->posy  && entrenador->posx == entrenador->objetivo_actual->posx){
		log_info(team_logger, "LLEGUE AL POKEMON");
	}
}

void seleccionar_el_entrenador_mas_cercano_al_pokemon(t_pokemon* pokemon){
	//Para poder planificar un entrenador, se seleccionará el hilo del entrenador más cercano al Pokémon.

	t_list* lista_aux;
	lista_aux = list_duplicate(lista_entrenadores);
	list_add_all(lista_aux, lista_bloqueados);
	t_pokemon* objetivo_actual;
	int i = 0;
	bool mas_cerca;
	t_entrenador* entrenador_mas_cercano;
	t_entrenador* otro_entrenador;
	entrenador_mas_cercano = list_get(lista_aux, i);
	int cantidad_entrenadores = list_size(lista_aux);
	while(i < cantidad_entrenadores){
		i++;
		if(i == cantidad_entrenadores){
			objetivo_actual = pokemon;
			entrenador_mas_cercano->objetivo_actual = objetivo_actual;
			list_add(lista_listos, (void*)entrenador_mas_cercano);
			sacar_entrenador_de_lista_pid(lista_entrenadores, entrenador_mas_cercano->id);
			break;
		}
		otro_entrenador = list_get(lista_aux, i);
		if(otro_entrenador == NULL){
			break;
		}
		mas_cerca = esta_mas_cerca(entrenador_mas_cercano, otro_entrenador, pokemon);
		if(!mas_cerca){
			entrenador_mas_cercano = otro_entrenador;
		}
	}

	list_destroy(lista_aux);
	log_info(team_logger,"El entrenador mas cercano es el de id %d y su objetivo es %s\n", entrenador_mas_cercano->id, entrenador_mas_cercano->objetivo_actual->especie);
	//free(entrenador_mas_cercano);
}

bool esta_mas_cerca(t_entrenador* entrenador1, t_entrenador* entrenador2, t_pokemon* pokemon){
	int distancia_entrenador1 = distancia_a_pokemon(entrenador1, pokemon);
	int distancia_entrenador2 = distancia_a_pokemon(entrenador2, pokemon);
	if(distancia_entrenador1 < distancia_entrenador2){
		return true;
	} else {
		return false;
	}
}

int cant_max_de_entrenadores(){
	return list_size(lista_entrenadores);
}

int distancia_a_pokemon(t_entrenador* entrenador, t_pokemon* pokemon){
	int distancia_X = entrenador->posx - pokemon->posx;
	int distancia_Y = entrenador->posy - pokemon->posy;
	int distancia_total = abs(distancia_X) + abs(distancia_Y);
	return distancia_total;
}

void planificar(){
	/* Cada entrenador al iniciar en el sistema entrará en estado New. ------- ESTO YA ESTA! CUANDO SE ORDENA LA LISTA DE ENTRENADORES */

	/*A medida que el Team empiece a recibir distintos Pokémon en el mapa despertará a los distintos entrenadores en estado New o en
	Blocked (que estén esperando para procesar) pasandolos a Ready. */

	/*Siempre se planificará aquel entrenador que se encuentre sin estar realizando ninguna operación activamente y, en caso de
existir más de uno, sea el que más cerca se encuentre del objetivo. A medida que cada entrenador se planifique (ya sea para moverse, intercambiar o atrapar un
Pokémon) entrarán en estado exec. En el contexto de nuestro trabajo practico no contemplaremos el multiprocesamiento, esto implica que solo UN entrenador
podrá estar en estado Exec en determinado tiempo. Cuando un entrenador en estado Exec finalice su recorrido y su ejecución planificada entrará en un estado bloqueados.
Este estado implica que el entrenador no tiene más tareas para realizar momentáneamente. Cuando un entrenador en estado Exec cumpla todos sus objetivos, pasará a estado Exit.
Cuando todos los entrenadores dentro de un Team se encuentren en Exit, se considera que el proceso Team cumplió el objetivo global.
	 */


	t_pokemon* pokemon;
	pokemon = list_get(lista_mapa, 0);/*esto funciona solo para appeared.
	Para localized voy a tener que buscar al mas cercano para cada pokemon de la especie que aparezca.
	en un ciclo*/

	if(pokemon == NULL){
		log_info(team_logger,"Esperando que aparezcan pokemones en el mapa");
	}

	seleccionar_el_entrenador_mas_cercano_al_pokemon(pokemon);
	log_info(team_logger,"Entrenador mas cercano seleccionado");

	obtener_proximo_ejecucion();
}

int main(){

	inicializar_logger();
	inicializar_archivo_de_configuracion();
	inicializar_semaforos();
	configurar_signals();
	inicializar_listas();//sacar los leaks
	definir_objetivo_global();//sacar los leaks
	localizar_entrenadores_en_mapa();

	//Crea el socket servidor para recibir mensajes de gameboy
	int serv_socket = iniciar_servidor(PUERTO);

	//Crea el socket cliente para conectarse al broker
	enviar_get();

	//convertirse_en_suscriptor_global_del_broker();

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
