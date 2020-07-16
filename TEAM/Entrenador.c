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
			sacar_entrenador_de_lista_pid(lista_bloqueados_esperando, entrenador->id);
			list_add(lista_finalizar, entrenador);
			entrenador->estado = FINALIZANDO;
			entrenador_en_ejecucion = NULL;
			log_debug(team_logger, "El entrenador %d finalizo", entrenador->id);
			log_info(team_logger_oficial, "El entrenador %d finalizo", entrenador->id);
			sem_post(&todos_los_entrenadores_finalizaron);
			sem_post(&orden_para_planificar);
			return NULL;
		}
		sem_post(&termine_carajo);
		if(entrenador->cant_maxima_objetivos == 0){
			break;
		}
	}

	realizar_las_operaciones_de_deadlock(entrenador);

	return NULL;

}

void realizar_las_operaciones_de_deadlock(t_entrenador * entrenador){
	//HAY INTERBLOQUEO
	while(GLOBAL_SEGUIR){
		sem_wait(&array_semaforos_deadlock[entrenador->id]);
		
		if(!objetivo_personal_cumplido(entrenador)){
			log_info(team_logger, "Soy el entrenador que va a ejecutar para resolver el deadlock, mi id es: %d.", entrenador->id);

			mover_entrenador_a_otra_posicion(entrenador);
			realizar_intercambio(entrenador);

			/*Si luego de realizar el intercambio cumplio su objetivo personal, sale del while*/
			if(objetivo_personal_cumplido(entrenador)){
				break;
			}

		}else{
			break;
		}
		
	}

	sem_wait(&array_semaforos_finalizar[entrenador->id]);
	log_debug(team_logger, "El entrenador %d finalizo", entrenador->id);
	log_info(team_logger_oficial, "El entrenador %d finalizo", entrenador->id);
	sem_post(&todos_los_entrenadores_finalizaron);
	sem_post(&puedo_volver_a_ejecutar);

}

void llegar_a_el_pokemon(t_entrenador * entrenador){

	/*Cada movimiento en el mapa responderá a un ciclo de CPU, y este NO realizará movimientos diagonales para llegar a la posición deseada.
	 * Para simular más a la realidad esta funcionalidad, se deberá agregar un retardo de X segundos configurado por archivo de configuración.*/

	log_info(team_logger_oficial, "El entrenador %i se mueve a atrapar a %s a la posicion %i %i", 
	entrenador->id, entrenador->objetivo_actual->especie,entrenador->objetivo_actual->posx, entrenador->objetivo_actual->posy);
	log_info(team_logger, "El entrenador %i se mueve a atrapar a %s a la posicion (%i, %i)", 
	entrenador->id, entrenador->objetivo_actual->especie,entrenador->objetivo_actual->posx, entrenador->objetivo_actual->posy);

		//Primero me muevo por izq
		while(entrenador->posx < entrenador->objetivo_actual->posx){
			consumir_un_ciclo_de_cpu_mientras_planificamos(entrenador);
			entrenador->posx = entrenador->posx + 1;
		}

		//Despues me muevo por derecha
		while(entrenador->posx > entrenador->objetivo_actual->posx){
			consumir_un_ciclo_de_cpu_mientras_planificamos(entrenador);
			entrenador->posx = entrenador->posx - 1;
		}

		//Despues me muevo por arriba
		while(entrenador->posy > entrenador->objetivo_actual->posy){
			consumir_un_ciclo_de_cpu_mientras_planificamos(entrenador);
			entrenador->posy = entrenador->posy - 1;
		}

		//Despues me muevo por abajo
		while(entrenador->posy < entrenador->objetivo_actual->posy){
			consumir_un_ciclo_de_cpu_mientras_planificamos(entrenador);
			entrenador->posy = entrenador->posy + 1;
		}

		if(entrenador->posy == entrenador->objetivo_actual->posy  && entrenador->posx == entrenador->objetivo_actual->posx){
			log_info(team_logger, "El entrenador de id %d llegó al pokemon %s.", entrenador->id, entrenador->objetivo_actual->especie);
		}
}

