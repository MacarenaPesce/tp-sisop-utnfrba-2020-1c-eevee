/*
 * Team.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Team.h"

void operar_con_appeared_pokemon(t_appeared_pokemon * paquete){
	/*Este mensaje permitirá la inclusión en el proceso Team de un nuevo Pokémon en el mapa. Al llegar este mensaje, el proceso Team deberá verificar si requiere
	 * atrapar el mismo controlando los Pokemon globales necesarios y los ya atrapados. No se debe poder atrapar mas Pokemon de una especie de los requeridos globalmente.
	 * En caso que se requiera el mismo, se debe agregar a la lista de Pokémon requeridos y en el momento que un entrenador se encuentre en estado “Dormido” o “Libre”
	 * debe planificarlo para ir a atraparlo. En este mensaje se recibirán los siguientes parámetros: Especie de Pokemon y Posición del Pokemon.*/

	agregar_pokemon_a_mapa(paquete->pokemon, paquete->coordenadas);

	log_info(team_logger, "Agregue el pokemon %s con coordenadas (%d, %d) al mapa\n", paquete->pokemon, paquete->coordenadas.posx, paquete->coordenadas.posy);

	sem_post(&orden_para_planificar);

	free(paquete);
}

void operar_con_caught_pokemon(uint32_t status, uint32_t id_correlativo){
	/*El proceso Team se suscribirá de manera global a esta cola de mensajes. Al recibir uno de los mismos deberá realizar los siguientes pasos:
	1. Validar si el id de mensaje correlativo del mensaje corresponde a uno pendiente de respuesta generado por la la instrucción CATCH_POKEMON antes descrita.
	Si no corresponde  ninguno, ignorar el mensaje.
	2. En caso que corresponda se deberá validar si el resultado del mensaje es afirmativo (se trapó el Pokémon).
	Si es así se debe asignar al entrenador bloqueado el Pokémon y habilitarlo a poder volver operar.
	 */

	t_mensaje_guardado* mensaje_guardado_catch = buscar_mensaje_por_id(id_correlativo, mensajes_para_chequear_id);

	t_catch_pokemon* catch_pokemon = malloc(sizeof(t_catch_pokemon));
	catch_pokemon = mensaje_guardado_catch->contenido;

	if(mensaje_guardado_catch != NULL){
		if(status == OK){
			t_entrenador * entrenador = malloc(sizeof(t_entrenador));
			entrenador = buscar_entrenador_por_objetivo_actual(catch_pokemon);

			sacar_entrenador_de_lista_pid(lista_bloqueados_esperando_caught, entrenador->id);

			actualizar_mapa_y_entrenador(catch_pokemon, entrenador);
		}
	}
}

