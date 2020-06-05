/*
 * Funciones_para_listas.c
 *
 *  Created on: 30 may. 2020
 *      Author: utnso
 */

#include "Funciones_para_listas.h"

void inicializar_listas(){

	lista_listos = list_create();
	lista_finalizar = list_create();
	lista_bloqueados = list_create();
	lista_objetivos = list_create();
	lista_entrenadores = list_create();
	lista_mapa = list_create();

}

int ordenar(char* cadena1, char* cadena2){
	return strcmp(cadena1, cadena2);
}

void _a_la_lista(char *elemento){
	if (elemento != NULL) {
		list_add(lista_config, elemento);
	}
}

void _imprimir(char *string) {
	if(string != NULL) {
		char **pokes = string_split(string, "|");
		string_iterate_lines(pokes, _a_la_lista);
		free(pokes);
		//printf("Read: %s\n", string);
	}else {
		printf("Got NULL\n");
	}
}

void mostrar(void *elemento) {
	printf("El elemento: %s\n", (char *)elemento);
}

void separar_listas_objetivos(char *string){
	if(string != NULL){
		list_add(lista_global_objetivos, string);
		//printf("Una lista de objetivos es: %s\n", string);
	} else{
		printf("Got NULL\n");
	}
}

void separar_pokemones_de_objetivo(char* objetivos_de_entrenador){
	char **pokes = string_split(objetivos_de_entrenador, "|");
	string_iterate_lines(pokes, agregar_a_lista_objetivos);
	free(pokes);
}

void agregar_a_lista_objetivos(char* elemento){
	if (elemento != NULL) {
		list_add(lista_objetivos_de_entrenador, elemento);
	}
}