void atrapar(t_entrenador * entrenador){
	/*Para esto, se deben ejecutar los siguientes pasos:
1. Enviar el mensaje a la cola de mensajes CATCH_POKEMON indicando cual es la especie del Pokémon y la posición del mismo.
2. Obtener el ID del mensaje anterior desde el Broker y guardarlo a la espera de la llegada de la respuesta en CAUGHT_POKEMON.
3. Bloquear al entrenador en cuestión a la espera del resultado del mensaje. Este entrenador no podrá volver a ejecutar hasta que se reciba el resultado.
En caso que el Broker no se encuentre funcionando o la conexión inicial falle, se deberá tomar como comportamiento Default que el Pokémon ha sido 
atrapado con éxito.*/

	t_catch_pokemon* catch_pokemon = malloc(sizeof(t_catch_pokemon));
	catch_pokemon->pokemon = entrenador->objetivo_actual->especie;
	catch_pokemon->coordenadas.posx = entrenador->objetivo_actual->posx;
	catch_pokemon->coordenadas.posy = entrenador->objetivo_actual->posy;

	t_servidor * servidor = malloc(sizeof(t_servidor));
	servidor->ip = ip_broker;
	servidor->puerto = puerto_broker;
	servidor->id_cliente = id;

	if(!fijarme_si_debo_atraparlo_usando_el_objetivo_global(entrenador->objetivo_actual->especie)){
		log_warning(team_logger, "No necesitamos mas de esta especie %s", entrenador->objetivo_actual->especie);
		entrenador->razon_de_bloqueo = ESPERANDO_POKEMON;
		bloquear_entrenador(entrenador);
		return;
	}

	consumir_un_ciclo_de_cpu_mientras_planificamos(entrenador);

	 t_packed* ack = enviar_catch_pokemon(servidor, -1, catch_pokemon);

	log_info(team_logger, "El entrenador %d hizo el pedido de catch pokemon para esta especie: %s", entrenador->id, entrenador->objetivo_actual->especie);

	if(ack == (t_packed*) -1){
		log_info(team_logger, "El broker esta caído, pasamos a hacer el procedimiento default para atrapar un pokemon");
		hacer_procedimiento_para_atrapar_default(catch_pokemon, entrenador);
	}else{
		//Recibo ACK
		if(ack->operacion == ACK){
			log_info(team_logger, "Confirmada recepcion del pedido CATCH para el pokemon: %s\n", entrenador->objetivo_actual->especie);
			log_debug(team_logger, "ID DE MENSAJE CATCH: %d\n", ack->id_mensaje);

			t_mensaje_guardado * mensaje = malloc(sizeof(t_mensaje_guardado));
			mensaje->id = ack->id_mensaje;
			mensaje->operacion = CATCH;
			mensaje->contenido = catch_pokemon;

			pthread_mutex_lock(&mensaje_chequear_id_mutex);
			list_add(mensajes_para_chequear_id, mensaje);
			pthread_mutex_unlock(&mensaje_chequear_id_mutex);

			hacer_procedimiento_para_atrapar_pokemon_con_broker(entrenador);

			sem_wait(&array_semaforos_caught[entrenador->id]);

			log_info(team_logger, "Soy el entrenador %d de nuevo, y voy a atrapar al pokemon para el cual esperaba el OK y me lo dieron!", entrenador->id);
			log_info(team_logger, "El pokemon %s ha sido atrapado con exito", catch_pokemon->pokemon);

			actualizar_mapa_y_entrenador(catch_pokemon, entrenador);
		}
	}

	free(servidor);
	free(catch_pokemon);

	return;

}

