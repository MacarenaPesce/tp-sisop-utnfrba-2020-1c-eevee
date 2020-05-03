/*
 * Herramientas_broker.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef MEMORIA_BROKER_H_
#define MEMORIA_BROKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <commons/list.h>

void* AsignarMemoriaInicial(int tamanio_en_bytes, t_list* lista_memoria);
void  LiberarMemoriaInicial(void* bloque_memoria_inicial);

void* AsignarBloqueMemoria(t_list* lista_memoria, int tamanio_en_bytes);
void  LiberarBloqueMemoria(t_bloque_memoria *bloque);

struct t_bloque_memoria{
    int tamanio;
    bool esta_vacio;
    float* payload;
};


#endif /* MEMORIA_BROKER_H_ */
