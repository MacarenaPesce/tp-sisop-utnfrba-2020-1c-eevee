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
	sem_post(&orden_para_planificar);

}

void operar_con_caught_pokemon(uint32_t status, uint32_t id_correlativo){
	/*El proceso Team se suscribirá de manera global a esta cola de mensajes. Al recibir uno de los mismos deberá realizar los siguientes pasos:
	1. Validar si el id de mensaje correlativo del mensaje corresponde a uno pendiente de respuesta generado por la la instrucción CATCH_POKEMON antes descrita.
	Si no corresponde  ninguno, ignorar el mensaje.
	2. En caso que corresponda se deberá validar si el resultado del mensaje es afirmativo (se atrapó el Pokémon).
	Si es así se debe asignar al entrenador bloqueado el Pokémon y habilitarlo a poder volver operar.
	 */

	t_mensaje_guardado_catch* mensaje_guardado_catch = buscar_mensaje_por_id_catch(id_correlativo, mensajes_para_chequear_id_catch);

	if(mensaje_guardado_catch == NULL){
		return;
	}

	t_catch_pokemon* catch_pokemon = mensaje_guardado_catch->contenido;

	if(catch_pokemon->pokemon != NULL){
		if(status == OK){
			sem_wait(&podes_sacar_entrenador);
			t_entrenador * entrenador = buscar_entrenador_por_objetivo_actual(catch_pokemon);
			entrenador->objetivo_actual = NULL;			
			sacar_entrenador_de_lista_pid(lista_bloqueados_esperando_caught, entrenador->id);
			log_info(team_logger, "El pokemon %s ha sido atrapado con exito por el entrenador %d", catch_pokemon->pokemon, entrenador->id);

			actualizar_mapa_y_entrenador(catch_pokemon, entrenador);
			sem_post(&mapa_y_entrenador);
		}
	}
}

