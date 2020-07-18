/*
 * Deadlock.c
 *
 *  Created on: 16 jun. 2020
 *      Author: utnso
 */

#include "Deadlock.h"

void crear_hilo_para_deadlock(){
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)chequear_deadlock, NULL);
	pthread_detach(hilo);
}

void chequear_cantidad_de_deadlocks_producidos(){
	hayDeadlock = true;
	if(es_el_primer_deadlock){
		log_info(team_logger_oficial, "Inicio del algoritmo de deteccion y resolucion de deadlock");	
		pthread_mutex_lock(&lista_bloq_max_mutex);
		t_list * aux_a_recorrer = list_duplicate(lista_bloqueados_cant_max_alcanzada);
		pthread_mutex_unlock(&lista_bloq_max_mutex);
	
		signed int SIN_ESPERA_ASIGNADA = CANTIDAD_EN_DEADLOCK;
		uint32_t id_de_entrenador_nuevo = -1;
		t_list * lista_espera_circular = list_create();
		t_entrenador * un_entrenador;
		t_entrenador * otro_entrenador;
		t_objetivo_entrenador * pokemon_innecesario_de_uno;
		t_objetivo_entrenador * pokemon_innecesario_de_otro;
		t_objetivo_entrenador* pokemon_objetivo_de_entrenador;
		 t_objetivo_entrenador * pokemon_objetivo_de_uno;
		 t_objetivo_entrenador * pokemon_objetivo_de_otro;

		while(SIN_ESPERA_ASIGNADA != 0){
			if(list_size(aux_a_recorrer) == 1){
				break;
			}
			if(id_de_entrenador_nuevo == -1){
				un_entrenador = list_get(aux_a_recorrer, 0);
				un_entrenador->espera_asignada = true;
				SIN_ESPERA_ASIGNADA--;
				espera_circular++;
			} else {
				un_entrenador = buscar_entrenador_por_id(aux_a_recorrer, id_de_entrenador_nuevo);
				uint32_t id_de_entrenador_nuevo = -1;
			}
		
			pokemon_innecesario_de_uno = elegir_pokemon_innecesario(un_entrenador);
	
			for(int i = 1; i < list_size(aux_a_recorrer); i++){
				otro_entrenador = list_get(aux_a_recorrer, i);
				if(otro_entrenador == NULL){
					break;
				}
				if(!otro_entrenador->espera_asignada){
					pokemon_objetivo_de_otro = buscar_pokemon_objetivo_por_especie(otro_entrenador->objetivo, pokemon_innecesario_de_uno->especie);
					if(pokemon_objetivo_de_otro != NULL){
						otro_entrenador->espera_asignada = true;
						SIN_ESPERA_ASIGNADA--;			
						break;
					}
				}
			}
		
			pokemon_innecesario_de_otro = elegir_pokemon_innecesario_util(otro_entrenador, un_entrenador);
	
			pokemon_objetivo_de_uno = buscar_pokemon_objetivo_por_especie(un_entrenador->objetivo, pokemon_innecesario_de_otro->especie);
			if(pokemon_objetivo_de_uno != NULL){
				t_entrenador * entrenador_eliminado = sacar_entrenador_de_lista_pid(aux_a_recorrer, un_entrenador->id);
				list_add(lista_espera_circular, un_entrenador);
				//si 1 tiene mas de un pokemon innecesario entonces me guardo el id
				if(tiene_mas_de_un_innecesario(otro_entrenador)){
					espera_circular++;
					id_de_entrenador_nuevo = otro_entrenador->id;
				}
			} else {
				id_de_entrenador_nuevo = otro_entrenador->id;
				if(lista_espera_circular != NULL){
					for(int j = 0; j < list_size(lista_espera_circular); j++){
						t_entrenador * entrenador = list_get(lista_espera_circular, 0);
						pokemon_objetivo_de_entrenador = buscar_pokemon_objetivo_por_especie(entrenador->objetivo, pokemon_innecesario_de_otro->especie);
						if(pokemon_objetivo_de_entrenador != NULL){
							sacar_entrenador_de_lista_pid(aux_a_recorrer, un_entrenador->id);
							sacar_entrenador_de_lista_pid(aux_a_recorrer, otro_entrenador->id);
							SIN_ESPERA_ASIGNADA--;
							break;
						}
					}
				}
			}
		}
		es_el_primer_deadlock = false;
		deadlocks_producidos = espera_circular;
		log_info(team_logger, "La cantidad de deadlocks producidos es %i", espera_circular);
		log_info(team_logger_oficial, "Cantidad de deadlocks detectados: %i", espera_circular);
		list_destroy(aux_a_recorrer);
		list_destroy(lista_espera_circular);
	}
}