void mover_entrenador_a_otra_posicion(t_entrenador* entrenador1){

	pthread_mutex_lock(&lista_comun_deadlock);
	t_entrenador* entrenador2 = list_get(lista_bloqueados_deadlock, 0);
	pthread_mutex_unlock(&lista_comun_deadlock);

	log_info(team_logger, "El entrenador %d se mueve a la posicion de coordenadas (%d, %d)", entrenador1->id,entrenador2->posx, entrenador2->posy);
	log_info(team_logger_oficial, "El entrenador %d se mueve a la posicion de coordenadas (%d, %d)", entrenador1->id,entrenador2->posx, entrenador2->posy);
	
	//Primero me muevo por izq
	while(entrenador1->posx < entrenador1->objetivo_actual->posx){
		consumir_un_ciclo_de_cpu(entrenador1);
		entrenador1->posx = entrenador1->posx + 1;
	}

	//Despues me muevo por derecha
	while(entrenador1->posx > entrenador1->objetivo_actual->posx){
		consumir_un_ciclo_de_cpu(entrenador1);
		entrenador1->posx = entrenador1->posx - 1;
	}

	//Despues me muevo por arriba
	while(entrenador1->posy > entrenador1->objetivo_actual->posy){
		consumir_un_ciclo_de_cpu(entrenador1);
		entrenador1->posy = entrenador1->posy - 1;
	}

	//Despues me muevo por abajo
	while(entrenador1->posy < entrenador1->objetivo_actual->posy){
		consumir_un_ciclo_de_cpu(entrenador1);
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
		//log_info(team_logger, "Un pokemon en posesion de %i es %s", entrenador->id, pokemon_en_posesion->especie);
		t_objetivo_entrenador* pokemon_objetivo = buscar_pokemon_objetivo_por_especie(entrenador->objetivo, pokemon_en_posesion->especie);

		int cantidad_en_posesion = pokemon_en_posesion->cantidad;

		if(pokemon_objetivo == NULL && cantidad_en_posesion > 0){
			//log_info(team_logger, "No necesito nada de esta especie y tengo varios");
			pokemon_innecesario = pokemon_en_posesion;
			break;
		} else if(pokemon_objetivo == NULL && cantidad_en_posesion == 0){
			//log_info(team_logger, "Ya no tengo mas pokes de esta especie no necesitada");
		}

		else if(pokemon_objetivo != NULL){
			int cantidad_objetivo = pokemon_objetivo->cantidad;
			if(cantidad_objetivo >= 0){
				//log_info(team_logger, "Todavia necesito de esta especie o tengo justito");
			} else {
				pokemon_innecesario = pokemon_en_posesion;
				pokemon_objetivo->cantidad++;
				break;
			}
		}
	}
	return pokemon_innecesario;
}

t_objetivo_entrenador * elegir_pokemon_innecesario_util(t_entrenador * entrenador2, t_entrenador * entrenador1){
	t_objetivo_entrenador* pokemon_innecesario;
	bool encontre_uno_util = false;
	for (int i = 0; i < list_size(entrenador2->pokemones); i++){
		t_objetivo_entrenador* pokemon_en_posesion = list_get(entrenador2->pokemones, i);
		t_objetivo_entrenador* pokemon_objetivo = buscar_pokemon_objetivo_por_especie(entrenador2->objetivo, pokemon_en_posesion->especie);

		int cantidad_en_posesion = pokemon_en_posesion->cantidad;

		if(pokemon_objetivo == NULL && cantidad_en_posesion > 0){
			//log_info(team_logger, "No necesito nada de esta especie y tengo varios");			
			for(int j = 0; j < list_size(entrenador1->objetivo); j++){
				t_objetivo_entrenador * objetivo_de_uno = list_get(entrenador1->objetivo, j);
				if(string_equals_ignore_case(objetivo_de_uno->especie, pokemon_en_posesion->especie)){
					pokemon_innecesario = pokemon_en_posesion;
					encontre_uno_util = true;
				}
			}
			if(encontre_uno_util){
				break;
			}
		} else if(pokemon_objetivo == NULL && cantidad_en_posesion == 0){
			//log_info(team_logger, "Ya no tengo mas pokes de esta especie no necesitada");
		}

		else if(pokemon_objetivo != NULL){
			int cantidad_objetivo = pokemon_objetivo->cantidad;
			if(cantidad_objetivo >= 0){
				//log_info(team_logger, "Todavia necesito de esta especie o tengo justito");
			} else {
				for(int j = 0; j < list_size(entrenador1->objetivo); j++){
					t_objetivo_entrenador * objetivo_de_uno = list_get(entrenador1->objetivo, j);
					if(objetivo_de_uno->especie == pokemon_en_posesion->especie){
						pokemon_innecesario = pokemon_en_posesion;
						encontre_uno_util = true;
					}
				}
				if(encontre_uno_util){
					pokemon_objetivo->cantidad++;
					break;
				}	
			}
		}
	}
	if(!encontre_uno_util){
		pokemon_innecesario = elegir_pokemon_innecesario(entrenador2);
	}
	return pokemon_innecesario;
}

