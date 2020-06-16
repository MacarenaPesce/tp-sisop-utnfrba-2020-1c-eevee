/*
 * Funciones_para_deadlock.c
 *
 *  Created on: 14 jun. 2020
 *      Author: utnso
 */

#include "Funciones_para_deadlock.h"
/*


int bloquear_pokemon(char* pokemon , int pid){
	t_pokemones_bloqueados * pokemon_bloqueado;

	if((pokemon_bloqueado = buscar_pokemon_bloqueado(pokemon)) == NULL){
		pokemon_bloqueado = malloc(sizeof(t_pokemones_bloqueados));

		memset(pokemon_bloqueado, 0, sizeof(t_pokemones_bloqueados));

		pokemon_bloqueado->pokemon = strdup(pokemon);
		pokemon_bloqueado->pid = pid;

		list_add(pokemones_bloqueados, pokemon_bloqueado);

		log_info(team_logger, "Se creó el pokemon bloqueado %s",pokemon);

		//Busco si existe el entrenador en el sistema
		if((entrenador_en_ejecucion != NULL  && entrenador_en_ejecucion->id == pid) || 	// EJEC
		  (buscar_entrenador_en_lista_pid(lista_listos, pid) ) )						// READY
		{
			log_info(team_logger,"Se asigna el pokemon al entrenador %d\n",pid);
		}else{
			pokemon_bloqueado->pid = -1;
			if(pid>-1)
				log_info(team_logger, "No existe el entrenador de ID %d en READY ni en EJECUCION, se asigna el pokemon a sistema\n",pid);
			else
				log_info(team_logger,"Se asigna el pokemon a sistema\n");
		}
	}
	else{
		log_info(team_logger, "EL pokemon %s ya está bloqueada! No se agrega a la lista",pokemon);
		if(pokemon_bloqueado->pid == pid){
			log_info(team_logger,"El pokemon %s ya está asignada al entrenador %d\n",pokemon,pid);
			return 0;
		}
		else if(pokemon_bloqueado->pid > -1)
			log_info(team_logger,"El pokemon %s está asignada al entrenador %d\n",pokemon,pokemon_bloqueado->pid);
		else
			log_info(team_logger,"El pokemon %s está asignada a sistema\n",pokemon);

		return 1;
	}

	return 0;
}

int desbloquear_pokemon(char* pokemon){
	bool is_pokemon_bloqueado(t_pokemones_bloqueados * pokemon_bloqueado){
		return ((!strcmp(pokemon, pokemon_bloqueado->pokemon)));
	}

	t_pokemones_bloqueados * pokemon_bloqueado = buscar_pokemon_bloqueado(pokemon);

	if(pokemon_bloqueado == NULL){
		log_info(team_logger,"El pokemon %s no está bloqueada\n",pokemon);
		return 1;
	}

	//Busco el primer entrenador bloqueado por el pokemon a desbloquear
	t_entrenador * entrenador_a_desbloquear = buscar_entrenador_bloqueado_por_pokemon(pokemon);
	if(entrenador_a_desbloquear!=NULL){
		//Si existe, lo remuevo de los bloqueados
		entrenador_a_desbloquear = sacar_entrenador_bloqueado_por_pokemon(pokemon);

		log_info(team_logger,"El entrenador %d estaba bloqueado por el pokemon %s, se pasa a ready",entrenador_a_desbloquear->id,pokemon);

		free(entrenador_a_desbloquear->pokemon_bloqueo);
		entrenador_a_desbloquear->pokemon_bloqueo = NULL;
		entrenador_a_desbloquear->estado = READY;
		/*entrenador_a_desbloquear->tiempo_espera = 0;
		estimar_entrenador(entrenador_a_desbloquear);

		if(!strcmp(config.algoritmo, "SJF-CD"))
		{
			if((entrenador_en_ejecucion!=NULL) && (entrenador_a_desbloquear->estimacion_real < entrenador_en_ejecucion->estimacion_actual))
			{
				logger_planificador(escribir_loguear,l_info,"El entrenador desbloqueado de PID %d debe desalojar al entrenador en ejecucion!",entrenador_a_desbloquear->pid);
				desalojo_en_ejecucion++;
				entrenador_por_desalojar = entrenador_a_desbloquear;
				logger_planificador(escribir_loguear,l_info,"Esperando a que entrenador %d termine su sentencia\n",entrenador_en_ejecucion->pid);
			}
		}

		//Lo agrego a Ready
		list_add(entrenador_listos,entrenador_a_desbloquear);


		//Busco si hay otro entrenador bloqueado por la misma pokemon
		//Si no existe otro entrenador bloqueado por esa pokemon, hay que eliminar El pokemon de la lista de pokemons bloqueadas
		if(buscar_entrenador_bloqueado_por_pokemon(pokemon)==NULL)
		{
			logger_planificador(escribir_loguear,l_info,"No hay otro entrenador bloqueados por El pokemon %s, se elimina de la lista de pokemons bloqueadas\n",pokemon);
			list_remove_and_destroy_by_condition(pokemones_bloqueados,(void*)is_pokemon_bloqueado,(void*)destruir_pokemon_bloqueado);
			return 2;
		}
	}
	else
	{
		//Si no hay entrenador bloqueado por esa pokemon, solamente lo elimino de las pokemons bloqueadas
		logger_planificador(escribir_loguear,l_info,"No hay ningun entrenadors bloqueados por El pokemon %s, se elimina de la lista de pokemons bloqueadas\n",pokemon);
		list_remove_and_destroy_by_condition(pokemones_bloqueados,(void*)is_pokemon_bloqueado,(void*)destruir_pokemon_bloqueado);
		return 2;
	}

	return 0;
}

t_pokemones_bloqueados * buscar_pokemon_bloqueado(char* pokemon){
	bool is_pokemon_bloqueado(t_pokemones_bloqueados * pokemon_bloqueado){
		return ((!strcmp(pokemon, pokemon_bloqueado->pokemon)));
	}

	return (list_find(pokemones_bloqueados,(void*)is_pokemon_bloqueado));
}

int destruir_pokemon_bloqueado(t_pokemones_bloqueados * pokemon_bloqueado){
	free(pokemon_bloqueado->pokemon);
	free(pokemon_bloqueado);

	return 0;
}

void desbloquear_pokemones_bloqueados_pid(int pid){
	t_list * pokemones_bloqueados_aux;

	bool is_bloqueado_pid(t_pokemones_bloqueados * pokemon){
		return(pokemon->pid==pid);
	}

	void desbloquear_recursos(t_pokemones_bloqueados * pokemon){
		//Si desbloqueo El pokemon, pero no la destruyó de la lista, detruyo el local
		if(!desbloquear_pokemon(pokemon->pokemon))
			destruir_pokemon_bloqueado(pokemon);
	}

	pokemones_bloqueados_aux = list_filter(pokemones_bloqueados, (void*)is_bloqueado_pid);

	if(!list_is_empty(pokemones_bloqueados_aux)){
		log_info(team_logger,"El entrenador %d tiene recursos tomados, liberando pokemons...",pid);
		list_iterate(pokemones_bloqueados_aux,(void*)desbloquear_recursos);
	}

	list_destroy(pokemones_bloqueados_aux);

	return;
}


void mostrar_bloqueos(void){
	t_list * lista;

	lista = list_create();
	list_add_all(lista,pokemones_bloqueados);

	printf("\nEstado actual de la lista de pokemons bloqueadas: \n\n");

	list_iterate(lista,(void*)mostrar_pokemon_bloqueado);

	printf("\nTamaño: %d \n",list_size(lista));

	list_destroy(lista);
	printf("\n");

	return;
}

void mostrar_pokemon_bloqueado(t_pokemones_bloqueados * pokemon_bloqueado){
	printf("pokemon Bloqueada: %s\n", pokemon_bloqueado->pokemon);
	printf("PID entrenador que la tiene asignada: %d\n", pokemon_bloqueado->pid);
	printf("\n");

	return;
}

int bloquear_entrenador_pid(char * pokemon,int pid)
{
	if(entrenador_en_ejecucion != NULL  && entrenador_en_ejecucion->pid == pid)
	{
		/* Si el entrenador esta en ejecucion, esperar a que termine la instrucción y desalojar

		logger_planificador(escribir_loguear,l_info,"Esperando a que termine de ejecutar la sentencia\n");
		if(entrenador_en_ejecucion->pokemon_bloqueo!=NULL)
		{
			free(entrenador_en_ejecucion->pokemon_bloqueo);
			entrenador_en_ejecucion->pokemon_bloqueo = NULL;
		}

		entrenador_en_ejecucion->pokemon_bloqueo = strdup(pokemon);

		bloqueo_en_ejecucion++;
	}
	else if (buscar_entrenador_en_lista_pid(entrenador_listos, pid) )
	{
		// Si el entrenador está en la lista de listos, hay que pasarlo a bloqueado
		t_entrenador * entrenador_aux = sacar_entrenador_de_lista_pid(entrenador_listos,pid);

		entrenador_aux->pokemon_bloqueo = strdup(pokemon);
		entrenador_aux->estado = bloqueado;

		list_add(entrenador_bloqueados,entrenador_aux);

		logger_planificador(escribir_loguear,l_info,"El entrenador %d estaba en listos, se pasó a bloqueados",pid);
	}
	else
	{
		logger_planificador(escribir_loguear,l_info,"No existe el entrenador de ID %d en READY ni en EJECUCION",pid);
		return 1;
	}

	return 0;
}

t_entrenador * buscar_entrenador_bloqueado_por_pokemon(char* pokemon){
	bool is_entrenador_bloqueado(t_entrenador * entrenador){
		return ((!strcmp(pokemon, entrenador->pokemon_bloqueo)));
	}

	return(list_find(lista_bloqueados,(void*)is_entrenador_bloqueado));
}

t_entrenador * sacar_entrenador_bloqueado_por_pokemon(char* pokemon){
	bool is_entrenador_bloqueado(t_entrenador * entrenador){
		return ((!strcmp(pokemon, entrenador->pokemon_bloqueo)));
	}

	return(list_remove_by_condition(lista_bloqueados,(void*)is_entrenador_bloqueado));
}

t_list* entrenadores_bloqueados_por_pokemon(char* recurso){

	bool is_entrenador_bloqueado_por_pokemon(t_entrenador* entrenador){
		return !strcmp(recurso, entrenador->pokemon_bloqueo);
	}

	return list_filter(lista_bloqueados, (void*) is_entrenador_bloqueado_por_pokemon);
}


void consultar_deadlock(void){
	/* deadlock: Esta consola también permitirá analizar los deadlocks que existan en el
	 * sistema y a que entrenador están asociados.
	 * Pudiendo resolverlos manualmente con la sentencia de kill previamente descrita.


	int tam_block = list_size(lista_bloqueados);
	int analizar= 0;
	t_entrenador * entrenador_aux = NULL;
	t_entrenador * entrenador_analizado = NULL;
	t_list * pokemones_bloqueados_aux;
	t_pokemones_bloqueados * pokemon_analizado;
	t_pokemones_bloqueados * pokemon_aux;

	t_deadlock * entrenador_inicial;
	t_deadlock * entrenador_aux_deadlock;

	t_list * deadlock;
	t_list * lista_deadlocks;

	int seguir_buscando = 0;

	lista_deadlocks = list_create();

	for(int i=0;i<tam_block;i++){
		entrenador_analizado = list_get(lista_bloqueados,i);
		entrenador_aux = entrenador_analizado;

		//Si el entrenador ya se encuentra en un deadlock, no lo analizo
		analizar = buscar_entrenador_en_deadlock(lista_deadlocks, entrenador_analizado->id);
		if(analizar){
			log_info(team_logger, "No se analiza el entrenador %d porque ya esta en un deadlock",entrenador_analizado->id);
			continue;
		}

		deadlock = list_create();

		entrenador_inicial = malloc(sizeof(t_deadlock));
		memset(entrenador_inicial,0,sizeof(t_deadlock));
		entrenador_inicial->pid = entrenador_aux->id;
		entrenador_inicial->pokemon_pedido = strdup(entrenador_aux->pokemon_bloqueo);

		list_add(deadlock,entrenador_inicial);

		log_info(team_logger,"Analizando al entrenador %d",entrenador_analizado->id);
		seguir_buscando++;

		while(seguir_buscando){

			log_info(team_logger, "Busco el pokemon %s\n",entrenador_aux->pokemon_bloqueo);
			pokemon_aux = buscar_pokemon_bloqueado(entrenador_aux->pokemon_bloqueo);
			if(pokemon_aux){
				log_info(team_logger,"Encontre el pokemon bloqueado %s\n",pokemon_aux->pokemon);

				entrenador_aux_deadlock = malloc(sizeof(t_deadlock));
				memset(entrenador_aux_deadlock,0,sizeof(t_deadlock));
				entrenador_aux_deadlock->pid = pokemon_aux->pid;
				entrenador_aux_deadlock->pokemon_capturado = strdup(pokemon_aux->pokemon);

				list_add(deadlock,entrenador_aux_deadlock);

				log_info(team_logger,"Busco entrenador %d que tiene %s\n",pokemon_aux->pid,pokemon_aux->pokemon);
				entrenador_aux = buscar_entrenador_en_lista_pid(lista_bloqueados,pokemon_aux->pid);

				if(entrenador_aux){
					log_info(team_logger,"El entrenador %d fue bloqueado por %s\n",entrenador_aux->id,entrenador_aux->pokemon_bloqueo);
					entrenador_aux_deadlock->pokemon_pedido = strdup(entrenador_aux->pokemon_bloqueo);

					log_info(team_logger, "Busco el pokemon bloqueado %s\n",entrenador_aux->pokemon_bloqueo);
					pokemon_aux = buscar_pokemon_bloqueado(entrenador_aux->pokemon_bloqueo);
					if(pokemon_aux){
						log_info(team_logger, "El pokemon bloqueado %s lo tiene el entrenador %d\n",pokemon_aux->pokemon,pokemon_aux->pid);

						log_info(team_logger,"Comparo al entrenador %d que tiene el pokemon %s con el entrenador analizado %d\n",pokemon_aux->pid,pokemon_aux->pokemon,entrenador_analizado->id);
						if(pokemon_aux->pid == entrenador_analizado->id){
							entrenador_inicial->pokemon_capturado = strdup(pokemon_aux->pokemon);
							log_info(team_logger,"Se encontró un deadlock analizando al entrenador %d\n",entrenador_analizado->id);

							seguir_buscando = 0;

							t_list* deadlock_aux = list_create();
							list_add_all(deadlock_aux,deadlock);

							list_add(lista_deadlocks,deadlock_aux);
							list_clean(deadlock);

						}
					}
					else{
						log_info(team_logger,"No hay deadlock para %d\n",entrenador_aux->id);
						seguir_buscando = 0;
					}
				}
				else{
					log_info(team_logger,"No hay deadlock para %d\n",entrenador_aux->id);
					seguir_buscando = 0;
				}
			}
			else{
				log_info(team_logger,"No hay deadlock para %d\n",entrenador_aux->id);
				seguir_buscando = 0;
			}

		}//End while

		list_destroy(deadlock);

	}//End for

	if(list_size(lista_deadlocks))
	{
		list_iterate(lista_deadlocks,(void*)mostrar_deadlock);
	}
	else{
		log_info(team_logger,"No se encontraron deadlocks en el estado actual del sistema\n");
	}


	destruir_lista_deadlocks(lista_deadlocks);
	return;
}

int buscar_entrenador_en_deadlock(t_list* lista_deadlocks,int pid){

	bool deadlock_tiene_pid(t_list * deadlock){

		bool is_pid_en_deadlock(t_deadlock * entrenador_en_deadlock){
			return (entrenador_en_deadlock->pid==pid);
		}
		return(list_find(deadlock,(void*)is_pid_en_deadlock));
	}

	t_list * aux = NULL;
	if(list_size(lista_deadlocks)){
		aux = list_find(lista_deadlocks,(void*)deadlock_tiene_pid);
			if(aux!=NULL)
				return 1;
			else
				return 0;
	}
	else{
		return 0;
	}
}

void destruir_lista_deadlocks(t_list * lista_deadlocks){

	void destruir_deadlock(t_list * deadlock){

		void destruir_entrenador_en_deadlock(t_deadlock * entrenador_en_deadlock){
			if(entrenador_en_deadlock->pokemon_pedido!=NULL){
				free(entrenador_en_deadlock->pokemon_pedido);
				entrenador_en_deadlock->pokemon_pedido = NULL;
			}
			if(entrenador_en_deadlock->pokemon_capturado!=NULL){
				free(entrenador_en_deadlock->pokemon_capturado);
				entrenador_en_deadlock->pokemon_capturado = NULL;
			}
			free(entrenador_en_deadlock);
		}
		list_destroy_and_destroy_elements(deadlock,(void*)destruir_entrenador_en_deadlock);
	}
	list_destroy_and_destroy_elements(lista_deadlocks,(void*)destruir_deadlock);
}

void mostrar_deadlock(t_list * deadlock){
	void mostrar_entrenador_en_deadlock(t_deadlock * entrenador){
		log_info(team_logger,"ID entrenador: %d\n", entrenador->pid);
		log_info(team_logger,"pokemon pedido: %s\n", entrenador->pokemon_pedido);
		log_info(team_logger,"pokemon capturado: %s\n", entrenador->pokemon_capturado);

		printf("\n");

		return;
	}

	printf("Deadlock encontrado:\n\n");

	list_iterate(deadlock,(void*)mostrar_entrenador_en_deadlock);
}
*/