void * chequear_deadlock(){

	while(GLOBAL_SEGUIR){
		sem_wait(&chequeo_de_deadlock);

		pthread_mutex_lock(&lista_entrenadores_mutex);
		int cant_nuevos = list_size(lista_entrenadores);;
		pthread_mutex_unlock(&lista_entrenadores_mutex);

		pthread_mutex_lock(&lista_listos_mutex);
		int cant_ready = list_size(lista_listos);
		pthread_mutex_unlock(&lista_listos_mutex);

		if(cant_nuevos == 0 && todos_bloqueados_por_cantidad_maxima() && cant_ready == 0){
			printf("\n");
			log_warning(team_logger, "Deadlock detectado");

			pthread_mutex_lock(&lista_bloq_max_mutex);
			CANTIDAD_EN_DEADLOCK = list_size(lista_bloqueados_cant_max_alcanzada);
			pthread_mutex_unlock(&lista_bloq_max_mutex);

			log_info(team_logger_oficial, "La cantidad de entrenadores en deadlock es %d", CANTIDAD_EN_DEADLOCK);
			log_info(team_logger,"La cantidad de entrenadores en deadlock es %d", CANTIDAD_EN_DEADLOCK);
			
			chequear_cantidad_de_deadlocks_producidos();
			//sem_wait(&contador_de_deadlocks_producidos);
			break;
		}
	}

	pthread_mutex_lock(&lista_bloq_max_mutex);
	t_entrenador* entrenador1 = list_get(lista_bloqueados_cant_max_alcanzada, 0);
	sacar_entrenador_de_lista_pid(lista_bloqueados_cant_max_alcanzada,entrenador1->id);
	pthread_mutex_unlock(&lista_bloq_max_mutex);

	pthread_mutex_lock(&lista_listos_mutex);
	list_add(lista_listos, entrenador1);
	entrenador1 = sacar_entrenador_de_lista_pid(lista_listos, entrenador1->id);
	pthread_mutex_unlock(&lista_listos_mutex);
	
	log_info(team_logger, "El entrenador %d pasó a la lista de listos para resolucion de deadlock", entrenador1->id);
	log_info(team_logger_oficial, "El entrenador %d pasó a la lista de listos para resolucion de deadlock", entrenador1->id);
	entrenador1->estado = EJECUTANDO;

	//pthread_mutex_lock(&tocando_pokemones_objetivos);
	ver_entre_quienes_hay_deadlock_y_resolverlo(entrenador1);
	//pthread_mutex_unlock(&tocando_pokemones_objetivos);
	printf("\n");

	return NULL;
}

void ver_entre_quienes_hay_deadlock_y_resolverlo(t_entrenador * entrenador1){

	t_entrenador* entrenador2;
	t_objetivo_entrenador* pokemon1 = elegir_pokemon_innecesario(entrenador1);
  
	log_info(team_logger, "El pokemon innecesario del entrenador %i es  %s", entrenador1->id, pokemon1->especie);
	
	int cant;

	for(int i = 0; i < list_size(lista_bloqueados_cant_max_alcanzada); i++){
		if(pokemon1 == NULL){
			log_info(team_logger, "El pokemon innecesario del entrenador %i es NULO", entrenador1->id);
		}
		pthread_mutex_lock(&lista_bloq_max_mutex);
		entrenador2 = list_get(lista_bloqueados_cant_max_alcanzada, i);
		pthread_mutex_unlock(&lista_bloq_max_mutex);

		/*for(int i = 0; i<(list_size(entrenador2->objetivo));i++){
			t_objetivo_entrenador* pokemonnn = list_get(entrenador2->objetivo, i);
			log_error(team_logger, "POKEMON %s VUELTA %d", pokemonnn->especie, i);
		}*/

		//log_error(team_logger, "EL POKEMON INNECESARIO ES %s", pokemon1->especie);

		t_objetivo_entrenador* pokemon2 = buscar_pokemon_objetivo_por_especie(entrenador2->objetivo, pokemon1->especie);
		//log_info(team_logger, "La especie del pokemon2 es %s", pokemon2->especie);
		if(pokemon2 != NULL){
			//pthread_mutex_lock(&tocando_pokemones_objetivos);
			cant = (int)pokemon2->cantidad;
			//log_info(team_logger, "La cantidad es %i", cant);
			//pthread_mutex_unlock(&tocando_pokemones_objetivos);
			if(cant > 0){
				log_info(team_logger, "Hay deadlock entre el entrenador %d y el entrenador %d", entrenador1->id, entrenador2->id);
				break;
			}
		}
	}

	planificar_para_deadlock(entrenador1, entrenador2, pokemon1);
}