void agregar_pokemon_a_mapa(char * especie, t_coordenadas coordenadas){

	t_objetivo* un_objetivo = buscar_pokemon_por_especie(lista_objetivos, especie);

	if(un_objetivo == NULL){
		log_info(team_logger, "No necesito este pokemon");
	}

	if(un_objetivo->cantidad_necesitada > un_objetivo->cantidad_atrapada){

		t_pokemon * pokemon = malloc(sizeof(t_pokemon));
		pokemon->especie = especie;
		pokemon->posx = coordenadas.posx;
		pokemon->posy = coordenadas.posy;

		pthread_mutex_lock(&mapa_mutex);
		list_add(lista_mapa, (void*)pokemon);
		pthread_mutex_unlock(&mapa_mutex);

	}else{
		log_info(team_logger,"Ya tenemos la cantidad necesaria de la especie %s\n", especie);
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
	entrenador->ejec_anterior = 0;
	entrenador->estimacion_actual = estimacion_inicial;
	entrenador->estimacion_anterior = estimacion_inicial;
	entrenador->estimacion_real = estimacion_inicial;
	entrenador->instruccion_actual = 0;
	entrenador->estado = NUEVO;
	entrenador->desalojado = false;

	t_pokemon* un_pokemon = malloc(sizeof(t_pokemon));
	entrenador->pokemones = list_create();

	for(int i = 0; i < list_size(lista_pokemones_de_entrenador); i++){
		un_pokemon = list_get(lista_pokemones_de_entrenador, i);
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
			entrenador->objetivo_actual = NULL;// no se si esto es necesario*/

			pthread_mutex_lock(&mapa_mutex);
			list_remove(lista_mapa, i); //elimino el poke del mapa
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
}

void chequear_si_fue_cumplido_el_objetivo_global(){
	if(objetivo_global_cumplido()){
		log_info(team_logger, "Objetivo global cumplido");
		terminar_team_correctamente();
	}
}

void hacer_procedimiento_para_atrapar_default(t_catch_pokemon* catch_pokemon, t_entrenador * entrenador){

	log_info(team_logger_oficial, "Falló la conexión con Broker; inicia la operación default");
	log_info(team_logger, "El pokemon %s ha sido atrapado con exito", catch_pokemon->pokemon);
	log_info(team_logger_oficial, "Se ha atrapado el pokemon %s en la posicion %i %i", catch_pokemon->pokemon, catch_pokemon->coordenadas.posx, catch_pokemon->coordenadas.posy);

	actualizar_mapa_y_entrenador(catch_pokemon, entrenador);
}

void hacer_procedimiento_para_atrapar_pokemon_con_broker(t_entrenador * entrenador){
	entrenador->razon_de_bloqueo = ESPERANDO_MENSAJE_CAUGHT;
	bloquear_entrenador(entrenador);
}

void bloquear_entrenador(t_entrenador* entrenador){

	switch(entrenador->razon_de_bloqueo){
		case ESPERANDO_POKEMON:

			list_add(lista_bloqueados_esperando, (void*)entrenador);
			log_info(team_logger_oficial, "El entrenador %d esta bloqueado esperando pokemones", entrenador->id);
			log_info(team_logger, "El entrenador %d esta bloqueado esperando que aparezcan los siguientes pokemones:", entrenador->id);
			mostrar_lo_que_hay_en_la_lista_de_objetivos_del_entrenador(entrenador->objetivo);
			break;

		case ESPERANDO_MENSAJE_CAUGHT:

			list_add(lista_bloqueados_esperando_caught, (void*)entrenador);
			log_info(team_logger_oficial, "El entrenador %d esta bloqueado esperando que llegue mensaje Caught", entrenador->id);
			log_info(team_logger, "El entrenador %d esta bloqueado esperando que llegue mensaje caught");
			break;

		case CANTIDAD_MAXIMA_ALCANZADA:

			pthread_mutex_lock(&lista_bloq_max_mutex);
			list_add(lista_bloqueados_cant_max_alcanzada, (void*)entrenador);
			pthread_mutex_unlock(&lista_bloq_max_mutex);

			sacar_entrenador_de_lista_pid(lista_bloqueados_esperando, entrenador->id);

			log_info(team_logger,"El entrenador %d está bloqueado por haber alcanzado la cantidad máxima de pokemones que podía atrapar", entrenador->id);
			log_info(team_logger_oficial,"El entrenador %d está bloqueado por haber alcanzado la cantidad máxima de pokemones que podía atrapar", entrenador->id);

			break;
		default:
			break;
	}
}

void consumir_un_ciclo_de_cpu(){
	ciclos_de_cpu++;
	sleep(retardo_ciclo_cpu);
	if(!strcmp(algoritmo_planificacion, "SJF-CD")){
		if(desalojo_en_ejecucion){
			confirmar_desalojo_en_ejecucion();
			me_desalojaron = true;
		}
	}
}

void confirmar_desalojo_en_ejecucion(void){
	if(entrenador_por_desalojar!=NULL) {
		entrenador_en_ejecucion->estado = LISTO;
		entrenador_en_ejecucion = NULL;
		entrenador_por_desalojar = NULL;
		desalojo_en_ejecucion = false;
	}
}

void crear_hilo_para_tratamiento_de_mensajes(){
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)tratamiento_de_mensajes, NULL);
}

bool chequear_si_recibi_appeared_de_especie_antes(char * pokemon){
	t_mensaje_guardado * mensaje = buscar_mensaje_appeared_por_especie(pokemon, mensajes_para_chequear_id);
	return mensaje != NULL;
}

bool fijarme_si_debo_atraparlo_usando_el_objetivo_global(char * pokemon){
	t_objetivo * objetivo = buscar_pokemon_por_especie(lista_objetivos, pokemon);
		return objetivo != NULL && objetivo->cantidad_necesitada > objetivo->cantidad_atrapada;
}