void agregar_pokemon_a_mapa(t_pokemon * pokemon){

	t_objetivo* un_objetivo = buscar_pokemon_por_especie(lista_objetivos, pokemon->especie);

	if(un_objetivo == NULL){
		log_info(team_logger, "No necesito este pokemon: %s", pokemon->especie);
	}else{
		pthread_mutex_lock(&pokemones_asignados);
		pokemon->asignado = false;
		pthread_mutex_unlock(&pokemones_asignados);

		pthread_mutex_lock(&mapa_mutex);
		list_add(lista_mapa, (void*)pokemon);
		pthread_mutex_unlock(&mapa_mutex);

		log_info(team_logger, "Agregue el pokemon %s con coordenadas (%d, %d) al mapa\n", pokemon->especie, pokemon->posx, pokemon->posy);
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

	sem_post(&objetivos_listos);

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

					pthread_mutex_lock(&tocando_pokemones_objetivos);
					lista_objetivos_de_entrenador = obtener_pokemones(lista_global_objetivos, lista_objetivos_de_entrenador, pos_entrenador_en_lista_objetivos);
					pthread_mutex_unlock(&tocando_pokemones_objetivos);

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

	log_info(team_logger,"Entrenadores ubicados\n");
	list_destroy_and_destroy_elements(lista_pokemones_de_entrenador, (void*)destruir_objetivo_entrenador);
	list_destroy_and_destroy_elements(lista_objetivos_de_entrenador, (void*)destruir_objetivo_entrenador);

}

void agregar_entrenador(uint32_t posx, uint32_t posy, uint32_t id, t_list* lista_pokemones_de_entrenador, t_list* lista_objetivos_de_entrenador){
	sacar_de_objetivos_pokemones_atrapados(lista_objetivos_de_entrenador, lista_pokemones_de_entrenador);

	t_entrenador* entrenador = malloc(sizeof(t_entrenador));
	entrenador->posx = posx;
	entrenador->posy = posy;
	entrenador->id = id;
	entrenador->ejec_anterior = 0;
	entrenador->estimacion_actual = estimacion_inicial;
	entrenador->estimacion_anterior = estimacion_inicial;
	entrenador->estimacion_real = estimacion_inicial;
	entrenador->instruccion_actual = 0;
	entrenador->estado = NUEVO;
	entrenador->desalojado = false;
	entrenador->agoto_quantum = false;
	entrenador->quantum_restante = quantum;
	entrenador->ciclos_de_cpu = 0;
	entrenador->espera_asignada = false;
	entrenador->razon_de_bloqueo = NINGUNA;

	t_pokemon* un_pokemon;
	entrenador->pokemones = list_create();

	for(int i = 0; i < list_size(lista_pokemones_de_entrenador); i++){
		un_pokemon = list_get(lista_pokemones_de_entrenador, i);
		list_add(entrenador->pokemones, (void*)un_pokemon);
	}

	entrenador->objetivo = list_create();
	t_objetivo_entrenador * una_meta;
	for(int i = 0; i < list_size(lista_objetivos_de_entrenador); i++){
		una_meta = list_get(lista_objetivos_de_entrenador, i);
		list_add(entrenador->objetivo, (void*)una_meta);
	}

	uint32_t cantidad_maxima = obtener_cantidad_maxima(entrenador->objetivo);
	entrenador->cant_maxima_objetivos = cantidad_maxima;


	list_add(lista_entrenadores, (void*)entrenador);
	list_clean(lista_pokemones_de_entrenador);

	/*CREO UN HILO POR ENTRENADOR*/
	pthread_t entre;
	pthread_create(&entre,NULL,(void*)jugar_con_el_entrenador,(void*)entrenador);
	pthread_detach(entre);
}

void ver_razon_de_bloqueo(t_entrenador * entrenador){
	if(entrenador->cant_maxima_objetivos == 0){
		entrenador->razon_de_bloqueo = CANTIDAD_MAXIMA_ALCANZADA;
	}else{
		entrenador->razon_de_bloqueo = ESPERANDO_POKEMON;
	}
}

void actualizar_mapa_y_entrenador(t_catch_pokemon* catch_pokemon, t_entrenador* entrenador){
	pthread_mutex_lock(&mapa_mutex);
	int max = list_size(lista_mapa);
	pthread_mutex_unlock(&mapa_mutex);


	//SETEARLE AL ENTRENADOR UNA NUEVA POSICION Y ELIMINAMOS EL POKEMON DEL MAPA
	//PORQUE YA NO ESTA DISPONIBLE
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

			pthread_mutex_lock(&mapa_mutex);
			list_remove(lista_mapa, i); //elimino el poke del mapa
			//remover al pokemon de la lista de asignados
			pthread_mutex_unlock(&mapa_mutex);
			break;
		}
	}

	//REVISAMOS SI DEBE BLOQUEARSE Y ACTUALIZAMOS EL OBJETIVO GLOBAL
	if(objetivo_personal_cumplido(entrenador)){
		t_objetivo* pokemon_encontrado = buscar_pokemon_por_especie(lista_objetivos, catch_pokemon->pokemon);
		pokemon_encontrado->cantidad_atrapada++; /*Busco la especie en la lista global y sumo uno a los atrapados*/

	} else {
		ver_razon_de_bloqueo(entrenador);
		bloquear_entrenador(entrenador);
	}

	free(catch_pokemon);
}

void chequear_si_fue_cumplido_el_objetivo_global(){
	if(objetivo_global_cumplido()){
		for(int i=0; i < MAXIMO_ENTRENADORES; i++){
			sem_wait(&todos_los_entrenadores_finalizaron);
		}

		printf("************************************************************************************************************");
		printf("\n");
		log_info(team_logger, "Objetivo global cumplido!!!!! :D");
		printf("************************************************************************************************************");
		printf("\n");

		log_info(team_logger,"La cantidad de ciclos de CPU totales es: %i", ciclos_de_cpu);
		log_info(team_logger_oficial,"La cantidad de ciclos de CPU totales es: %i", ciclos_de_cpu);
		log_info(team_logger,"La cantidad de cambios de contextos realizados es: %i", cambios_de_contexto);
		log_info(team_logger_oficial,"La cantidad de cambios de contextos realizados es: %i", cambios_de_contexto);
		for (int i = 0; i < MAXIMO_ENTRENADORES; i++){
			t_entrenador* entrenador = list_get(lista_finalizar, i);
			log_info(team_logger, "La cantidad de ciclos de CPU realizados por el entrenador %i es: %i", entrenador->id, entrenador->ciclos_de_cpu);
			log_info(team_logger_oficial, "La cantidad de ciclos de CPU realizados por el entrenador %i es: %i", entrenador->id, entrenador->ciclos_de_cpu);
		}
		log_info(team_logger, "La cantidad de deadlocks producidos es: %i", deadlocks_producidos);
		log_info(team_logger_oficial, "La cantidad de deadlocks producidos es: %i", deadlocks_producidos);
		log_info(team_logger, "La cantidad de deadlocks resueltos es: %i", deadlocks_resueltos);
		log_info(team_logger_oficial, "La cantidad de deadlocks resueltos es: %i", deadlocks_resueltos);

		terminar_team_correctamente();
	}
}

