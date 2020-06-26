/*
 * Algoritmos_de_planificacion.c
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#include "Algoritmos_de_planificacion.h"



/*
 * *
			 * Si el algoritmo es con desalojo, (solo SJF) debo chequear si la estimacion del esi nuevo es
			 * menor a la rafaga pendiente del que esta en ejecucion, si lo es reemplazarlo por el nuevo.
			 *
			if(!strcmp(config.algoritmo, "SJF-CD"))
			{
				if((esi_en_ejecucion!=NULL) && (nuevo_esi->estimacion_real < esi_en_ejecucion->estimacion_actual))
				{
					logger_planificador(escribir_loguear,l_info,"El ESI nuevo de PID %d debe desalojar al ESI en ejecucion!",nuevo_esi->pid);
					desalojo_en_ejecucion++;
					esi_por_desalojar = nuevo_esi;
					logger_planificador(escribir_loguear,l_info,"Esperando a que ESI %d termine su sentencia\n",esi_en_ejecucion->pid);
				}
			}


 */

void * planificar(){
	/*A medida que el Team empiece a recibir distintos Pokémon en el mapa despertará a los distintos entrenadores en estado New o en
	Blocked (que estén esperando para procesar) pasandolos a Ready. */

	/*Siempre se planificará aquel entrenador que se encuentre sin estar realizando ninguna operación activamente y, en caso de
existir más de uno, sea el que más cerca se encuentre del objetivo. A medida que cada entrenador se planifique (ya sea para moverse, intercambiar o atrapar un
Pokémon) entrarán en estado exec. En el contexto de nuestro trabajo practico no contemplaremos el multiprocesamiento, esto implica que solo UN entrenador
podrá estar en estado Exec en determinado tiempo. Cuando un entrenador en estado Exec finalice su recorrido y su ejecución planificada entrará en un estado bloqueados.
Este estado implica que el entrenador no tiene más tareas para realizar momentáneamente. Cuando un entrenador en estado Exec cumpla todos sus objetivos, pasará a estado Exit.
Cuando todos los entrenadores dentro de un Team se encuentren en Exit, se considera que el proceso Team cumplió el objetivo global. */

	while(GLOBAL_SEGUIR){

		sem_wait(&orden_para_planificar);

		/*if(!strcmp(config.algoritmo, "SJF-CD")){
			if((esi_en_ejecucion!=NULL) && (nuevo_esi->estimacion_real < esi_en_ejecucion->estimacion_actual))
			{
				logger_planificador(escribir_loguear,l_info,"El ESI nuevo de PID %d debe desalojar al ESI en ejecucion!",nuevo_esi->pid);
				desalojo_en_ejecucion++;
				esi_por_desalojar = nuevo_esi;
				logger_planificador(escribir_loguear,l_info,"Esperando a que ESI %d termine su sentencia\n",esi_en_ejecucion->pid);
			}
		}*/

		obtener_proximo_ejecucion();
	}

	return NULL;
}

void crear_hilo_para_planificar(){
	sem_wait(&entrenadores_ubicados);
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)planificar, NULL);
}

void seleccionar_el_entrenador_mas_cercano_al_pokemon(t_pokemon* pokemon){
	//Para poder planificar un entrenador, se seleccionará el hilo del entrenador más cercano al Pokémon.

	t_list* lista_aux;
	lista_aux = list_duplicate(lista_entrenadores);
	list_add_all(lista_aux, lista_bloqueados_esperando);

	int i = 0;
	bool mas_cerca;
	t_entrenador* entrenador_mas_cercano;
	t_entrenador* otro_entrenador;
	entrenador_mas_cercano = list_get(lista_aux, i);
	int cantidad_entrenadores = list_size(lista_aux);

	while(i < cantidad_entrenadores){
		i++;
		if(i == cantidad_entrenadores){
			entrenador_mas_cercano->objetivo_actual = pokemon;

			pthread_mutex_lock(&lista_listos_mutex);
			list_add(lista_listos, (void*)entrenador_mas_cercano);
			pthread_mutex_unlock(&lista_listos_mutex);

			pthread_mutex_lock(&lista_entrenadores_mutex);
			sacar_entrenador_de_lista_pid(lista_entrenadores, entrenador_mas_cercano->id);
			pthread_mutex_unlock(&lista_entrenadores_mutex);
			break;
		}
		otro_entrenador = list_get(lista_aux, i);
		if(otro_entrenador == NULL){
			break;
		}
		mas_cerca = esta_mas_cerca(entrenador_mas_cercano, otro_entrenador, pokemon);
		if(!mas_cerca){
			entrenador_mas_cercano = otro_entrenador;
		}
	}

	list_destroy(lista_aux);
	if(entrenador_mas_cercano == NULL){
		log_info(team_logger, "No hay mas entrenadores disponibles");
	} else {
		log_info(team_logger,"El entrenador %d pasa a Ready por ser el mas cercano a %s", entrenador_mas_cercano->id, entrenador_mas_cercano->objetivo_actual->especie);
	}
}

