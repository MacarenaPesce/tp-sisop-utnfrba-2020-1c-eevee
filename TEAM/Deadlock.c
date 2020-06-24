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
	log_warning(team_logger, "HILO DEADLOCK CREADO");

	while(GLOBAL_SEGUIR){
		pthread_mutex_lock(&lista_entrenadores_mutex);
		int cant_nuevos = list_size(lista_entrenadores);
		pthread_mutex_unlock(&lista_entrenadores_mutex);

		pthread_mutex_lock(&lista_listos_mutex);
		int cant_ready = list_size(lista_listos);
		pthread_mutex_unlock(&lista_listos_mutex);

		if(cant_nuevos == 0 && todos_bloqueados_por_cantidad_maxima() && cant_ready == 0){
			sem_post(&hay_interbloqueo);
			interbloqueo();
			printf("\n");
		}
	}

	return NULL;
}

void interbloqueo(){
	sem_wait(&hay_interbloqueo);

	log_warning(team_logger, "Deadlock detectado");

	pthread_mutex_lock(&lista_bloq_max_mutex);
	CANTIDAD_EN_DEADLOCK = list_size(lista_bloqueados_cant_max_alcanzada);
	pthread_mutex_unlock(&lista_bloq_max_mutex);

	log_info(team_logger,"La cantidad de entrenadores en deadlock es %d", CANTIDAD_EN_DEADLOCK);

	inicializar_semaforos_deadlock();

	pthread_mutex_lock(&mutex_deadlock);
	hayDeadlock = false;

	pthread_mutex_lock(&lista_bloq_max_mutex);
	hayDeadlock = list_size(lista_bloqueados_cant_max_alcanzada) > 1;
	pthread_mutex_unlock(&lista_bloq_max_mutex);

	pthread_mutex_unlock(&mutex_deadlock);

	ver_entre_quienes_hay_deadlock_y_resolverlo();

}

void ver_entre_quienes_hay_deadlock_y_resolverlo(){

	pthread_mutex_lock(&lista_bloq_max_mutex);
	t_entrenador* entrenador1 = list_get(lista_bloqueados_cant_max_alcanzada, 0);
	pthread_mutex_unlock(&lista_bloq_max_mutex);


	t_entrenador* entrenador2;
	t_objetivo_entrenador* pokemon1 = elegir_pokemon_innecesario(entrenador1);

	for(int i = 1; i < list_size(lista_bloqueados_cant_max_alcanzada); i++){

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

	pthread_mutex_lock(&lista_bloq_max_mutex);
	sacar_entrenador_de_lista_pid(lista_bloqueados_cant_max_alcanzada,entrenador2->id);
	pthread_mutex_unlock(&lista_bloq_max_mutex);

	list_add(lista_bloqueados_deadlock, entrenador2);
	log_info(team_logger, "El entrenador %d pasó a la lista de bloqueados esperando resolucion de deadlock", entrenador2->id);

	pthread_mutex_lock(&lista_bloq_max_mutex);
	sacar_entrenador_de_lista_pid(lista_bloqueados_cant_max_alcanzada,entrenador1->id);
	pthread_mutex_unlock(&lista_bloq_max_mutex);

	pthread_mutex_lock(&lista_listos_mutex);
	list_add(lista_listos, entrenador1);
	pthread_mutex_unlock(&lista_listos_mutex);
	log_info(team_logger, "El entrenador %d pasó a la lista de listos para resolucion de deadlock", entrenador1->id);

	t_entrenador* entre_a_ejecutar;
	entrenador_en_ejecucion = NULL;
	pthread_mutex_lock(&lista_listos_mutex);
	entre_a_ejecutar = list_remove(lista_listos,0);
	pthread_mutex_unlock(&lista_listos_mutex);
	entre_a_ejecutar->estado = EJECUTANDO;


	sem_post(&hay_interbloqueo_avisar_a_entrenador);
	t_semaforo_deadlock * sem_entrenador_deadlock = obtener_semaforo_deadlock_por_id(entre_a_ejecutar->id);

	log_info(team_logger, "Haciendo el intercambio");
	printf("\n");

	sem_post(sem_entrenador_deadlock->semaforo);


	//ESPERAR A ENTRENADOR
	sem_wait(&aviso_entrenador_hizo_intercambio);

	verificar_si_entrenador_sigue_bloqueado(entrenador1);
	verificar_si_entrenador_sigue_bloqueado(entrenador2);

	verificar_si_sigue_habiendo_deadlock_luego_del_intercambio();

}

void verificar_si_entrenador_sigue_bloqueado(t_entrenador* entrenador){

	if(objetivo_personal_cumplido(entrenador)){

		sacar_entrenador_de_lista_pid(lista_bloqueados_deadlock,entrenador->id);

		pthread_mutex_lock(&lista_bloq_max_mutex);
		sacar_entrenador_de_lista_pid(lista_bloqueados_cant_max_alcanzada,entrenador->id);
		pthread_mutex_unlock(&lista_bloq_max_mutex);


		pthread_mutex_lock(&lista_listos_mutex);
		sacar_entrenador_de_lista_pid(lista_listos,entrenador->id);
		pthread_mutex_unlock(&lista_listos_mutex);

		list_add(lista_finalizar, entrenador);

		sem_post(&array_semaforos_finalizar[entrenador->id]);
		log_info(team_logger, "El entrenador %d finalizo HILO DEADLOCK", entrenador->id);

	} else {
		sacar_entrenador_de_lista_pid(lista_bloqueados_deadlock,entrenador->id);

		pthread_mutex_lock(&lista_bloq_max_mutex);
		list_add(lista_bloqueados_cant_max_alcanzada, entrenador);
		pthread_mutex_unlock(&lista_bloq_max_mutex);

		log_info(team_logger, "El entrenador %d paso a la lista de bloqueados con cantidad maxima", entrenador->id);
	}
}

void verificar_si_sigue_habiendo_deadlock_luego_del_intercambio(){
	pthread_mutex_lock(&lista_bloq_max_mutex);
	CANTIDAD_EN_DEADLOCK = list_size(lista_bloqueados_cant_max_alcanzada);
	pthread_mutex_unlock(&lista_bloq_max_mutex);


	if(CANTIDAD_EN_DEADLOCK > 0){
		list_clean(lista_bloqueados_deadlock);
		list_clean(lista_listos);
		list_destroy_and_destroy_elements(semaforos_deadlock, destruir_semaforos_deadlock);

		sem_post(&hay_interbloqueo);

	} else {
		terminar_team_correctamente();
	}
}