void hacer_procedimiento_para_atrapar_default(t_catch_pokemon* catch_pokemon, t_entrenador * entrenador){

	log_info(team_logger_oficial, "Falló la conexión con Broker; inicia la operación default");
	log_debug(team_logger, "El pokemon %s ha sido atrapado con exito por el entrenador %d", catch_pokemon->pokemon, entrenador->id);
	log_info(team_logger_oficial, "Se ha atrapado el pokemon %s en la posicion %d %d", catch_pokemon->pokemon, catch_pokemon->coordenadas.posx, catch_pokemon->coordenadas.posy);
	
	actualizar_mapa_y_entrenador(catch_pokemon, entrenador);
	sem_post(&mapa_y_entrenador);

}

void hacer_procedimiento_para_atrapar_pokemon_con_broker(t_entrenador * entrenador){
	entrenador->razon_de_bloqueo = ESPERANDO_MENSAJE_CAUGHT;
	bloquear_entrenador(entrenador);
	consumir_un_ciclo_de_cpu_mientras_planificamos(entrenador);
}

bool hay_pokemones_en_el_mapa(){
	return (list_size(lista_mapa) > 0);
}

void chequeo_si_puedo_atrapar_otro(){
	if(hay_pokemones_en_el_mapa()){
		for(int i=0; i<list_size(lista_mapa);i++){
			pthread_mutex_lock(&mapa_mutex);
			t_pokemon * pokemon = list_get(lista_mapa, i);
			pthread_mutex_unlock(&mapa_mutex);
			if(!esta_en_lista_asignados(pokemon)){
				seleccionar_el_entrenador_mas_cercano_al_pokemon(pokemon);
				break;
			}
		}
	}
}

bool esta_en_lista_asignados(t_pokemon * pokemon){
	pokemon = buscar_pokemon_por_especie_y_ubicacion(lista_asignados, pokemon);
	//Si esta en la lista de asignados, da verdadero
	return pokemon != NULL;
}

void bloquear_entrenador(t_entrenador* entrenador){
	entrenador_en_ejecucion = NULL;
	cambios_de_contexto++;
	switch(entrenador->razon_de_bloqueo){
		case ESPERANDO_POKEMON:
			//log_info(team_logger,"La estimacion de este entrenador al bloquearse es %f", entrenador->estimacion_real);

			pthread_mutex_lock(&bloqueados_esperando_mutex);
			list_add(lista_bloqueados_esperando, (void*)entrenador);
			pthread_mutex_unlock(&bloqueados_esperando_mutex);

			log_info(team_logger_oficial, "El entrenador %d esta bloqueado esperando pokemones", entrenador->id);
			log_info(team_logger, "El entrenador %d esta bloqueado esperando que aparezcan pokemones", entrenador->id);
			//mostrar_lo_que_hay_en_la_lista_de_objetivos_del_entrenador(entrenador->objetivo);
			chequeo_si_puedo_atrapar_otro();
			sem_post(&orden_para_planificar);

			break;

		case ESPERANDO_MENSAJE_CAUGHT:

			list_add(lista_bloqueados_esperando_caught, (void*)entrenador);
			log_info(team_logger_oficial, "El entrenador %d esta bloqueado esperando que llegue mensaje Caught", entrenador->id);
			log_info(team_logger, "El entrenador %d esta bloqueado esperando que llegue mensaje caught\n", entrenador->id);

			sem_post(&podes_sacar_entrenador);
			break;

		case CANTIDAD_MAXIMA_ALCANZADA:
			entrenador->quantum_restante = quantum;
			pthread_mutex_lock(&lista_bloq_max_mutex);
			list_add(lista_bloqueados_cant_max_alcanzada, (void*)entrenador);
			pthread_mutex_unlock(&lista_bloq_max_mutex);

			pthread_mutex_lock(&bloqueados_esperando_mutex);
			sacar_entrenador_de_lista_pid(lista_bloqueados_esperando, entrenador->id);
			pthread_mutex_unlock(&bloqueados_esperando_mutex);

			log_info(team_logger,"El entrenador %d está bloqueado por haber alcanzado la cantidad máxima de pokemones que podía atrapar\n", entrenador->id);
			log_info(team_logger_oficial,"El entrenador %d está bloqueado por haber alcanzado la cantidad máxima de pokemones que podía atrapar", entrenador->id);

			sem_post(&me_bloquee);
			sem_post(&orden_para_planificar);

			break;
		default:
			break;
	}
}