void * tratamiento_de_mensajes(){

	while(GLOBAL_SEGUIR){
		sem_wait(&mensaje_nuevo_disponible);

		t_mensaje_guardado * mensaje;

		pthread_mutex_lock(&mensaje_nuevo_mutex);
		mensaje = list_remove(mensajes_que_llegan_nuevos, 0);
		pthread_mutex_unlock(&mensaje_nuevo_mutex);

		if(mensaje->operacion == APPEARED){
			t_appeared_pokemon * contenido = mensaje->contenido;

			if(fijarme_si_debo_atraparlo_usando_el_objetivo_global(contenido->pokemon)){
				t_pokemon * pokemon = malloc(sizeof(t_pokemon));
				pokemon->especie = contenido->pokemon;
				pokemon->posx = contenido->coordenadas.posx;
				pokemon->posy = contenido->coordenadas.posy;

				seleccionar_el_entrenador_mas_cercano_al_pokemon(pokemon);
				operar_con_appeared_pokemon(mensaje->contenido);
			} else {
				log_info(team_logger, "No necesito a este pokemon");
			}
		}

		if(mensaje->operacion == LOCALIZED){
			t_localized_pokemon * contenido = mensaje->contenido;
			t_mensaje_guardado * mensaje_buscado;
			mensaje_buscado = buscar_mensaje_por_id(mensaje->id_correlacional, mensajes_para_chequear_id);

			if(mensaje_buscado != NULL){
				if(!chequear_si_recibi_appeared_de_especie_antes(((t_localized_pokemon *)(mensaje->contenido))->pokemon)){
					t_objetivo * objetivo = buscar_pokemon_por_especie(lista_objetivos, ((t_localized_pokemon *)(mensaje->contenido))->pokemon);
					int contador = 0;
					for(int i = 0; i < contenido->cantidad_coordenadas; i++){
						t_coordenadas * coord = malloc(sizeof(t_coordenadas));
						coord = list_get(contenido->lista_coordenadas, i);
						 //con esto me fijo de no operar con mas pokemones de esta especie de los que necesito
						if(objetivo->cantidad_necesitada > contador){

							//por cada elemento de la lista de coordenadas agrego un pokemon
							t_pokemon * pokemon = malloc(sizeof(t_pokemon));
							pokemon->especie = ((t_localized_pokemon *)(mensaje->contenido))->pokemon;
							pokemon->posx = coord->posx;
							pokemon->posy = coord->posy;

							t_appeared_pokemon * appeared_p = malloc(sizeof(t_appeared_pokemon));
							appeared_p->pokemon = pokemon->especie;
							appeared_p->coordenadas.posx = pokemon->posx;
							appeared_p->coordenadas.posy = pokemon->posy;
							contador++;

							seleccionar_el_entrenador_mas_cercano_al_pokemon(pokemon);
							operar_con_appeared_pokemon(appeared_p);
						}
					}

				}
			}
		}

		if(mensaje->operacion == CAUGHT){
			t_caught_pokemon * contenido = mensaje->contenido;
			operar_con_caught_pokemon(contenido->status, mensaje->id_correlacional);
		}

		chequear_si_fue_cumplido_el_objetivo_global();

	}

	return NULL;
}

int main(){

	inicializar_logger();
	inicializar_archivo_de_configuracion();
	inicializar_semaforos();
	configurar_signals();
	inicializar_listas();//sacar los leaks

	hayDeadlock = false;

	definir_objetivo_global();//sacar los leaks
	localizar_entrenadores_en_mapa();
	sem_post(&entrenadores_ubicados);

	//Crea el socket servidor para recibir mensajes de gameboy
	int serv_socket = iniciar_servidor(PUERTO);

	//Crea el socket cliente para conectarse al broker
	enviar_get();

	crear_hilo_para_tratamiento_de_mensajes();
	crear_hilo_para_planificar();
	crear_hilo_para_deadlock();
	//convertirse_en_suscriptor_global_del_broker();
	crear_hilo_de_escucha_para_gameboy(serv_socket);


	close(serv_socket);

	terminar_team_correctamente();
	return 0;
}