void planificar_para_deadlock(t_entrenador* entrenador1, t_entrenador* entrenador2, t_objetivo_entrenador* pokemon){

	t_pokemon* pokemon_innecesario = malloc(sizeof(t_pokemon));
	pokemon_innecesario->especie = pokemon->especie;
	pokemon_innecesario->posx = entrenador2->posx;
	pokemon_innecesario->posy = entrenador2->posy;
	entrenador1->objetivo_actual = pokemon_innecesario;

	pthread_mutex_lock(&lista_comun_deadlock);
	list_add(lista_bloqueados_deadlock, entrenador2);
	pthread_mutex_unlock(&lista_comun_deadlock);

	pthread_mutex_lock(&lista_bloq_max_mutex);
	sacar_entrenador_de_lista_pid(lista_bloqueados_cant_max_alcanzada,entrenador2->id);
	pthread_mutex_unlock(&lista_bloq_max_mutex);

	log_info(team_logger, "El entrenador %d pasó a la lista de bloqueados esperando resolucion de deadlock", entrenador2->id);
	log_info(team_logger_oficial, "El entrenador %d pasó a la lista de bloqueados esperando resolucion de deadlock", entrenador2->id);
	log_info(team_logger_oficial, "El entrenador %d pasó a ejecutar", entrenador1->id);
	log_info(team_logger, "Haciendo el intercambio");
	
	sem_post(&array_semaforos_deadlock[entrenador1->id]);
	cambios_de_contexto++;
	//ESPERAR A ENTRENADOR
	sem_wait(&aviso_entrenador_hizo_intercambio);
	
	verificar_si_entrenador_sigue_bloqueado(entrenador1);
	verificar_si_entrenador_sigue_bloqueado(entrenador2);

	if(objetivo_personal_cumplido(entrenador2)){
		sem_post(&array_semaforos_deadlock[entrenador2->id]);
	}
	
	verificar_si_sigue_habiendo_deadlock_luego_del_intercambio();

}

void verificar_si_entrenador_sigue_bloqueado(t_entrenador* entrenador){

	if(!objetivo_personal_cumplido(entrenador)){

		pthread_mutex_lock(&lista_bloq_max_mutex);
		list_add(lista_bloqueados_cant_max_alcanzada, entrenador);
		pthread_mutex_unlock(&lista_bloq_max_mutex);

		pthread_mutex_lock(&lista_comun_deadlock);
		sacar_entrenador_de_lista_pid(lista_bloqueados_deadlock,entrenador->id);
		pthread_mutex_unlock(&lista_comun_deadlock);

		log_info(team_logger, "Luego de hacer un intercambio el entrenador %d paso a la lista de bloqueados con cantidad maxima", entrenador->id);
		log_info(team_logger_oficial, "Luego de hacer un intercambio el entrenador %d paso a la lista de bloqueados con cantidad maxima", entrenador->id);

	}

	if(objetivo_personal_cumplido(entrenador)){
		pthread_mutex_lock(&lista_bloq_max_mutex);
		sacar_entrenador_de_lista_pid(lista_bloqueados_cant_max_alcanzada,entrenador->id);
		pthread_mutex_unlock(&lista_bloq_max_mutex);

		pthread_mutex_lock(&lista_comun_deadlock);
		sacar_entrenador_de_lista_pid(lista_bloqueados_deadlock,entrenador->id);
		pthread_mutex_unlock(&lista_comun_deadlock);
		list_add(lista_finalizar, entrenador);
		sem_post(&array_semaforos_finalizar[entrenador->id]);
	}

}

void verificar_si_sigue_habiendo_deadlock_luego_del_intercambio(){
	list_clean(lista_listos);
	pthread_mutex_lock(&lista_bloq_max_mutex);
	CANTIDAD_EN_DEADLOCK = list_size(lista_bloqueados_cant_max_alcanzada);
	pthread_mutex_unlock(&lista_bloq_max_mutex);

	if(CANTIDAD_EN_DEADLOCK > 0){		
		sem_wait(&puedo_volver_a_ejecutar);
		sem_post(&chequeo_de_deadlock);
		chequear_deadlock();
	}else{
		hayDeadlock = false;
		deadlocks_resueltos = espera_circular;
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