void consumir_un_ciclo_de_cpu(t_entrenador* entrenador){
	if((!strcmp(algoritmo_planificacion, "FIFO"))){
		ciclos_de_cpu++;
		entrenador->ciclos_de_cpu++;
		sleep(retardo_ciclo_cpu);
		log_info(team_logger, "El entrenador %d ejecutó 1 ciclo de cpu", entrenador->id);
	}

	if((!strcmp(algoritmo_planificacion, "SJF-SD"))){
		ciclos_de_cpu++;
		entrenador->ciclos_de_cpu++;
		sleep(retardo_ciclo_cpu);
		log_info(team_logger, "El entrenador %d ejecutó 1 ciclo de cpu", entrenador->id);

		entrenador->instruccion_actual++;
		entrenador->estimacion_actual--;
		entrenador->ejec_anterior = 0;
	}

	if(!strcmp(algoritmo_planificacion, "SJF-CD")){
		ciclos_de_cpu++;
		entrenador->ciclos_de_cpu++;
		sleep(retardo_ciclo_cpu);
		log_info(team_logger, "El entrenador %d ejecutó 1 ciclo de cpu", entrenador->id);
		entrenador_en_ejecucion->instruccion_actual++;
		entrenador_en_ejecucion->estimacion_actual--;
		log_info(team_logger, "Mi estimacion actual es %f", entrenador_en_ejecucion->estimacion_actual);
		entrenador_en_ejecucion->ejec_anterior = 0;

		if(desalojo_en_ejecucion){
			entrenador_en_ejecucion = NULL;
			pthread_mutex_lock(&lista_listos_mutex);
			list_add(lista_listos, entrenador);
			pthread_mutex_unlock(&lista_listos_mutex);
			cambios_de_contexto++;
			log_info(team_logger, "El entrenador de id %d fue desalojado y paso a Ready\n", entrenador->id);
			log_info(team_logger_oficial, "El entrenador de id %d fue desalojado y paso a Ready\n", entrenador->id);

			sem_post(&orden_para_planificar);
			log_info(team_logger_oficial, "El entrenador %d pasó a ejecutar", entrenador->id);
		}
	}

	if(!strcmp(algoritmo_planificacion, "RR")){
		ciclos_de_cpu++;

		pthread_mutex_lock(&mutex_ciclos_cpu_entrenador);
		entrenador->ciclos_de_cpu++;
		pthread_mutex_unlock(&mutex_ciclos_cpu_entrenador);

		sleep(retardo_ciclo_cpu);

		if(entrenador->quantum_restante > 1){
			log_info(team_logger, "El entrenador %d ejecutó 1 ciclo de cpu", entrenador->id);
			entrenador->quantum_restante--;
		}else{
			log_info(team_logger, "El entrenador %d ejecutó 1 ciclo de cpu", entrenador->id);
			entrenador_en_ejecucion = NULL;
			entrenador->agoto_quantum = true;
			entrenador->quantum_restante = quantum;
			cambios_de_contexto++;
			
				pthread_mutex_lock(&lista_listos_mutex);
				list_add(lista_listos, entrenador);
				pthread_mutex_unlock(&lista_listos_mutex);
				//cambios_de_contexto++;
				log_info(team_logger, "El entrenador de id %d fue desalojado y paso a Ready\n", entrenador->id);
				log_info(team_logger_oficial, "El entrenador de id %d fue desalojado y paso a Ready\n", entrenador->id);
				sem_post(&orden_para_planificar);
				log_info(team_logger_oficial, "El entrenador %d pasó a ejecutar", entrenador->id);
		}
	}
}

