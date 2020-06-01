/*
 * Funciones_para_listas.c
 *
 *  Created on: 30 may. 2020
 *      Author: utnso
 */

#include "Funciones_para_listas.h"

void inicializar_listas(){

	cola_listos = list_create();
	cola_finalizar = list_create();
	cola_bloqueados = list_create();
	lista_objetivos = list_create();
	lista_entrenadores = list_create();

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
		//printf("Read: %s\n", string);
	}else {
		printf("Got NULL\n");
	}
}

void mostrar(void *elemento) {
	printf("El elemento: %s\n", (char *)elemento);
}

