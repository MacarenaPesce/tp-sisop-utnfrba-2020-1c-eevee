/*
 * Entrenador.c
 *
 *  Created on: 23 jun. 2020
 *      Author: utnso
 */

#include "Entrenador.h"

void * jugar_con_el_entrenador(t_entrenador * entrenador){

	while(GLOBAL_SEGUIR){
		sem_wait(&array_semaforos[entrenador->id]);
		log_info(team_logger, "Soy el entrenador que va a ejecutar, mi id es: %d.", entrenador->id);

		llegar_a_el_pokemon(entrenador);
		atrapar(entrenador);

		if(objetivo_personal_cumplido(entrenador)){
			list_add(lista_finalizar, entrenador);
			log_warning(team_logger, "El entrenador %d finalizo", entrenador->id);
			return NULL;
		}

		if(entrenador->cant_maxima_objetivos == 0){
			break;
		}
	}

	//HAY INTERBLOQUEO
	sem_wait(&hay_interbloqueo);

	t_semaforo_deadlock * sem_entrenador_deadlock = obtener_semaforo_deadlock_por_id(entrenador->id);

	if(sem_entrenador_deadlock == NULL){
		sem_wait(&array_semaforos_finalizar[entrenador->id]);
		sem_post(&hay_interbloqueo);

		log_warning(team_logger, "El entrenador %d finalizo", entrenador->id);
		return NULL;

	}else{
		sem_wait(sem_entrenador_deadlock->semaforo);
		log_info(team_logger, "Soy el entrenador que va a ejecutar para resolver el deadlock, mi id es: %d.", entrenador->id);
		mover_entrenador_a_otra_posicion(entrenador);
		realizar_intercambio(entrenador);

		sem_wait(&array_semaforos_finalizar[entrenador->id]);

		log_warning(team_logger, "El entrenador %d finalizo", entrenador->id);
		return NULL;
	}

}

void llegar_a_el_pokemon(t_entrenador * entrenador){
	/*Cada movimiento en el mapa responderá a un ciclo de CPU, y este NO realizará movimientos diagonales para llegar a la posición deseada.
	 * Para simular más a la realidad esta funcionalidad, se deberá agregar un retardo de X segundos configurado por archivo de configuración.*/

	log_info(team_logger_oficial, "El entrenador %i se mueve a atrapar a %s a la posicion %i %i", entrenador->id, entrenador->objetivo_actual->especie,entrenador->objetivo_actual->posx, entrenador->objetivo_actual->posy);

	/************************************FIFO************************************/

	if((!strcmp(algoritmo_planificacion, "FIFO"))) {

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

	/************************************ROUND ROBIN************************************

	else if((!strcmp(algoritmo_planificacion, "RR"))){
		log_info(team_logger_oficial, "El entrenador %i se mueve a atrapar a %s a la posicion %i %i", entrenador->id, entrenador->objetivo_actual->especie,entrenador->objetivo_actual->posx, entrenador->objetivo_actual->posy);
		//Primero me muevo por izq
		while(entrenador->posx < entrenador->objetivo_actual->posx){
			if(quantum_actual == 0){
				entrenador_desalojado = entrenador;
				sem_post(&orden_para_planificar);
			}
				consumir_un_ciclo_de_cpu();
				entrenador->posx = entrenador->posx + 1;
				log_info(team_logger, "Soy %i Me movi una posición por izq", entrenador->id);
				quantum_actual--;
			}

		//Despues me muevo por derecha
		while(entrenador->posx > entrenador->objetivo_actual->posx){
			if(quantum_actual == 0){
				entrenador_desalojado = entrenador;
				sem_post(&orden_para_planificar);
			}
			consumir_un_ciclo_de_cpu();
			entrenador->posx = entrenador->posx - 1;
			log_info(team_logger, "Soy %i Me movi una posición por der", entrenador->id);
			quantum_actual--;
		}

		//Despues me muevo por arriba
		while(entrenador->posy > entrenador->objetivo_actual->posy){
			if(quantum_actual == 0){
				entrenador_desalojado = entrenador;
				sem_post(&orden_para_planificar);
			}
			consumir_un_ciclo_de_cpu();
			entrenador->posy = entrenador->posy - 1;
			log_info(team_logger, "Soy %i Me movi una posición arriba", entrenador->id);
			quantum_actual--;
		}

		//Despues me muevo por abajo
		while(entrenador->posy < entrenador->objetivo_actual->posy){
			if(quantum_actual == 0){
				entrenador_desalojado = entrenador;
				sem_post(&orden_para_planificar);
			}
			consumir_un_ciclo_de_cpu();
			entrenador->posy = entrenador->posy + 1;
			log_info(team_logger, "Soy %i Me movi una posición abajo", entrenador->id);
			quantum_actual--;
		}

		if(entrenador->posy == entrenador->objetivo_actual->posy  && entrenador->posx == entrenador->objetivo_actual->posx){
			log_info(team_logger, "El entrenador de id %d llegó al pokemon %s.", entrenador->id, entrenador->objetivo_actual->especie);
			atrapar(entrenador);
		} else {
			sem_post(&orden_para_planificar);
		}

	}
*/
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
			log_info(team_logger, "Confirmada recepcion del pedido CATCH para el pokemon: %s\n", entrenador->objetivo_actual->especie);
			log_info(team_logger, "EL ID DEL MENSAJE ES: %d\n", ack->id_mensaje);

			t_mensaje_guardado * mensaje = malloc(sizeof(t_mensaje_guardado));
			mensaje->id = ack->id_mensaje;
			mensaje->operacion = CATCH;
			mensaje->contenido = catch_pokemon;

			pthread_mutex_lock(&mensaje_chequear_id_mutex);
			list_add(mensajes_para_chequear_id, mensaje);
			pthread_mutex_unlock(&mensaje_chequear_id_mutex);

			hacer_procedimiento_para_atrapar_pokemon_con_broker(entrenador);
		}
	}

	free(servidor);

}

void mover_entrenador_a_otra_posicion(t_entrenador* entrenador1){

	t_entrenador* entrenador2 = malloc(sizeof(t_entrenador));
	entrenador2 = list_get(lista_bloqueados_deadlock, 0);

	log_info(team_logger, "El entrenador %d se mueve a la posicion de coordenadas (%d, %d)", entrenador1->id,entrenador2->posx, entrenador2->posy);
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
		log_info(team_logger, "El entrenador de id %d llegó a la posicion del entrenador de id %d", entrenador1->id, entrenador2->id);

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


void realizar_intercambio(t_entrenador* entrenador1){

	for (int ciclo = 0; ciclo < 5; ciclo++){ //cada intercambio consume 5 ciclos de cpu
		consumir_un_ciclo_de_cpu();
	}

	t_entrenador* entrenador2 = malloc(sizeof(t_entrenador));
	entrenador2 = list_get(lista_bloqueados_deadlock, 0); //entrenador con quien hago el intercambio
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

	log_info(team_logger, "Se intercambiaron los pokemones %s y %s entre los entrenadores %d y %d", pokemon1->especie, pokemon2->especie, entrenador1->id, entrenador2->id);

	sacar_entrenador_de_lista_pid(lista_bloqueados_deadlock,entrenador2->id);

	//AVISAR A DEADLOCK
	sem_post(&aviso_entrenador_hizo_intercambio);

}