void consumir_un_ciclo_de_cpu_mientras_planificamos(t_entrenador * entrenador){
	if((!strcmp(algoritmo_planificacion, "FIFO"))){
		ciclos_de_cpu++;
		entrenador->ciclos_de_cpu++;
		sleep(retardo_ciclo_cpu);
	}

	if((!strcmp(algoritmo_planificacion, "SJF-SD"))){
		ciclos_de_cpu++;
		entrenador->ciclos_de_cpu++;
		sleep(retardo_ciclo_cpu);
		log_info(team_logger, "El entrenador %d ejecutó 1 ciclo de cpu", entrenador->id);

		entrenador->instruccion_actual++;
		entrenador->estimacion_actual--;
		entrenador->ejec_anterior = 0;
	}

	if(!strcmp(algoritmo_planificacion, "SJF-CD")){
		ciclos_de_cpu++;
		entrenador->ciclos_de_cpu++;
		sleep(retardo_ciclo_cpu);
		log_info(team_logger, "El entrenador %d ejecutó 1 ciclo de cpu", entrenador->id);
		entrenador_en_ejecucion->instruccion_actual++;
		entrenador_en_ejecucion->estimacion_actual--;
		log_info(team_logger, "Mi estimacion actual es %f", entrenador_en_ejecucion->estimacion_actual);
		entrenador_en_ejecucion->ejec_anterior = 0;

		if(desalojo_en_ejecucion){
			entrenador_en_ejecucion = NULL;
			pthread_mutex_lock(&lista_listos_mutex);
			list_add(lista_listos, entrenador);
			pthread_mutex_unlock(&lista_listos_mutex);
			cambios_de_contexto++;
			log_info(team_logger, "El entrenador de id %d fue desalojado y paso a Ready\n", entrenador->id);
			log_info(team_logger_oficial, "El entrenador de id %d fue desalojado y paso a Ready\n", entrenador->id);

			sem_post(&orden_para_planificar);
			sem_wait(&array_semaforos[entrenador->id]);
		}
	}

	if(!strcmp(algoritmo_planificacion, "RR")){
		ciclos_de_cpu++;
		entrenador->ciclos_de_cpu++;
		sleep(retardo_ciclo_cpu);

		if(entrenador->quantum_restante > 1){
			log_info(team_logger, "El entrenador %d ejecutó 1 ciclo de cpu", entrenador->id);
			entrenador->quantum_restante--;
		}else{
			log_info(team_logger, "El entrenador %d ejecutó 1 ciclo de cpu", entrenador->id);
			entrenador_en_ejecucion = NULL;
			entrenador->agoto_quantum = true;
			entrenador->quantum_restante = quantum;
			cambios_de_contexto++;
			
			if(entrenador->cant_maxima_objetivos == 0 || (entrenador->razon_de_bloqueo != NINGUNA)){
				log_info(team_logger, "El entrenador de id %d fue desalojado\n", entrenador->id);
				sem_post(&orden_para_planificar);

			}else{
				pthread_mutex_lock(&lista_listos_mutex);
				list_add(lista_listos, entrenador);
				pthread_mutex_unlock(&lista_listos_mutex);
				//cambios_de_contexto++;
				log_info(team_logger, "El entrenador de id %d fue desalojado y paso a Ready\n", entrenador->id);
				log_info(team_logger_oficial, "El entrenador de id %d fue desalojado y paso a Ready\n", entrenador->id);
				sem_post(&orden_para_planificar);
				sem_wait(&array_semaforos[entrenador->id]);
			}
		}
	}
}

void crear_hilo_para_tratamiento_de_mensajes(){
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)tratamiento_de_mensajes, NULL);
	pthread_detach(hilo);
}

bool chequear_si_recibi_appeared_de_especie_antes(char * pokemon){
	char* especie = buscar_mensaje_appeared_por_especie(pokemon, lista_historico_appeared_pokemon);
	return especie != NULL;
}

bool fijarme_si_debo_atraparlo_usando_el_objetivo_global(char * pokemon){
	t_objetivo * objetivo = buscar_pokemon_por_especie(lista_objetivos, pokemon);
		return objetivo != NULL && objetivo->cantidad_necesitada > objetivo->cantidad_atrapada;
}

