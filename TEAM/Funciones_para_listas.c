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
	pokemones_ordenada = list_create();
	mensajes = list_create();

}

int ordenar(char* cadena1, char* cadena2){
	return strcmp(cadena1, cadena2);
}

void string_iterate_lines_with_list(char** strings, t_list* list, void (*closure)(char*, t_list*)) {
	while (*strings != NULL) {
		closure(*strings, list);
		strings++;
	}
}

void separar_listas_pokemones(char *string, t_list* lista){
	if(string != NULL){
		list_add(lista, string);
		//printf("Una lista de pokemones es: %s\n", string);
	} else{
		printf("Got NULL\n");
	}
}

void separar_pokemones_de_entrenador(char* pokemones_de_entrenador, t_list* lista){
	char** pokes = string_split(pokemones_de_entrenador, "|");
	string_iterate_lines_with_list(pokes, lista, agregar_a_lista_pokemones);
	free(pokes);
}



void agregar_a_lista_pokemones(char* especie, t_list* lista){
	if (especie != NULL) {
		list_add(lista, especie);
	}
}

void agregar_objetivo(char* especie, uint32_t cantidad, t_list* lista){
	if(lista == lista_objetivos){
		t_objetivo* objetivo = malloc(sizeof(t_objetivo));
		objetivo->especie = especie;
		objetivo->cantidad_necesitada = cantidad;
		objetivo->cantidad_atrapada = 0;
		list_add(lista, (void*)objetivo);
	} else {
		t_objetivo_entrenador* objetivo = malloc(sizeof(t_objetivo_entrenador));
		objetivo->especie = especie;
		objetivo->cantidad = cantidad;
		list_add(lista, (void*)objetivo);
	}
}

void cargar_objetivos(t_list* pokemones, t_list* lista){
		char* unPokemon;
		unPokemon = list_get(pokemones, 0);
		uint32_t contador = 0;
		char* un_char = "Ultimo poke\0";
		char* ultimo_poke = string_from_format("%s\0", un_char);
		list_add(pokemones, ultimo_poke);

		/*Empiezo a cargar a lista de objetivos, con tipo y cantidad de cada uno*/
		char* especiePokemon;
		char* otroPokemon;
		int i = 0;
		while(pokemones != NULL){
			especiePokemon = unPokemon;
			otroPokemon = list_get(pokemones, i);
			if(otroPokemon == NULL){
				agregar_objetivo(especiePokemon, contador, lista);
				break;
			}
			if(string_equals_ignore_case(unPokemon,otroPokemon)){
				contador++;
				i++;
			}else{
				agregar_objetivo(especiePokemon, contador, lista);
				unPokemon = otroPokemon;
				contador = 1;
				i++;

			}
		}

		free(unPokemon);
		//list_destroy(pokemones);
}




