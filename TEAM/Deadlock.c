/*
 * Deadlock.c
 *
 *  Created on: 16 jun. 2020
 *      Author: utnso
 */

#include "Deadlock.h"

/***************DEALOCK***************/

void crear_hilo_para_deadlock(){
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)interbloqueo, NULL);
}

void chequear_deadlock(){
	if(list_size(lista_entrenadores) == 0 && todos_bloqueados_por_cantidad_maxima()){
		log_info(team_logger, "Deadlock detectado");
		sem_post(&hay_interbloqueo);
	}
}

void * interbloqueo(){
	sem_wait(&hay_interbloqueo);
	pthread_mutex_lock(&mutex_deadlock);
	hayDeadlock = false;

	hayDeadlock = list_size(lista_bloqueados_cant_max_alcanzada) > 1;
	pthread_mutex_unlock(&mutex_deadlock);
	CANTIDAD_EN_DEADLOCK = list_size(lista_bloqueados_cant_max_alcanzada);
	log_info(team_logger,"La cantidad de entrenadores en deadlock en %i", CANTIDAD_EN_DEADLOCK);

	inicializar_semaforos_deadlock();
	sem_post(&semaforos_listos);


	if(hayDeadlock){
		t_entrenador* entrenador1 = list_get(lista_bloqueados_cant_max_alcanzada, 0);

		t_entrenador* entrenador2;
		t_objetivo_entrenador* pokemon1;
		pokemon1 = elegir_pokemon_innecesario(entrenador1);
		log_info(team_logger, "Un pokemon innecesario para el entrenador %i es: %s", entrenador1->id,pokemon1->especie);
		for(int i = 1; i < list_size(lista_bloqueados_cant_max_alcanzada); i++){
			entrenador2 = list_get(lista_bloqueados_cant_max_alcanzada, i);
			t_objetivo_entrenador* pokemon2 = buscar_pokemon_objetivo_por_especie(entrenador2->objetivo, pokemon1->especie);
			if(pokemon2 != NULL){
				if(pokemon2->cantidad > 0){
					log_info(team_logger, "Hay deadlock entre %i y %i", entrenador1->id, entrenador2->id);
					break;
				}
			}
		}
		planificar_para_deadlock(entrenador1, entrenador2, pokemon1);
	}

}

void planificar_para_deadlock(t_entrenador* entrenador1, t_entrenador* entrenador2, t_objetivo_entrenador* pokemon){
	sem_wait(&semaforos_listos);
	t_pokemon* pokemon_innecesario = malloc(sizeof(t_pokemon));
	pokemon_innecesario->especie = pokemon->especie;
	pokemon_innecesario->posx = entrenador2->posx;
	pokemon_innecesario->posy = entrenador2->posy;
	entrenador1->objetivo_actual = pokemon_innecesario;

	sacar_entrenador_de_lista_pid(lista_bloqueados_cant_max_alcanzada,entrenador2->id);
	list_add(lista_bloqueados_deadlock, entrenador2);
	log_info(team_logger, "El entrenador %i pasó a la lista de bloqueados esperando deadlock", entrenador2->id);


	sacar_entrenador_de_lista_pid(lista_bloqueados_cant_max_alcanzada,entrenador1->id);
	list_add(lista_listos, entrenador1);
	log_info(team_logger, "El entrenador %i pasó a la lista de listos para resolucion de deadlock", entrenador1->id);

	//hayDeadlock = true;
	//obtener_proximo_ejecucion();
	t_entrenador* entre_a_ejecutar;
	entrenador_en_ejecucion = NULL;
	entre_a_ejecutar = list_remove(lista_listos,0);
	entre_a_ejecutar->estado = EJECUTANDO;

	log_info(team_logger, "Planificando para resolver Deadlock");


	sem_post(&semaforos_deadlock[(int)entre_a_ejecutar->id]);

	/*Estas dos funciones funciones que deberian manejarse en el hilo entrenador*/

	log_info(team_logger, "Soy el entrenador que va a ejecutar para deadlock, mi id es: %d.", entre_a_ejecutar->id);
	/*mover_entrenador_a_otra_posicion(entre_a_ejecutar);
	realizar_intercambio(entrenador1);*/


}

