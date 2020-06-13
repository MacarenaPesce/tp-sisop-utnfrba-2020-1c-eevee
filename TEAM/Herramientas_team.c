/*
 * Herramientas_team.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Herramientas_team.h"
#include "Funciones_para_listas.h"

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
		return (contador == list_size(lista_objetivos)); //es verdadero y cumple esto y ademas no haya deadlock
}

void inicializar_logger(){
	team_logger = log_create("team.log", "Team", 1, LOG_LEVEL_DEBUG);
	log_info(team_logger,"Hi, bienvenido a Team");

	team_logger_oficial = log_create("/home/utnso/log_team1", "Team", 0, LOG_LEVEL_DEBUG);
	log_info(team_logger_oficial,"Bienvenido a Team, este es el archivo de log oficial");
}

void inicializar_semaforos(){
	array_semaforos = (sem_t*)malloc(sizeof(sem_t)*MAXIMO_ENTRENADORES);
	for(int i = 0; i < MAXIMO_ENTRENADORES; i++){
		sem_init(&array_semaforos[i], 0, 0);
	}

	sem_init(&hay_un_pokemon_nuevo, 0, 0);
}

void inicializar_archivo_de_configuracion(){
	config = config_create("team.config");
	if(config == NULL){
		log_info(team_logger,"El archivo de configuracion no existe. Fijate en la carpeta Debug.");
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
		obtener_valor_config(KEY_CONFIG_LOG_FILE, config, obtener_el_log_file);


		log_info(team_logger,"Archivo de configuracion cargado correctamente :)\n");
		config_destroy(config);

	}

}

void obtener_valor_config(char* key, t_config* file, void(*obtener)(void)){
	if (config_has_property(file, key)){
		obtener();
	}
}

void obtener_las_posiciones_de_entrenadores(){
	posiciones_entrenadores = config_get_array_value(config, KEY_CONFIG_POSICIONES_ENTRENADORES);
	t_list * lista_aux = list_create();
	string_iterate_lines_with_list(posiciones_entrenadores, lista_aux, separar_pokemones_de_entrenador);
	MAXIMO_ENTRENADORES = list_size(lista_aux)/2;
	//log_info(team_logger,"MAXIMO ENTRENADORES %d", MAXIMO_ENTRENADORES);
	log_info(team_logger,"Las posiciones de entrenadores recuperadas");
	list_destroy(lista_aux);
}

void obtener_los_pokemon_de_entrenadores(){
	pokemon_entrenadores = config_get_array_value(config, KEY_CONFIG_POKEMON_ENTRENADORES);
	log_info(team_logger,"Los pokemon de los entrenadores recuperados");
}

void obtener_los_objetivos_de_entrenadores(){
	objetivos_entrenadores = config_get_array_value(config, KEY_CONFIG_OBJETIVOS_ENTRENADORES);
	log_info(team_logger,"Los objetivos de los entrenadores recuperados");
}

void obtener_el_tiempo_de_reconexion(){
	tiempo_reconexion = config_get_int_value(config, KEY_CONFIG_TIEMPO_RECONEXION);
	log_info(team_logger,"El tiempo de reconexion es: %d",tiempo_reconexion);
}

void obtener_el_alpha(){
	alpha = config_get_int_value(config, KEY_CONFIG_ALPHA);
	log_info(team_logger,"El alpha es: %d",alpha);
}

void obtener_el_retardo_de_ciclo_de_cpu(){
    retardo_ciclo_cpu = config_get_int_value(config, KEY_CONFIG_RETARDO_CICLO_CPU);
	log_info(team_logger,"El retardo de ciclo de cpu es: %d",retardo_ciclo_cpu);
}

void obtener_el_algoritmo_de_planificacion(){
	algoritmo_planificacion = strdup(config_get_string_value(config, KEY_CONFIG_ALGORITMO_PLANIFICACION));
	log_info(team_logger,"El algoritmo de planificacion es: %s",algoritmo_planificacion);
}

void obtener_el_quantum(){
	quantum = config_get_int_value(config, KEY_CONFIG_QUANTUM);
	log_info(team_logger,"El quantum es: %d",quantum);
}

void obtener_la_estimacion_inicial(){
	estimacion_inicial = config_get_int_value(config, KEY_CONFIG_ESTIMACION_INICIAL);
	log_info(team_logger,"La estimacion_inicial es: %d",estimacion_inicial);
}

void obtener_la_ip_del_broker(){
	ip_broker = strdup(config_get_string_value(config, KEY_CONFIG_IP_BROKER));
	log_info(team_logger,"La ip del broker es: %s",ip_broker);
}

void obtener_el_puerto_del_broker(){
	puerto_broker = strdup(config_get_string_value(config, KEY_CONFIG_PUERTO_BROKER));
	log_info(team_logger,"El puerto del broker es: %s",puerto_broker);
}

void obtener_el_log_file(){
	log_file = strdup(config_get_string_value(config, KEY_CONFIG_LOG_FILE));
	log_info(team_logger,"El log file es: %s",log_file);
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
	sigaddset(&signal_struct.sa_mask, SIGSEGV);
	if (sigaction(SIGINT, &signal_struct, NULL) < 0) {
		log_info(team_logger, " SIGACTION error ");
	}
}

void capturar_signal(int signo){

    if(signo == SIGINT)
    {
    	log_info(team_logger,"\n TEAM DEJA DE FUNCIONAR");
    	GLOBAL_SEGUIR = 0;
    	terminar_team_correctamente();
    	exit(EXIT_FAILURE);

    }
    else if(signo == SIGPIPE)
    {
    	log_info(team_logger,"Desconectado");
    }
    else if(signo == SIGSEGV)
	{
		log_info(team_logger,"SEGMENTATION FAULT");
	}

}

int destruir_entrenador(t_entrenador * entrenador){
	/*if(entrenador->objetivo!=NULL){
		free(entrenador->objetivo);
		entrenador->objetivo = NULL;
	}*/
	free(entrenador);

	return 0;
}

