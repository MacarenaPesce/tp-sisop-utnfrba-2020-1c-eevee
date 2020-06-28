/*
 * Funciones_para_listas.h
 *
 *  Created on: 30 may. 2020
 *      Author: utnso
 */

#ifndef FUNCIONES_PARA_LISTAS_H_
#define FUNCIONES_PARA_LISTAS_H_

#include "Contexto_team.h"
#include "Herramientas_team.h"

/*PARA SEPARAR LISTAS*/
void separar_pokemones_de_objetivo(char*);
void separar_listas_pokemones(char*, t_list*);
void separar_listas_objetivos(char*);
void separar_pokemones_de_entrenador(char*, t_list*);


/*PARA INICIALIZAR LISTAS*/
void inicializar_listas();


/*PARA AGREGAR COSAS A LISTAS*/
void agregar_a_lista_objetivos(char*);
void agregar_a_lista_pokemones(char*, t_list*);
void agregar_objetivo(char*, uint32_t, t_list*);
void agregar_objetivo_entrenador(char*, uint32_t, t_list*);


/*PARA CARGAR LISTAS*/
void cargar_objetivos(t_list*, t_list*);
void cargar_objetivos_entrenador(t_list* , t_list*);


/*PARA OBTENER ALGO DE LAS LISTAS*/
t_list* obtener_pokemones(t_list*,t_list*, uint32_t);
uint32_t obtener_cantidad_maxima(t_list*);
void sacar_de_objetivos_pokemones_atrapados(t_list*, t_list*);


/*PARA MOSTRAR LO QUE HAY EN LAS LISTAS*/
void mostrar_lo_que_hay_en_la_lista_de_objetivos();
void mostrar_lo_que_hay_en_lista_entrenadores();


/*OTROS*/
void liberar_lista(char**);
void string_iterate_lines_with_list(char**, t_list*, void (*closure)(char*, t_list*));
int ordenar(char*, char*);


#endif /* FUNCIONES_PARA_LISTAS_H_ */
