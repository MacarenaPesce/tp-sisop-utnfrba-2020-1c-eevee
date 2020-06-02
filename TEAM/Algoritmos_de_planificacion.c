/*
 * Algoritmos_de_planificacion.c
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#include "Algoritmos_de_planificacion.h"

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

	/*
	* El comparador devuelve si el primer parametro debe aparecer antes que el
	* segundo en la lista
	*/

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

	alpha = 50; //por darle un numero
	entrenador->estimacion_anterior = entrenador->estimacion_real;

	entrenador->estimacion_real = ( (alpha / 100) * entrenador->instruccion_actual ) +
					  ( ( 1 - (alpha / 100) ) * entrenador->estimacion_real );

	entrenador->estimacion_actual  = entrenador->estimacion_real;
	entrenador->instruccion_actual = 0;

	return 0;
}

void obtener_proximo_ejecucion(void){
	/*
	 * Para planificar a los distintos entrenadores se utilizarán los algoritmos FIFO, Round Robin y Shortest
job first con y sin desalojo. Para este último algoritmo se desconoce la próxima rafaga, por lo que se
deberá utilizar la fórmula de la media exponencial. A su vez, la estimación inicial para todos los
entrenadores será la misma y deberá poder ser modificable por archivo de configuración
	 */

	t_entrenador * ejec_ant;
	entrenador_en_ejecucion = NULL;
	t_list * lista_aux;

	ejec_ant = entrenador_en_ejecucion;

	/* SJF debe copiar la lista de listos a una lista auxiliar,
	 * ordenarla por estimacion mas corta, tomar el primero, destruir la lista auxiliar.
	 * Eso para ambos casos
	 */

	lista_aux = list_duplicate(lista_listos);
	printf("Planificando por %s...", algoritmo_planificacion);

	if( (!strcmp(algoritmo_planificacion, "SJF-SD")) || (!strcmp(algoritmo_planificacion, "SJF-CD")))
	{
		ordenar_lista_estimacion(lista_aux);
	}


	/* FIFO: Directamente saca el primer elemento de la lista y lo pone en ejecucion
	 * Por default hace fifo
	 */
	entrenador_en_ejecucion = list_remove(lista_aux,0);
	if(!list_is_empty(lista_listos)){
		printf("Saco de la lista de listos el próximo entrenador a ejecutar");
		entrenador_en_ejecucion = sacar_entrenador_de_lista_pid(lista_listos,entrenador_en_ejecucion->id);
		entrenador_en_ejecucion->estado = EJECUTANDO;
	}
	else{
		entrenador_en_ejecucion = NULL;
		printf("No hay entrenadores para ejecutar! Todo en orden...");
	}

	list_destroy(lista_aux);

	//Si hubo un cambio en el entrenador en ejecucion, debo avisarle al nuevo entrenador en ejecucion que es su turno
	//TODO

	return;
}