int destruir_objetivo(t_objetivo * objetivo){
	/*if(objetivo->especie!=NULL){
		free(objetivo->especie);
		objetivo->especie = NULL;
	}*/
	free(objetivo);

	return 0;
}

int destruir_objetivo_entrenador(t_objetivo_entrenador * objetivo){
	/*if(objetivo->especie!=NULL){
		free(objetivo->especie);
		objetivo->especie = NULL;
	}*/
	free(objetivo);

	return 0;
}

int destruir_pokemon(t_pokemon * pokemon){
	/*if(pokemon->especie!=NULL){
		free(pokemon->especie);
		pokemon->especie = NULL;
	}*/
	free(pokemon);

	return 0;
}

void terminar_team_correctamente(){
	log_info(team_logger,"Cerrando team...");
/*
	list_destroy(lista_entrenadores);
	list_destroy(lista_objetivos);
	list_destroy(lista_listos);
	list_destroy(lista_finalizar);
	list_destroy(lista_bloqueados);
	list_destroy_and_destroy_elements(lista_mapa,(void*)destruir_pokemon);
	//list_destroy_and_destroy_elements(lista_pokemones_objetivos,(void*)destruir_objetivo_entrenador);
	//list_destroy_and_destroy_elements(lista_pokemones_de_entrenador,(void*)destruir_objetivo_entrenador);
	//list_destroy_and_destroy_elements(lista_objetivos_de_entrenador,(void*)destruir_objetivo_entrenador);



	if(entrenador_en_ejecucion!=NULL){
		destruir_entrenador(entrenador_en_ejecucion);
	}

	//log_destroy(team_logger);
	 *
	if(posiciones_entrenadores!=NULL)
	{
		free(posiciones_entrenadores);
		posiciones_entrenadores = NULL;
	}

	if(pokemon_entrenadores!=NULL)
	{
		free(pokemon_entrenadores);
		pokemon_entrenadores = NULL;
	}

	if(objetivos_entrenadores!=NULL)
	{
		free(objetivos_entrenadores);
		objetivos_entrenadores = NULL;
	}*/

	if(log_file!=NULL)
	{
		free(log_file);
		log_file = NULL;
	}

	if(puerto_broker!=NULL)
	{
		free(puerto_broker);
		puerto_broker = NULL;
	}

	if(ip_broker!=NULL)
	{
		free(ip_broker);
		ip_broker = NULL;
	}

	if(algoritmo_planificacion!=NULL)
	{
		free(algoritmo_planificacion);
		algoritmo_planificacion = NULL;
	}

	return;

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

t_entrenador * buscar_entrenador_por_objetivo_actual(t_catch_pokemon* catch_pokemon){
	bool es_el_buscado(t_entrenador* entrenador){
		return entrenador->objetivo_actual == catch_pokemon;
	}
	return (list_find(lista_bloqueados,(void*)es_el_buscado));
}

t_mensaje_guardado * buscar_mensaje(uint32_t id){
	bool es_el_buscado(t_mensaje_guardado* mensaje){
		return mensaje->id == id;
	}
	return (list_find(mensajes,(void*)es_el_buscado));
}
