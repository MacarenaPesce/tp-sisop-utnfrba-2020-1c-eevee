/*
 * Funciones_para_listas.c
 *
 *  Created on: 30 may. 2020
 *      Author: utnso
 */

#include "Funciones_para_listas.h"

void mostrar_lo_que_hay_en_la_lista_de_objetivos(){
	int k = 0;
	t_objetivo * objetivo = malloc(sizeof(t_objetivo));
	while(!list_is_empty(lista_objetivos)){
		objetivo = list_get(lista_objetivos, k);
		if(objetivo == NULL){
			break;
		}
		log_info(team_logger,"Un objetivo es de la especie = %s, cantidad necesitada %i, cantidad atrapada %i", objetivo->especie, objetivo->cantidad_necesitada, objetivo->cantidad_atrapada);
		k++;
	}
	free(objetivo);
}

void mostrar_lo_que_hay_en_lista_entrenadores(){
	int l = 0;
	t_entrenador * entrenador;
	while(!list_is_empty(lista_entrenadores)){
		entrenador = list_get(lista_entrenadores, l);
		if(entrenador == NULL){
			break;
		}
		log_info(team_logger,"Un entrenador tiene id = %i, pos x = %i, y = %i, y puede atrapar %i pokemones\n", entrenador->id, entrenador->posx, entrenador->posy, entrenador->cant_maxima_objetivos);
		l++;
	}
	//free(entrenador);
}

void inicializar_listas(){

	lista_listos = list_create();
	lista_finalizar = list_create();
	lista_bloqueados = list_create();
	lista_objetivos = list_create();
	lista_entrenadores = list_create();
	lista_mapa = list_create();
	pokemones_ordenada = list_create();
	mensajes = list_create();
	lista_bloqueados_esperando = list_create();

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
	char* un_char = "|\0";
	if(pokemones_de_entrenador == NULL){
	} else if(!(string_contains(pokemones_de_entrenador, un_char))) {
		agregar_a_lista_pokemones(pokemones_de_entrenador, lista);
	} else {
		char**pokes = string_split(pokemones_de_entrenador, "|");
		string_iterate_lines_with_list(pokes, lista, agregar_a_lista_pokemones);
		free(pokes);
	}
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
		if(especie != NULL){
		t_objetivo_entrenador* objetivo = malloc(sizeof(t_objetivo_entrenador));
		objetivo->especie = especie;
		objetivo->cantidad = cantidad;
		list_add(lista, (void*)objetivo);
		} else {
			printf("El objetivo de este entrenador es nulo");
		}
	}
}

void cargar_objetivos(t_list* pokemones, t_list* lista){
		char* unPokemon;
		unPokemon = list_get(pokemones, 0);
		if(unPokemon != NULL){
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
		}

		//free(unPokemon);
		//list_destroy(pokemones);
}

t_list* obtener_pokemones(t_list* lista_global,t_list* lista, uint32_t posicion){

	char* pokemones_de_entrenador;
	lista_pokemones_objetivos = list_create();//este list_create es el culpable de la mayoria de los leaks
	pokemones_de_entrenador = list_get(lista_global, posicion);

	separar_pokemones_de_entrenador(pokemones_de_entrenador, lista);
	list_sort(lista, (void*)ordenar);
	cargar_objetivos(lista, lista_pokemones_objetivos);
	list_remove(lista_pokemones_objetivos, list_size(lista_pokemones_objetivos)-1);

	return lista_pokemones_objetivos;

}

uint32_t obtener_cantidad_maxima(t_list* lista){
	uint32_t contador = 0;
	t_objetivo_entrenador* un_objetivo;
	for(int i = 0; i < list_size(lista); i++){
		un_objetivo = list_get(lista, i);
		contador += un_objetivo->cantidad;
	}
	return contador;

}

void sacar_de_objetivos_pokemones_atrapados(t_list* lista_de_objetivos, t_list* lista_de_pokemones){
	for (int i = 0; i < list_size(lista_de_pokemones); i++){
		t_objetivo_entrenador* pokemon = list_get(lista_de_pokemones, i);
		t_objetivo_entrenador* objetivo = buscar_pokemon_por_especie(lista_de_objetivos, pokemon->especie);
		if(objetivo != NULL){
			if(objetivo->cantidad >= pokemon->cantidad){
				objetivo->cantidad -= pokemon->cantidad;
			} else {
				objetivo->cantidad = 0;
			}
		}
	}
}