void * tratamiento_de_mensajes(){
	while(1){
		pthread_mutex_lock(&global_seguir_mutex);
		if(GLOBAL_SEGUIR == 0){
			break;
		}
		pthread_mutex_unlock(&global_seguir_mutex);
		
		sem_wait(&mensaje_nuevo_disponible);

		t_packed * paquete;

		while(1){
			pthread_mutex_lock(&mensaje_nuevo_mutex);
			paquete = list_remove(mensajes_que_llegan_nuevos, 0);
			pthread_mutex_unlock(&mensaje_nuevo_mutex);

			switch (paquete->cola_de_mensajes){
				case COLA_APPEARED_POKEMON:
					tratar_appeared_pokemon(paquete);
					break;
				case COLA_LOCALIZED_POKEMON:
					tratar_localized_pokemon(paquete);
					break;
				case COLA_CAUGHT_POKEMON:
					tratar_caught_pokemon(paquete);
					break;
				default:
					break;
			}
			break;
		}
		chequear_si_fue_cumplido_el_objetivo_global();
		sem_post(&paquete_usado);
	}
	return NULL;
}

void tratar_appeared_pokemon(t_packed* paquete){
	t_appeared_pokemon * appeared = paquete->mensaje;

	agregar_a_historico_appeared(appeared->pokemon);

	t_pokemon * pokemon = malloc(sizeof(t_pokemon));
	pokemon->posx = appeared->coordenadas.posx;
	pokemon->posy = appeared->coordenadas.posy;

	pokemon->especie = (char*)malloc(strlen(appeared->pokemon)+1);
	memcpy(pokemon->especie,appeared->pokemon,strlen(appeared->pokemon)+1);

	t_objetivo* un_objetivo = buscar_pokemon_por_especie(lista_objetivos, pokemon->especie);

	if(un_objetivo == NULL){
		log_info(team_logger, "No necesito este pokemon: %s", pokemon->especie);
		destruir_pokemon(pokemon);
		return;
	}

	seleccionar_el_entrenador_mas_cercano_al_pokemon(pokemon);
	operar_con_appeared_pokemon(pokemon);

	return;
}

void tratar_localized_pokemon(t_packed* paquete){
	t_localized_pokemon * localized = paquete->mensaje;

	/* Hice previamente un pedido de get_pokemon */
	pthread_mutex_lock(&mensaje_chequear_id_mutex);
	uint32_t * id_mensaje_buscado = buscar_mensaje_por_id(paquete->id_correlacional, mensajes_para_chequear_id);
	pthread_mutex_unlock(&mensaje_chequear_id_mutex);
	
	if(id_mensaje_buscado == NULL){
		return;
	}

	/* No recibí anteriormente un appeared de esta especie */
	bool encontre_pokemon_en_historico = chequear_si_recibi_appeared_de_especie_antes(localized->pokemon);

	if(encontre_pokemon_en_historico){
		return;
	}

	void trabajar_con_localized(void* _coordenada){
		t_coordenadas* coodenada = (t_coordenadas*) _coordenada;
		//Por cada elemento de la lista de coordenadas agrego un pokemon
		t_pokemon * pokemon = malloc(sizeof(t_pokemon));
		pokemon->posx = coodenada->posx;
		pokemon->posy = coodenada->posy;

		pokemon->especie = (char*)malloc(strlen(localized->pokemon)+1);
		memcpy(pokemon->especie,localized->pokemon,strlen(localized->pokemon)+1);

		seleccionar_el_entrenador_mas_cercano_al_pokemon(pokemon);
		operar_con_appeared_pokemon(pokemon);
	}

	list_iterate(localized->lista_coordenadas,trabajar_con_localized);

	return;

}

void tratar_caught_pokemon(t_packed* paquete){
	t_caught_pokemon * caught = paquete->mensaje;
	operar_con_caught_pokemon(caught->status, paquete->id_correlacional);

	return;
}

int main(int arcg, char** argv[]){

	inicializar_logger();
	inicializar_archivo_de_configuracion(argv[1]);
	inicializar_semaforos();
	configurar_signals();
	inicializar_listas();

	definir_objetivo_global();
	localizar_entrenadores_en_mapa();
	sem_post(&entrenadores_ubicados);

	//Crea el socket servidor para recibir mensajes de gameboy
	int serv_socket = iniciar_servidor(puerto_team);

	//Crea el socket cliente para conectarse al broker
	enviar_get();

	crear_hilo_para_tratamiento_de_mensajes();
	crear_hilo_para_planificar();
	crear_hilo_para_deadlock();
	convertirse_en_suscriptor_global_del_broker();
	crear_hilo_de_escucha_para_gameboy(serv_socket);

	close(serv_socket);
	return 0;
}
