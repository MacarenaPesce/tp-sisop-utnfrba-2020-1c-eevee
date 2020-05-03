#ifndef MEMORIA_BROKER_H_
#define MEMORIA_BROKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <commons/collections/list.h>

//--------------- ADMINISTRACION DE MEMORIA-------------------

t_list* lista_particiones; //lista de particiones de la memoria

//Estructura para reservar la memoria inicial
typedef struct{
    int tamanio;
    bool esta_vacio;
    float* payload;
}t_bloque_memoria;

//Estructura para particiones de la memoria inicial
/*
typedef struct{
    int id_particion;
    int tamanio;
    int esta_vacio;
    //int comienzo;
    //int fin;
}t_particion;*/    

void* AsignarMemoriaInicial(int tamanio_en_bytes, t_list* lista_memoria);
void  LiberarMemoriaInicial(void* bloque_memoria_inicial,t_list* lista_memoria);

void* AsignarBloqueMemoria(t_list* lista_memoria, int tamanio_en_bytes);
void  LiberarBloqueMemoria(t_bloque_memoria *bloque);

#endif /* MEMORIA_BROKER_H_ */
