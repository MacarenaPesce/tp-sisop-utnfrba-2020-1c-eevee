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
}

void * chequear_deadlock(){

	while(GLOBAL_SEGUIR){
		pthread_mutex_lock(&lista_entrenadores_mutex);
		int cant_nuevos = list_size(lista_entrenadores);
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

			log_info(team_logger,"La cantidad de entrenadores en deadlock es %d", CANTIDAD_EN_DEADLOCK);

			inicializar_semaforos_deadlock();
			break;
		}
	}

	t_entrenador* entrenador1 = malloc(sizeof(t_entrenador));

	pthread_mutex_lock(&lista_bloq_max_mutex);
	entrenador1 = list_get(lista_bloqueados_cant_max_alcanzada, 0);
	pthread_mutex_unlock(&lista_bloq_max_mutex);

	pthread_mutex_lock(&lista_bloq_max_mutex);
	sacar_entrenador_de_lista_pid(lista_bloqueados_cant_max_alcanzada,entrenador1->id);
	pthread_mutex_unlock(&lista_bloq_max_mutex);

	pthread_mutex_lock(&lista_listos_mutex);
	list_add(lista_listos, entrenador1);
	entrenador1 = sacar_entrenador_de_lista_pid(lista_listos, entrenador1->id);
	pthread_mutex_unlock(&lista_listos_mutex);
	log_info(team_logger, "El entrenador %d pasó a la lista de listos para resolucion de deadlock", entrenador1->id);

	entrenador1->estado = EJECUTANDO;

	interbloqueo(entrenador1);
	printf("\n");

	return NULL;
}

void interbloqueo(t_entrenador * entrenador1){

	pthread_mutex_lock(&mutex_deadlock);
	pthread_mutex_lock(&lista_bloq_max_mutex);
	hayDeadlock = list_size(lista_bloqueados_cant_max_alcanzada) > 1;
	pthread_mutex_unlock(&lista_bloq_max_mutex);
	pthread_mutex_unlock(&mutex_deadlock);

	ver_entre_quienes_hay_deadlock_y_resolverlo(entrenador1);

}

void ver_entre_quienes_hay_deadlock_y_resolverlo(t_entrenador * entrenador1){

	t_entrenador* entrenador2 = malloc(sizeof(t_entrenador));
	t_objetivo_entrenador* pokemon1 = elegir_pokemon_innecesario(entrenador1);

	for(int i = 0; i < list_size(lista_bloqueados_cant_max_alcanzada); i++){

		pthread_mutex_lock(&lista_bloq_max_mutex);
		entrenador2 = list_get(lista_bloqueados_cant_max_alcanzada, i);
		pthread_mutex_unlock(&lista_bloq_max_mutex);

		t_objetivo_entrenador* pokemon2 = buscar_pokemon_objetivo_por_especie(entrenador2->objetivo, pokemon1->especie);
			if(pokemon2 != NULL){
				if(pokemon2->cantidad > 0){
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

	list_add(lista_bloqueados_deadlock, entrenador2);
	log_info(team_logger, "El entrenador %d pasó a la lista de bloqueados esperando resolucion de deadlock", entrenador2->id);

	log_info(team_logger, "Haciendo el intercambio");
	printf("\n");


	sem_post(&hay_interbloqueo);
	t_semaforo_deadlock * sem_entrenador_deadlock = obtener_semaforo_deadlock_por_id(entrenador1->id);
	sem_post(sem_entrenador_deadlock->semaforo);


	//ESPERAR A ENTRENADOR
	sem_wait(&aviso_entrenador_hizo_intercambio);

	verificar_si_entrenador_sigue_bloqueado(entrenador1);
	verificar_si_entrenador_sigue_bloqueado(entrenador2);

	verificar_si_sigue_habiendo_deadlock_luego_del_intercambio();

}

void verificar_si_entrenador_sigue_bloqueado(t_entrenador* entrenador){

	if(!objetivo_personal_cumplido(entrenador)){

		pthread_mutex_lock(&lista_bloq_max_mutex);
		list_add(lista_bloqueados_cant_max_alcanzada, entrenador);
		pthread_mutex_unlock(&lista_bloq_max_mutex);

		list_add(lista_bloqueados_deadlock, entrenador);
		printf("\n");
		log_info(team_logger, "Luego de hacer un intercambio el entrenador %d paso a la lista de bloqueados con cantidad maxima", entrenador->id);

	}

	if(objetivo_personal_cumplido(entrenador)){

		pthread_mutex_lock(&lista_bloq_max_mutex);
		sacar_entrenador_de_lista_pid(lista_bloqueados_cant_max_alcanzada,entrenador->id);
		pthread_mutex_unlock(&lista_bloq_max_mutex);

		list_add(lista_finalizar, entrenador);

		sem_post(&array_semaforos_finalizar[entrenador->id]);
	}
}

void verificar_si_sigue_habiendo_deadlock_luego_del_intercambio(){
	pthread_mutex_lock(&lista_bloq_max_mutex);
	CANTIDAD_EN_DEADLOCK = list_size(lista_bloqueados_cant_max_alcanzada);
	pthread_mutex_unlock(&lista_bloq_max_mutex);

	if(CANTIDAD_EN_DEADLOCK > 0){
		list_destroy_and_destroy_elements(semaforos_deadlock, destruir_semaforos_deadlock);
		chequear_deadlock();
	}else{

		/*
		if(list_size(lista_finalizar) == MAXIMO_ENTRENADORES){
			terminar_team_correctamente();
		}*/
	}
}