void realizar_intercambio(t_entrenador* entrenador1){

	for (int ciclo = 0; ciclo < 5; ciclo++){ //cada intercambio consume 5 ciclos de cpu
		consumir_un_ciclo_de_cpu(entrenador1);
	}

	pthread_mutex_lock(&lista_comun_deadlock);
	t_entrenador* entrenador2 = list_get(lista_bloqueados_deadlock, 0); //entrenador con quien hago el intercambio
	pthread_mutex_unlock(&lista_comun_deadlock);

	log_info(team_logger_oficial, "Se va a realizar un intercambio entre el entrenador %i y %i", entrenador1->id, entrenador2->id);

	t_objetivo_entrenador* pokemon2 = elegir_pokemon_innecesario_util(entrenador2, entrenador1); //pokemon innecesario de E2 que le sirva a E1

	//pokemon1 es el pokemon innecesario de E1
	t_objetivo_entrenador* pokemon1 = buscar_pokemon_objetivo_por_especie(entrenador1->pokemones, entrenador1->objetivo_actual->especie);

	//Entre los objetivos del E1 busco si esta la especie del innecesario del E2
	t_objetivo_entrenador* pokemon1_nuevo = buscar_pokemon_objetivo_por_especie(entrenador1->objetivo, pokemon2->especie);
	if(pokemon1_nuevo != NULL){ //si es una especie que E1 queria, le resto la cantidad necesitada
		pokemon1_nuevo->cantidad--;
	}

	pokemon1->cantidad--;
	 //si no es una especie que yo necesitaba, agrego este pokemon a la de atrapados por E1
	agregar_objetivo(pokemon2->especie, 1, entrenador1->pokemones);
	t_objetivo_entrenador* pokemon2_inservible = buscar_pokemon_objetivo_por_especie(entrenador1->pokemones, pokemon2->especie);
	pokemon2_inservible->cantidad++;

	//Entre los objetivos del E2, busco la especie innecesaria del E1
	t_objetivo_entrenador* pokemon2_nuevo = buscar_pokemon_objetivo_por_especie(entrenador2->objetivo, entrenador1->objetivo_actual->especie);
	if(pokemon2_nuevo != NULL){ //si es una especie que necesito disminuyo la cantidad necesitada
		pthread_mutex_lock(&tocando_pokemones_objetivos);
		pokemon2_nuevo->cantidad--;
		pthread_mutex_unlock(&tocando_pokemones_objetivos);
	}
	pokemon2->cantidad--;
	//si no lo necesitaba lo agrego
	agregar_objetivo(pokemon1->especie, 1, entrenador2->pokemones);
	t_objetivo_entrenador* pokemon1_inservible = buscar_pokemon_objetivo_por_especie(entrenador2->pokemones, pokemon1->especie);
	pokemon1_inservible->cantidad++;

	log_info(team_logger, "Se intercambiaron los pokemones %s y %s entre los entrenadores %d y %d", pokemon1->especie, pokemon2->especie, entrenador1->id, entrenador2->id);

	pthread_mutex_lock(&lista_comun_deadlock);
	sacar_entrenador_de_lista_pid(lista_bloqueados_deadlock,entrenador2->id);
	pthread_mutex_unlock(&lista_comun_deadlock);

	//AVISAR A DEADLOCK
	sem_post(&aviso_entrenador_hizo_intercambio);

}
