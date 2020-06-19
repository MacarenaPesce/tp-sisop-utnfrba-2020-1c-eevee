/*
 * Team.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Team.h"

void operar_con_appeared_pokemon(t_pokemon * pokemon){
	/*Este mensaje permitirá la inclusión en el proceso Team de un nuevo Pokémon en el mapa. Al llegar este mensaje, el proceso Team deberá verificar si requiere
	 * atrapar el mismo controlando los Pokemon globales necesarios y los ya atrapados. No se debe poder atrapar mas Pokemon de una especie de los requeridos globalmente.
	 * En caso que se requiera el mismo, se debe agregar a la lista de Pokémon requeridos y en el momento que un entrenador se encuentre en estado “Dormido” o “Libre”
	 * debe planificarlo para ir a atraparlo. En este mensaje se recibirán los siguientes parámetros: Especie de Pokemon y Posición del Pokemon.*/

	agregar_pokemon_a_mapa(pokemon);
	log_info(team_logger, "Agregue el pokemon al mapa\n");
	sem_post(&hay_un_pokemon_nuevo);
}

void operar_con_localized_pokemon(t_localized_pokemon * mensaje){
	/*
	 * El proceso Team se suscribirá de manera global a esta cola de mensajes. Al recibir uno de los mismos deberá realizar los siguientes pasos:
		1. Verificar si ya recibió en algún momento un mensaje de la especie del Pokémon asociado al mensaje. Si es así, descarta el mensaje (ya sea Appeared o Localized).
		2. En caso de que nunca lo haya recibido, realiza las mismas operatorias que para APPEARED_POKEMON por cada coordenada del pokemon.
	 */

	//char* especie = mensaje->pokemon;
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

void operar_con_caught_pokemon(t_caught_pokemon * mensaje, uint32_t id){
	/*
	 * El proceso Team se suscribirá de manera global a esta cola de mensajes. Al recibir uno de los mismos deberá realizar los siguientes pasos:
		1. Validar si el id de mensaje correlativo del mensaje corresponde a uno pendiente de respuesta generado por la la instrucción CATCH_POKEMON antes descrita.
		Si no corresponde  ninguno, ignorar el mensaje.
		2. En caso que corresponda se deberá validar si el resultado del mensaje es afirmativo (se trapó el Pokémon).
		Si es así se debe asignar al entrenador bloqueado el Pokémon y habilitarlo a poder volver operar.
	 */

	t_mensaje_guardado* mensaje_guardado = buscar_mensaje(id);

	if(mensaje_guardado != NULL){
		if(mensaje->status == OK){
			t_catch_pokemon* catch_pokemon = malloc(sizeof(t_catch_pokemon));
			catch_pokemon->pokemon = mensaje_guardado->pokemon.especie;
			catch_pokemon->coordenadas.posx = mensaje_guardado->pokemon.posx;
			catch_pokemon->coordenadas.posy = mensaje_guardado->pokemon.posy;
			t_entrenador* entrenador = buscar_entrenador_por_objetivo_actual(catch_pokemon);

			actualizar_mapa_y_entrenador(catch_pokemon, entrenador);

			if(objetivo_personal_cumplido(entrenador)){
				entrenador->estado = FINALIZANDO;
				list_add(lista_finalizar, entrenador);
				t_objetivo* pokemon_encontrado;
				pokemon_encontrado = buscar_pokemon_por_especie(lista_objetivos, catch_pokemon->pokemon);
				pokemon_encontrado->cantidad_atrapada++;/*Busco la especie en la lista global y sumo uno a los atrapados*/

				chequear_si_fue_cumplido_el_objetivo_global();

			} else {
				bloquear_entrenador(entrenador);
				if(entrenador->razon_de_bloqueo == ESPERANDO_POKEMON){
					planificar();
				}
			}
			free(catch_pokemon);
			//chequear deadlock
		} else {
			log_info(team_logger,"No me importa este mensaje");
		}
	}
}

void agregar_pokemon_a_mapa(t_pokemon * pokemon){

	t_objetivo* un_objetivo;
	un_objetivo = buscar_pokemon_por_especie(lista_objetivos, pokemon->especie);

	if(un_objetivo == NULL){
		log_info(team_logger, "No necesito este pokemon");
	}

	if(un_objetivo->cantidad_necesitada > un_objetivo->cantidad_atrapada){
		pthread_mutex_lock(&mapa_mutex);
		list_add(lista_mapa, (void*)pokemon);
		pthread_mutex_unlock(&mapa_mutex);
	}else{
		log_info(team_logger,"Ya tenemos la cantidad necesaria de la especie %s\n", pokemon->especie);
	}
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
			char* coordenada_char;
			coordenada_char = list_get(pokemones_ordenada, i);
			if(coordenada_char != NULL) {
				int coordenada = atoi(coordenada_char);
				if(i == 0 || i % 2 == 0) {
					posx = coordenada;
				}else{
					posy = coordenada;
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

void agregar_entrenador(uint32_t posx, uint32_t posy, uint32_t id, t_list* lista_pokemones_de_entrenador, t_list* lista_objetivos_de_entrenador){
	sacar_de_objetivos_pokemones_atrapados(lista_objetivos_de_entrenador, lista_pokemones_de_entrenador);
	uint32_t cantidad_maxima = obtener_cantidad_maxima(lista_objetivos_de_entrenador);

	t_entrenador* entrenador = malloc(sizeof(t_entrenador));
	entrenador->posx = posx;
	entrenador->posy = posy;
	entrenador->id = id;
	entrenador->cant_maxima_objetivos = cantidad_maxima;

	t_pokemon* un_pokemon = malloc(sizeof(t_pokemon));
	entrenador->pokemones = list_create();
	for(int i = 0; i < list_size(lista_pokemones_de_entrenador); i++){
		un_pokemon = list_get(lista_pokemones_de_entrenador, i);
		//log_info(team_logger,"POKEMON NUMERO %d, ESPECIE %s POSX %d POSY %d\n", i, un_pokemon->especie, un_pokemon->posx, un_pokemon->posy);
		list_add(entrenador->pokemones, (void*)un_pokemon);
	}

	entrenador->objetivo = list_create();
	t_objetivo_entrenador * una_meta = malloc(sizeof(t_objetivo_entrenador));
	for(int i = 0; i < list_size(lista_objetivos_de_entrenador); i++){
		una_meta = list_get(lista_objetivos_de_entrenador, i);
		list_add(entrenador->objetivo, (void*)una_meta);
	}

	list_add(lista_entrenadores, (void*)entrenador);
	list_clean(lista_pokemones_de_entrenador);

	/*CREO UN HILO POR ENTRENADOR*/
	pthread_t entre;
	pthread_create(&entre,NULL,(void*)jugar_con_el_entrenador,(void*)entrenador);
}

void * jugar_con_el_entrenador(t_entrenador * entrenador){


	sem_wait(&array_semaforos[entrenador->id]);
	log_info(team_logger, "Soy el entrenador que va a ejecutar, mi id es: %d.", entrenador->id);

	llegar_a_el_pokemon(entrenador);
	atrapar(entrenador);

	pthread_mutex_lock(&mutex_deadlock);
	bool deadlock = hayDeadlock;
	pthread_mutex_unlock(&mutex_deadlock);

	if(deadlock){
		sem_wait(&semaforos_deadlock[entrenador->id]);

		log_info(team_logger, "Soy el entrenador que va a ejecutar para deadlock, mi id es: %d.", entrenador->id);
		mover_entrenador_a_otra_posicion(entrenador);
	}





	return NULL;
}

void actualizar_mapa_y_entrenador(t_catch_pokemon* catch_pokemon, t_entrenador* entrenador){
	pthread_mutex_lock(&mapa_mutex);
	int max = list_size(lista_mapa);
	pthread_mutex_unlock(&mapa_mutex);

	for(int i = 0; i < max; i++){
		t_pokemon* pokemon;

		pthread_mutex_lock(&mapa_mutex);
		pokemon = list_get(lista_mapa, i);
		pthread_mutex_unlock(&mapa_mutex);

		if(pokemon->especie == catch_pokemon->pokemon //busco al pokemon a atrapar en el mapa
			&& pokemon->posx == catch_pokemon->coordenadas.posx
			&& pokemon->posy == catch_pokemon->coordenadas.posy){
			entrenador->cant_maxima_objetivos--; //actualizo la cantidad maxima de objetivos a atrapar del entrenador
			if(pokemon->especie == NULL){
				printf("La especie es nula");
			}
			if(entrenador->objetivo == NULL){
				printf("La lista es nula");
			}
			t_objetivo_entrenador* pokemon_objetivo = (t_objetivo_entrenador*)buscar_pokemon_por_especie(entrenador->objetivo, pokemon->especie);
			if(pokemon_objetivo != NULL){
				pokemon_objetivo->cantidad--; //si es una especie que tenia como objetivo personal, disminuyo su cantidad
			}
			t_objetivo_entrenador* pokemon_encontrado = (t_objetivo_entrenador*)buscar_pokemon_por_especie(entrenador->pokemones, pokemon->especie);
			if(pokemon_encontrado == NULL){
				agregar_objetivo(pokemon->especie, 1, entrenador->pokemones); //agrego el pokemon a la lista de atrapados por el entrenador
			} else {
				pokemon_encontrado->cantidad++; // o actualizo la cantidad de esa especie de atrapados del entrenador
			}
			entrenador->objetivo_actual = NULL;// no se si esto es necesario*/

			pthread_mutex_lock(&mapa_mutex);
			list_remove(lista_mapa, i); //elimino el poke del mapa
			pthread_mutex_unlock(&mapa_mutex);
			break;
		}
	}
}

void chequear_si_fue_cumplido_el_objetivo_global(){

	if(objetivo_global_cumplido()){
		log_info(team_logger, "Objetivo global cumplido");
		terminar_team_correctamente();
	} else {
		log_info(team_logger, "Aun no se cumplio el objetivo global");
		planificar();
	}
}

void hacer_procedimiento_para_atrapar_default(t_catch_pokemon* catch_pokemon, t_entrenador * entrenador){
	log_info(team_logger_oficial, "Falló la conexión con Broker; inicia la operación default");
	log_info(team_logger, "El pokemon %s ha sido atrapado con exito", catch_pokemon->pokemon);
	log_info(team_logger_oficial, "Se ha atrapado el pokemon %s en la posicion %i %i", catch_pokemon->pokemon, catch_pokemon->coordenadas.posx, catch_pokemon->coordenadas.posy);
	actualizar_mapa_y_entrenador(catch_pokemon, entrenador);

	if(objetivo_personal_cumplido(entrenador)){
		entrenador->estado = FINALIZANDO;
		list_add(lista_finalizar, entrenador);//ver que onda el hilo
		log_info(team_logger, "Finalizo el entrenador %i", entrenador->id);
		t_objetivo* pokemon_encontrado = buscar_pokemon_por_especie(lista_objetivos, catch_pokemon->pokemon);
		pokemon_encontrado->cantidad_atrapada++; /*Busco la especie en la lista global y sumo uno a los atrapados*/

		chequear_si_fue_cumplido_el_objetivo_global();


	} else {

		bloquear_entrenador(entrenador);
		planificar();
		if(entrenador->razon_de_bloqueo == ESPERANDO_POKEMON){
			planificar();
		}
	}
}

void hacer_procedimiento_para_atrapar_pokemon_con_broker(t_packed * ack, t_entrenador * entrenador){

	//TODO RECIBIR ID Y GUARDARLO
	//TODO BLOQUEAR AL ENTRENADOR CON SEMAFOROS Y CAMBIARLE EL ESTADO A BLOQUEADO
}

void atrapar(t_entrenador * entrenador){
	/*Para esto, se deben ejecutar los siguientes pasos:
1. Enviar el mensaje a la cola de mensajes CATCH_POKEMON indicando cual es la especie del Pokémon y la posición del mismo.
2. Obtener el ID del mensaje anterior desde el Broker y guardarlo a la espera de la llegada de la respuesta en CAUGHT_POKEMON.
3. Bloquear al entrenador en cuestión a la espera del resultado del mensaje. Este entrenador no podrá volver a ejecutar hasta que se reciba el resultado.
En caso que el Broker no se encuentre funcionando o la conexión inicial falle, se deberá tomar como comportamiento Default que el Pokémon ha sido atrapado con éxito.*/

	t_catch_pokemon* catch_pokemon = malloc(sizeof(t_catch_pokemon));
	catch_pokemon->pokemon = entrenador->objetivo_actual->especie;
	catch_pokemon->coordenadas.posx = entrenador->objetivo_actual->posx;
	catch_pokemon->coordenadas.posy = entrenador->objetivo_actual->posy;

	t_servidor * servidor = malloc(sizeof(t_servidor));
	servidor->ip = ip_broker;
	servidor->puerto = "6009";
	servidor->id_cliente = id;

	t_packed * ack = enviar_catch_pokemon(servidor, -1, catch_pokemon);
	log_info(team_logger, "Enviado pedido de catch pokemon para esta especie: %s", entrenador->objetivo_actual->especie);

	if(ack == (t_packed*) -1){
		hacer_procedimiento_para_atrapar_default(catch_pokemon, entrenador);
	}else{
		//Recibo ACK
		if(ack->operacion == ACK){
			log_info(team_logger, "Confirmada recepcion del pedido CATCH para el pokemon: %s\n", catch_pokemon->pokemon);
			log_info(team_logger, "EL ID DEL MENSAJE ES: %d\n", ack->id_mensaje);
			hacer_procedimiento_para_atrapar_pokemon_con_broker(ack, entrenador);
		}
	}

	free(catch_pokemon);


}

bool todos_bloqueados_por_cantidad_maxima(){
	return list_size(lista_bloqueados_esperando) == 0 && list_size(lista_bloqueados_cant_max_alcanzada) > 0;
}

void mostrar_lo_que_hay_en_la_lista_de_objetivos_del_entrenador(t_list * lista){
	int k = 0;
	t_objetivo_entrenador * objetivo = malloc(sizeof(t_objetivo));
	while(!list_is_empty(lista)){
		objetivo = list_get(lista, k);
		if(objetivo == NULL){
			break;
		}
		log_info(team_logger,"Un objetivo es de la especie %s, cantidad necesitada %d", objetivo->especie, objetivo->cantidad);
		k++;
	}
	free(objetivo);
}

void bloquear_entrenador(t_entrenador* entrenador){
	if(entrenador->cant_maxima_objetivos == 0) {

		entrenador->razon_de_bloqueo = CANTIDAD_MAXIMA_ALCANZADA;
		list_add(lista_bloqueados_cant_max_alcanzada, (void*)entrenador);

		log_info(team_logger,"El entrenador %i está bloqueado por haber alcanzado la cantidad máxima de pokemones que podía atrapar", entrenador->id);
		log_info(team_logger_oficial,"El entrenador %i está bloqueado por haber alcanzado la cantidad máxima de pokemones que podía atrapar", entrenador->id);
		chequear_deadlock();

	}else{

		entrenador->razon_de_bloqueo = ESPERANDO_POKEMON;
		list_add(lista_bloqueados_esperando, (void*)entrenador);

		//log_info(team_logger, "El entrenador %d esta bloqueado esperando que aparezcan los siguientes pokemones:", entrenador->id);
		//mostrar_lo_que_hay_en_la_lista_de_objetivos_del_entrenador(entrenador->objetivo);

		//log_info(team_logger_oficial, "El entrenador %i esta bloqueado esperando que aparezca un pokemon", entrenador->id);

	}
}

void consumir_un_ciclo_de_cpu(){
	ciclos_de_cpu++;
	sleep(retardo_ciclo_cpu);
}

void llegar_a_el_pokemon(t_entrenador * entrenador){
	/*Cada movimiento en el mapa responderá a un ciclo de CPU, y este NO realizará movimientos diagonales para llegar a la posición deseada.
	 * Para simular más a la realidad esta funcionalidad, se deberá agregar un retardo de X segundos configurado por archivo de configuración.*/
	log_info(team_logger_oficial, "El entrenador %i se mueve a atrapar a %s a la posicion %i %i", entrenador->id, entrenador->objetivo_actual->especie,entrenador->objetivo_actual->posx, entrenador->objetivo_actual->posy);
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
		log_info(team_logger, "El entrenador de id %d llegó al pokemon %s.", entrenador->id, entrenador->objetivo_actual->especie);

	}
}

void * planificar(){
	/*Cada entrenador al iniciar en el sistema entrará en estado New. ------- ESTO YA ESTA! CUANDO SE ORDENA LA LISTA DE ENTRENADORES */

	/*A medida que el Team empiece a recibir distintos Pokémon en el mapa despertará a los distintos entrenadores en estado New o en
	Blocked (que estén esperando para procesar) pasandolos a Ready. */

	/*Siempre se planificará aquel entrenador que se encuentre sin estar realizando ninguna operación activamente y, en caso de
existir más de uno, sea el que más cerca se encuentre del objetivo. A medida que cada entrenador se planifique (ya sea para moverse, intercambiar o atrapar un
Pokémon) entrarán en estado exec. En el contexto de nuestro trabajo practico no contemplaremos el multiprocesamiento, esto implica que solo UN entrenador
podrá estar en estado Exec en determinado tiempo. Cuando un entrenador en estado Exec finalice su recorrido y su ejecución planificada entrará en un estado bloqueados.
Este estado implica que el entrenador no tiene más tareas para realizar momentáneamente. Cuando un entrenador en estado Exec cumpla todos sus objetivos, pasará a estado Exit.
Cuando todos los entrenadores dentro de un Team se encuentren en Exit, se considera que el proceso Team cumplió el objetivo global. */

	sem_wait(&hay_un_pokemon_nuevo);

	pthread_mutex_lock(&mapa_mutex);
	t_pokemon * pokemon = list_get(lista_mapa, 0);
	pthread_mutex_unlock(&mapa_mutex);

	seleccionar_el_entrenador_mas_cercano_al_pokemon(pokemon);
	obtener_proximo_ejecucion();

	return NULL;
}

void crear_hilo_para_planificar(){
	sem_wait(&entrenadores_ubicados);
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)planificar, NULL);
}

int main(){

	inicializar_logger();
	inicializar_archivo_de_configuracion();
	inicializar_semaforos();
	configurar_signals();
	inicializar_listas();//sacar los leaks
	definir_objetivo_global();//sacar los leaks
	localizar_entrenadores_en_mapa();
	sem_post(&entrenadores_ubicados);

	//Crea el socket servidor para recibir mensajes de gameboy
	int serv_socket = iniciar_servidor(PUERTO);

	//Crea el socket cliente para conectarse al broker
	enviar_get();

	//convertirse_en_suscriptor_global_del_broker();
	crear_hilo_para_deadlock();
	crear_hilo_para_planificar();
	crear_hilo_de_escucha_para_gameboy(serv_socket);


	close(serv_socket);

	terminar_team_correctamente();
	return 0;
}
