/*
 * Funciones_para_listas.h
 *
 *  Created on: 30 may. 2020
 *      Author: utnso
 */

#ifndef FUNCIONES_PARA_LISTAS_H_
#define FUNCIONES_PARA_LISTAS_H_

#include "Contexto_team.h"

void inicializar_listas();
int ordenar(char*, char*);
void separar_listas_objetivos(char*);
void agregar_a_lista_objetivos(char*);
void separar_pokemones_de_objetivo(char*);
void separar_listas_pokemones(char*, t_list*);
void agregar_a_lista_pokemones(char*, t_list*);
void separar_pokemones_de_entrenador(char*, t_list*);
void cargar_objetivos(t_list*, t_list*);
void agregar_objetivo(char*, uint32_t, t_list*);
void cargar_objetivos_entrenador(t_list* , t_list*);
void agregar_objetivo_entrenador(char*, uint32_t, t_list*);
void liberar_lista(char**);
void string_iterate_lines_with_list(char**, t_list*, void (*closure)(char*, t_list*));



#endif /* FUNCIONES_PARA_LISTAS_H_ */