void mover_entrenador_a_otra_posicion(t_entrenador* entrenador1){

	t_entrenador* entrenador2 = list_get(lista_bloqueados_deadlock, 0);

	log_info(team_logger, "El entrenador %i se mueve a la posicion %i %i", entrenador1->id,entrenador2->posx, entrenador2->posy);
		//Primero me muevo por izq
		while(entrenador1->posx < entrenador1->objetivo_actual->posx){
			consumir_un_ciclo_de_cpu();
			entrenador1->posx = entrenador1->posx + 1;
		}

		//Despues me muevo por derecha
		while(entrenador1->posx > entrenador1->objetivo_actual->posx){
			consumir_un_ciclo_de_cpu();
			entrenador1->posx = entrenador1->posx - 1;
		}

		//Despues me muevo por arriba
		while(entrenador1->posy > entrenador1->objetivo_actual->posy){
			consumir_un_ciclo_de_cpu();
			entrenador1->posy = entrenador1->posy - 1;
		}

		//Despues me muevo por abajo
		while(entrenador1->posy < entrenador1->objetivo_actual->posy){
			consumir_un_ciclo_de_cpu();
			entrenador1->posy = entrenador1->posy + 1;
		}

		if(entrenador1->posy == entrenador1->objetivo_actual->posy  && entrenador1->posx == entrenador1->objetivo_actual->posx){
			log_info(team_logger, "LLEGUE A LA POSICION DE ENTRENADOR %i", entrenador2->id);

		}
}

void realizar_intercambio(t_entrenador* entrenador1){

	t_entrenador* entrenador2 = list_get(lista_bloqueados_deadlock, 0); //entrenador con quien hago el intercambio
	t_objetivo_entrenador* pokemon2 = elegir_pokemon_innecesario(entrenador2); //pokemon innecesario de E2

	//pokemon1 es el pokemon innecesario de E1
	t_objetivo_entrenador* pokemon1 = buscar_pokemon_objetivo_por_especie(entrenador1->pokemones, entrenador1->objetivo_actual->especie);

	//Entre los objetivos del E1 busco si esta la especie del innecesario del E2
	t_objetivo_entrenador* pokemon1_nuevo = buscar_pokemon_objetivo_por_especie(entrenador1->objetivo, pokemon2->especie);
	if(pokemon1_nuevo != NULL){ //si es una especie que E1 queria, le resto la cantidad necesitada
		pokemon1_nuevo->cantidad--;
	}
	pokemon1->cantidad--;
	list_add(entrenador1->pokemones, pokemon2); //si no es una especie que yo necesitaba, agrego este pokemon a la de atrapados por E1

	//Entre los objetivos del E2, busco la especie innecesaria del E1
	t_objetivo_entrenador* pokemon2_nuevo = buscar_pokemon_objetivo_por_especie(entrenador2->objetivo, entrenador1->objetivo_actual->especie);
	if(pokemon2_nuevo != NULL){ //si es una especie que necesito disminuyo la cantidad necesitada
		pokemon2_nuevo->cantidad--;
	}
	pokemon2->cantidad--;
	list_add(entrenador2->pokemones, pokemon1); //si no lo necesitaba lo agrego

	for (int ciclo = 0; ciclo < 5; ciclo++){ //cada intercambio consume 5 ciclos de cpu
		consumir_un_ciclo_de_cpu();
	}

	if(objetivo_personal_cumplido(entrenador1)){
		entrenador1->estado = FINALIZANDO;
		list_add(lista_finalizar, entrenador1);
		log_info(team_logger, "El entrenador %i finalizo", entrenador1->id);
	} else {
		list_add(lista_bloqueados_cant_max_alcanzada, entrenador1);
		log_info(team_logger, "El entrenador %i paso a la lista de bloqueados con cantidad maxima", entrenador1->id);
	}

	if(objetivo_personal_cumplido(entrenador2)){
			entrenador2->estado = FINALIZANDO;
			list_add(lista_finalizar, entrenador2);
			log_info(team_logger, "El entrenador %i finalizo", entrenador2->id);
		} else {
			sacar_entrenador_de_lista_pid(lista_bloqueados_deadlock,entrenador2->id);
			list_add(lista_bloqueados_cant_max_alcanzada, entrenador2);
			log_info(team_logger, "El entrenador %i paso a la lista de bloqueados con cantidad maxima", entrenador2->id);
		}

	if(list_size(lista_bloqueados_cant_max_alcanzada) > 0){
		list_clean(lista_bloqueados_deadlock);
		interbloqueo(); //si todavia hay entrenadores bloqueados entre sí, vuelvo a llamar a la función
	} else {
		terminar_team_correctamente();
	}

}

t_objetivo_entrenador* elegir_pokemon_innecesario(t_entrenador* entrenador){
	t_objetivo_entrenador* pokemon_innecesario;
	for (int i = 0; i < list_size(entrenador->pokemones); i++){
		t_objetivo_entrenador* pokemon_en_posesion = list_get(entrenador->pokemones, i);
		t_objetivo_entrenador* pokemon_objetivo = buscar_pokemon_objetivo_por_especie(entrenador->objetivo, pokemon_en_posesion->especie);
		if(pokemon_objetivo != NULL){
			if(pokemon_objetivo->cantidad < 0){
				pokemon_innecesario = pokemon_en_posesion;
				break;
			}
		}else{
			pokemon_innecesario = pokemon_en_posesion;
		}
	}
	return pokemon_innecesario;
}

/***************FIN DE DEALOCK***************/

