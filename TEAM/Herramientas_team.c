/*
 * Herramientas_team.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Herramientas_team.h"
#include "Funciones_para_listas.h"

bool todos_bloqueados_por_cantidad_maxima(){
	pthread_mutex_lock(&lista_bloq_max_mutex);
	int cant = list_size(lista_bloqueados_cant_max_alcanzada);
	pthread_mutex_unlock(&lista_bloq_max_mutex);

	return list_size(lista_bloqueados_esperando) == 0 && cant > 0;
}

void mostrar_lo_que_hay_en_la_lista_de_objetivos_del_entrenador(t_list * lista){
	int k = 0;
	t_objetivo_entrenador * objetivo = malloc(sizeof(t_objetivo));
	while(!list_is_empty(lista)){
		objetivo = list_get(lista, k);
		if(objetivo == NULL){
			free(objetivo);
			break;
		}
		if(objetivo->cantidad > 0){
			log_info(team_logger,"Un objetivo es de la especie %s, cantidad necesitada %d", objetivo->especie, objetivo->cantidad);
		}
		k++;
	}
	free(objetivo);
}

bool objetivo_personal_cumplido(t_entrenador* entrenador){
	int contador = 0;
	if(entrenador->objetivo == NULL){
		printf("la lista es nula");
	}
	for (int i = 0; i < list_size(entrenador->objetivo); i++){
		t_objetivo_entrenador* un_objetivo = list_get(entrenador->objetivo, i);
		if(un_objetivo->cantidad == 0){
			contador++;
		}
	}
	return (contador == list_size(entrenador->objetivo));
}

bool objetivo_global_cumplido(){
	int contador = 0;
		for (int i = 0; i < list_size(lista_objetivos); i++){
			t_objetivo* un_objetivo = list_get(lista_objetivos, i);
			if(un_objetivo->cantidad_atrapada == un_objetivo->cantidad_necesitada){
				contador++;
			}
		}
		return (contador == list_size(lista_objetivos) && !hayDeadlock);
}

void inicializar_logger(){
	team_logger = log_create("team.log", "Team", 1, LOG_LEVEL_DEBUG);
	log_info(team_logger,"Hi, bienvenido a Team");

	team_logger_oficial = log_create("/home/utnso/log_team1", "Team", 0, LOG_LEVEL_DEBUG);
}

void inicializar_semaforos(){
	array_semaforos = (sem_t*)malloc(sizeof(sem_t)*MAXIMO_ENTRENADORES);
	for(int i = 0; i < MAXIMO_ENTRENADORES; i++){
		sem_init(&array_semaforos[i], 0, 0);
	}

	array_semaforos_finalizar = (sem_t*)malloc(sizeof(sem_t)*MAXIMO_ENTRENADORES);
	for(int i = 0; i < MAXIMO_ENTRENADORES; i++){
		sem_init(&array_semaforos_finalizar[i], 0, 0);
	}

	array_semaforos_caught = (sem_t*)malloc(sizeof(sem_t)*MAXIMO_ENTRENADORES);
	for(int i = 0; i < MAXIMO_ENTRENADORES; i++){
		sem_init(&array_semaforos_caught[i], 0, 0);
	}

	array_semaforos_deadlock = (sem_t*)malloc(sizeof(sem_t)*MAXIMO_ENTRENADORES);
	for(int i = 0; i < MAXIMO_ENTRENADORES; i++){
		sem_init(&array_semaforos_deadlock[i], 0, 0);
	}

	pthread_mutex_init(&mutex_deadlock, NULL);
	pthread_mutex_init(&mapa_mutex, NULL);
	pthread_mutex_init(&llego_gameboy, NULL);
	pthread_mutex_init(&lista_bloq_max_mutex, NULL);
	pthread_mutex_init(&lista_entrenadores_mutex, NULL);
	pthread_mutex_init(&lista_listos_mutex, NULL);
	pthread_mutex_init(&mensaje_nuevo_mutex, NULL);
	pthread_mutex_init(&mensaje_chequear_id_mutex, NULL);
	pthread_mutex_init(&tocando_pokemones_objetivos, NULL);
	pthread_mutex_init(&tocando_semaforos_deadlock, NULL);
	pthread_mutex_init(&lista_comun_deadlock, NULL);
	pthread_mutex_init(&mutex_para_colas, NULL);
	pthread_mutex_init(&mutex_ciclos_cpu, NULL);
	pthread_mutex_init(&pokemones_asignados, NULL);
	pthread_mutex_init(&mutex_ciclos_cpu_entrenador, NULL);
	pthread_mutex_init(&bloqueados_esperando_mutex, NULL);
	pthread_mutex_init(&global_seguir_mutex, NULL);

	sem_init(&entrenadores_ubicados, 0, 0);
	sem_init(&hay_interbloqueo, 0, 0);
	sem_init(&hay_interbloqueo_avisar_a_entrenador, 0, 0);
	sem_init(&semaforos_listos, 0, 0);
	sem_init(&operar_con_localized, 0, 0);
	sem_init(&operar_con_appeared, 0, 0);
	sem_init(&operar_con_caught, 0, 0);
	sem_init(&orden_para_planificar, 0, 0);
	sem_init(&aviso_entrenador_hizo_intercambio, 0, 0);
	sem_init(&ejecucion, 0, 0);
	sem_init(&chequeo_de_deadlock, 0, 0);
	sem_init(&todos_los_entrenadores_finalizaron, 0, 0);
	sem_init(&me_bloquee, 0, 0);
	sem_init(&puedo_volver_a_ejecutar, 0, 0);
	sem_init(&termine_carajo, 0, 0);
	sem_init(&contador_de_deadlocks_producidos, 0, 0);
	sem_init(&ultimo_entrenador, 0, 0);
	sem_init(&podes_sacar_entrenador, 0, 0);
	sem_init(&mapa_y_entrenador, 0, 0);

}


t_semaforo_deadlock * obtener_semaforo_deadlock_por_id(int id){
	bool buscar_sem_entrenador(void * sem){
		return ((t_semaforo_deadlock*)sem)->id_entrenador == id;
	}

	pthread_mutex_lock(&tocando_semaforos_deadlock);
	t_semaforo_deadlock * semaforo =  list_find(semaforos_deadlock, buscar_sem_entrenador);
	pthread_mutex_unlock(&tocando_semaforos_deadlock);
	return semaforo;
}

void destruir_semaforos_deadlock(void * semaforo){
	free(((t_semaforo_deadlock*)semaforo)->semaforo);
	free(semaforo);
}

void inicializar_semaforos_deadlock(){
	semaforos_deadlock = list_create();

	void inicializar_sem(void*entrenador){

		t_semaforo_deadlock * semaforo_deadlock = (t_semaforo_deadlock*)malloc(sizeof(t_semaforo_deadlock));
		sem_t * sem_deadlock = (sem_t *)malloc(sizeof(sem_t));
		sem_init(sem_deadlock, 0, 0);

		semaforo_deadlock->semaforo = sem_deadlock;
		semaforo_deadlock->id_entrenador = ((t_entrenador*)entrenador)->id;

		list_add(semaforos_deadlock, (void*)semaforo_deadlock);
	}

	pthread_mutex_lock(&lista_bloq_max_mutex);
	list_iterate(lista_bloqueados_cant_max_alcanzada, inicializar_sem);
	pthread_mutex_unlock(&lista_bloq_max_mutex);
}

void inicializar_archivo_de_configuracion(char * path){

	config = config_create(path);

	if(config == NULL){
		log_info(team_logger,"Error al cargar el archivo de configuración.");
		terminar_team_correctamente();
	}else{
		log_info(team_logger,"Cargando el archivo de configuracion...");
		obtener_valor_config(KEY_CONFIG_POSICIONES_ENTRENADORES, config, obtener_las_posiciones_de_entrenadores);
		obtener_valor_config(KEY_CONFIG_POKEMON_ENTRENADORES, config, obtener_los_pokemon_de_entrenadores);
		obtener_valor_config(KEY_CONFIG_OBJETIVOS_ENTRENADORES, config, obtener_los_objetivos_de_entrenadores);
		obtener_valor_config(KEY_CONFIG_TIEMPO_RECONEXION, config, obtener_el_tiempo_de_reconexion);
		obtener_valor_config(KEY_CONFIG_RETARDO_CICLO_CPU, config, obtener_el_retardo_de_ciclo_de_cpu);
		obtener_valor_config(KEY_CONFIG_ALGORITMO_PLANIFICACION, config, obtener_el_algoritmo_de_planificacion);
		obtener_valor_config(KEY_CONFIG_ALPHA, config, obtener_el_alpha);
		obtener_valor_config(KEY_CONFIG_QUANTUM, config, obtener_el_quantum);
		obtener_valor_config(KEY_CONFIG_ESTIMACION_INICIAL, config, obtener_la_estimacion_inicial);
		obtener_valor_config(KEY_CONFIG_IP_BROKER, config, obtener_la_ip_del_broker);
		obtener_valor_config(KEY_CONFIG_PUERTO_BROKER, config, obtener_el_puerto_del_broker);
		obtener_valor_config(KEY_CONFIG_PUERTO_TEAM, config, obtener_el_puerto_de_team);
		obtener_valor_config(KEY_CONFIG_LOG_FILE, config, obtener_el_log_file);
		obtener_valor_config(KEY_CONFIG_ID, config, obtener_el_id);


		log_info(team_logger,"Archivo de configuracion cargado correctamente :)\n");
		config_destroy(config);
	}

}

void obtener_valor_config(char* key, t_config* file, void(*obtener)(void)){
	if (config_has_property(file, key)){
		obtener();
	}
}

void obtener_el_id(){
	id = config_get_int_value(config, KEY_CONFIG_ID);
	log_debug(team_logger,"Mi ID es: %d",id);
}

void obtener_las_posiciones_de_entrenadores(){
	posiciones_entrenadores = config_get_array_value(config, KEY_CONFIG_POSICIONES_ENTRENADORES);
	t_list * lista_aux = list_create();
	string_iterate_lines_with_list(posiciones_entrenadores, lista_aux, separar_pokemones_de_entrenador);
	MAXIMO_ENTRENADORES = list_size(lista_aux)/2;
	log_debug(team_logger,"Las posiciones de entrenadores recuperadas");
	list_destroy(lista_aux);
}

void obtener_los_pokemon_de_entrenadores(){
	pokemon_entrenadores = config_get_array_value(config, KEY_CONFIG_POKEMON_ENTRENADORES);
	log_debug(team_logger,"Los pokemon de los entrenadores recuperados");
}

void obtener_los_objetivos_de_entrenadores(){
	objetivos_entrenadores = config_get_array_value(config, KEY_CONFIG_OBJETIVOS_ENTRENADORES);
	log_debug(team_logger,"Los objetivos de los entrenadores recuperados");
}

void obtener_el_tiempo_de_reconexion(){
	tiempo_reconexion = config_get_int_value(config, KEY_CONFIG_TIEMPO_RECONEXION);
	log_debug(team_logger,"El tiempo de reconexion es: %d",tiempo_reconexion);
}

void obtener_el_alpha(){
	alpha = config_get_int_value(config, KEY_CONFIG_ALPHA);
	log_debug(team_logger,"El alpha es: %d",alpha);
}

void obtener_el_retardo_de_ciclo_de_cpu(){
    retardo_ciclo_cpu = config_get_int_value(config, KEY_CONFIG_RETARDO_CICLO_CPU);
	log_debug(team_logger,"El retardo de ciclo de cpu es: %d",retardo_ciclo_cpu);
}

void obtener_el_algoritmo_de_planificacion(){
	algoritmo_planificacion = strdup(config_get_string_value(config, KEY_CONFIG_ALGORITMO_PLANIFICACION));
	log_debug(team_logger,"El algoritmo de planificacion es: %s",algoritmo_planificacion);
}

void obtener_el_quantum(){
	quantum = config_get_int_value(config, KEY_CONFIG_QUANTUM);
	log_debug(team_logger,"El quantum es: %d",quantum);
}

void obtener_la_estimacion_inicial(){
	estimacion_inicial = config_get_int_value(config, KEY_CONFIG_ESTIMACION_INICIAL);
	log_debug(team_logger,"La estimacion_inicial es: %d",estimacion_inicial);
}

void obtener_la_ip_del_broker(){
	ip_broker = strdup(config_get_string_value(config, KEY_CONFIG_IP_BROKER));
	log_debug(team_logger,"La ip del broker es: %s",ip_broker);
}

void obtener_el_puerto_del_broker(){
	puerto_broker = strdup(config_get_string_value(config, KEY_CONFIG_PUERTO_BROKER));
	log_debug(team_logger,"El puerto del broker es: %s",puerto_broker);
}

void obtener_el_puerto_de_team(){
	puerto_team = strdup(config_get_string_value(config, KEY_CONFIG_PUERTO_TEAM));
	log_debug(team_logger,"El puerto de Team es: %s",puerto_team);
}

void obtener_el_log_file(){
	log_file = strdup(config_get_string_value(config, KEY_CONFIG_LOG_FILE));
	log_debug(team_logger,"El log file es: %s",log_file);
}

void configurar_signals(void){
	struct sigaction signal_struct;
	signal_struct.sa_handler = capturar_signal;
	signal_struct.sa_flags = 0;

	sigemptyset(&signal_struct.sa_mask);

	sigaddset(&signal_struct.sa_mask, SIGPIPE);
	if (sigaction(SIGPIPE, &signal_struct, NULL) < 0) {
		log_info(team_logger, " SIGACTION error ");
	}

	sigaddset(&signal_struct.sa_mask, SIGINT);
	if (sigaction(SIGINT, &signal_struct, NULL) < 0) {
		log_info(team_logger, " SIGACTION error ");
	}

}

void capturar_signal(int signo){

    if(signo == SIGINT)
    {
    	log_warning(team_logger,"Team deja de funcionar");
    	GLOBAL_SEGUIR = 0;
    	terminar_team_correctamente();

    }
    else if(signo == SIGPIPE)
    {
    	log_info(team_logger,"Desconectado");
    }

}

int destruir_pokemon(t_pokemon * pokemon){
	if(pokemon->especie != NULL) free(pokemon->especie);	
	free(pokemon);
	return 0;
}

int destruir_entrenador(t_entrenador * entrenador){
	list_destroy_and_destroy_elements(entrenador->objetivo,(void*)destruir_objetivo_entrenador);
	list_destroy_and_destroy_elements(entrenador->pokemones, (void*)destruir_pokemon);
	destruir_objetivo(entrenador->objetivo_actual);
	free(entrenador);
	return 0;
}

int destruir_objetivo(t_objetivo * objetivo){
	free(objetivo);
	return 0;
}

int destruir_objetivo_entrenador(t_objetivo_entrenador * objetivo){
	free(objetivo);
	return 0;
}

int destruir_mensaje(t_mensaje_guardado * mensaje){
	free(mensaje);
	return 0;
}

void liberar_lista_char(char** lista){
	int contador = 0;
	while(lista[contador] != NULL){
		free(lista[contador]);
		contador++;
	}
	//free(lista);
}

void terminar_team_correctamente(){
	pthread_mutex_lock(&global_seguir_mutex);
	GLOBAL_SEGUIR = 0;
	pthread_mutex_unlock(&global_seguir_mutex);

	log_info(team_logger,"Cerrando team...");

	/*HAY QUE DESTRUIR TODAS LAS LISTAS*/
	list_destroy_and_destroy_elements(lista_mapa,(void*)destruir_pokemon);
	list_destroy(pokemones_ordenada);
	list_destroy(lista_entrenadores);
	list_destroy_and_destroy_elements(lista_listos,(void*)destruir_entrenador);
	list_destroy_and_destroy_elements(lista_finalizar,(void*)destruir_entrenador);
	list_destroy_and_destroy_elements(lista_bloqueados_esperando_caught,(void*)destruir_entrenador);
	list_destroy_and_destroy_elements(lista_bloqueados_deadlock,(void*)destruir_entrenador);
	list_destroy(lista_bloqueados_cant_max_alcanzada);
	list_destroy_and_destroy_elements(lista_bloqueados_esperando,(void*)destruir_entrenador);
	list_destroy_and_destroy_elements(lista_bloqueados,(void*)destruir_entrenador);

	list_destroy_and_destroy_elements(lista_global_objetivos,(void*)destruir_objetivo);
	list_destroy_and_destroy_elements(lista_global_pokemones,(void*)destruir_pokemon);

	list_destroy_and_destroy_elements(mensajes_para_chequear_id,(void*)destruir_mensaje);
	list_destroy_and_destroy_elements(mensajes_que_llegan_nuevos,(void*)destruir_mensaje);
	list_destroy_and_destroy_elements(lista_objetivos, (void*)destruir_objetivo);
	list_destroy(mensajes);
	//liberar_lista_char(pokes);

	log_destroy(team_logger);
	log_destroy(team_logger_oficial);

	if(log_file!=NULL){
		free(log_file);
		log_file = NULL;
	}

	if(puerto_broker!=NULL){
		free(puerto_broker);
		puerto_broker = NULL;
	}

	if(ip_broker!=NULL){
		free(ip_broker);
		ip_broker = NULL;
	}

	if(algoritmo_planificacion!=NULL){
		free(algoritmo_planificacion);
		algoritmo_planificacion = NULL;
	}

	//return;
	exit(EXIT_SUCCESS);

}