bool esta_mas_cerca(t_entrenador* entrenador1, t_entrenador* entrenador2, t_pokemon* pokemon){
	int distancia_entrenador1 = distancia_a_pokemon(entrenador1, pokemon);
	int distancia_entrenador2 = distancia_a_pokemon(entrenador2, pokemon);
	return distancia_entrenador1 < distancia_entrenador2;
}

int distancia_a_pokemon(t_entrenador* entrenador, t_pokemon* pokemon){
	int distancia_X = entrenador->posx - pokemon->posx;
	int distancia_Y = entrenador->posy - pokemon->posy;
	int distancia_total = abs(distancia_X) + abs(distancia_Y);
	return distancia_total;
}

t_entrenador * sacar_entrenador_de_lista_pid(t_list* lista,int pid){
	bool is_pid_entrenador(t_entrenador * entrenador){
		return (entrenador->id==pid);
	}
	return (list_remove_by_condition(lista,(void*)is_pid_entrenador));
}

void ordenar_lista_estimacion(t_list * lista){
	bool is_estimacion_menor(t_entrenador * entrenador1, t_entrenador * entrenador2){
		return ( (entrenador1->estimacion_real < entrenador2->estimacion_real) || (entrenador1->estimacion_real == entrenador2->estimacion_real) );
	}

	/*El comparador devuelve si el primer parametro debe aparecer antes que el segundo en la lista*/
	list_sort(lista, (void*)is_estimacion_menor);
	return;
}

void desalojar_ejecucion(void){
	if(entrenador_en_ejecucion!=NULL){
		estimar_entrenador(entrenador_en_ejecucion);
		entrenador_en_ejecucion = NULL;
	}
	return;
}

int estimar_entrenador(t_entrenador * entrenador){

	entrenador->estimacion_anterior = entrenador->estimacion_real;

	entrenador->estimacion_real = ((alpha/100)*entrenador->instruccion_actual) + ((1-(alpha/100))*entrenador->estimacion_real);

	entrenador->estimacion_actual  = entrenador->estimacion_real;
	entrenador->instruccion_actual = 0;

	return 0;
}

void obtener_proximo_ejecucion(void){
	/*Para planificar a los distintos entrenadores se utilizarán los algoritmos FIFO, Round Robin y Shortest job first con y sin desalojo. Para este último algoritmo
	 *se desconoce la próxima rafaga, por lo que se deberá utilizar la fórmula de la media exponencial. A su vez, la estimación inicial para todos los entrenadore
	 *se s será la misma y deberá poder ser modificable por archivo de configuración */

	t_entrenador * ejec_ant;
	t_list * lista_aux;

	ejec_ant = entrenador_en_ejecucion;

	/* SJF debe copiar la lista de listos a una lista auxiliar, ordenarla por estimacion mas corta, tomar el primero, destruir la lista auxiliar. Eso para ambos casos */

	lista_aux = list_duplicate(lista_listos);

	log_info(team_logger, "Planificando por %s", algoritmo_planificacion);
	printf("\n");

	if( (!strcmp(algoritmo_planificacion, "SJF-SD")) || (!strcmp(algoritmo_planificacion, "SJF-CD"))){
		ordenar_lista_estimacion(lista_aux);
	}

	if((!strcmp(algoritmo_planificacion, "RR"))){
		quantum_actual = quantum;
	}

	/* FIFO: Directamente saca el primer elemento de la lista y lo pone en ejecucion. Por default hace fifo */
	entrenador_desalojado = NULL;
	entrenador_en_ejecucion = list_remove(lista_aux,0);

	if(!list_is_empty(lista_listos)){
		pthread_mutex_lock(&lista_listos_mutex);
		entrenador_en_ejecucion = sacar_entrenador_de_lista_pid(lista_listos, entrenador_en_ejecucion->id);
		pthread_mutex_unlock(&lista_listos_mutex);

		entrenador_en_ejecucion->estado = EJECUTANDO;
		sem_post(&array_semaforos[(int)entrenador_en_ejecucion->id]);
	}
	else{
		entrenador_en_ejecucion = NULL;
		log_info(team_logger, "No hay entrenadores para ejecutar!");
	}

	list_destroy(lista_aux);

	//Si hubo un cambio en el entrenador en ejecucion, debo avisarle al nuevo entrenador en ejecucion que es su turno
		if((entrenador_en_ejecucion != NULL) && (ejec_ant != entrenador_en_ejecucion)){
			//log_info(team_logger,"Aca le debo avisar al entrenador %d que es su turno\n", entrenador_en_ejecucion->id);
		}

	return;
}