t_objetivo * buscar_pokemon_por_especie(t_list* lista, char* especie){

	bool es_la_especie_buscada(t_objetivo* pokemon){
		return (string_equals_ignore_case(pokemon->especie, especie));
	}
	return (list_find(lista,(void*)es_la_especie_buscada));
}

t_objetivo_entrenador * buscar_pokemon_objetivo_por_especie(t_list* lista, char* especie){

	bool es_la_especie_buscada(t_objetivo_entrenador* pokemon){
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

t_entrenador * buscar_entrenador_por_ubicacion(t_list* lista, uint32_t posx, uint32_t posy){
	bool es_el_buscado(t_entrenador* entrenador){
		return entrenador->posx == posx && entrenador->posy == posy;
	}
	return (list_find(lista,(void*)es_el_buscado));
}

t_pokemon * buscar_pokemon_por_especie_y_ubicacion(t_list * lista, t_pokemon * pokemon){
	bool es_la_especie_buscada(t_pokemon* pokemon1){
		return (string_equals_ignore_case(pokemon1->especie, pokemon->especie)) && pokemon1->posx == pokemon->posx && pokemon1->posy == pokemon->posy;
	}
	return (list_find(lista,(void*)es_la_especie_buscada));
}

t_entrenador * buscar_entrenador_por_objetivo_actual(t_catch_pokemon* catch_pokemon){
	bool es_el_buscado(t_entrenador* entrenador){
		return entrenador->objetivo_actual->especie == catch_pokemon->pokemon;
	}
	return (list_find(lista_bloqueados_esperando_caught,(void*)es_el_buscado));
}

t_mensaje_guardado * buscar_mensaje(uint32_t id){
	bool es_el_buscado(t_mensaje_guardado* mensaje){
		return mensaje->id == id;
	}
	return (list_find(mensajes,(void*)es_el_buscado));
}

t_mensaje_guardado * buscar_mensaje_por_id(uint32_t id_correlativo, t_list* mensajes){

	bool es_el_buscado(void* _mensaje){
		t_mensaje_guardado* mensaje = (t_mensaje_guardado*) _mensaje;
		return mensaje->id == id_correlativo;
	}

	return (list_find(mensajes,es_el_buscado));
}

t_mensaje_guardado * buscar_mensaje_appeared_por_especie(char* especie, t_list* mensajes){
	bool es_el_buscado(void* _mensaje){
		t_mensaje_guardado* mensaje = (t_mensaje_guardado*) _mensaje;
		return mensaje->operacion == APPEARED && (((t_appeared_pokemon*)(mensaje->contenido))->pokemon == especie);
	}
	return (list_find(mensajes,(void*)es_el_buscado));
}