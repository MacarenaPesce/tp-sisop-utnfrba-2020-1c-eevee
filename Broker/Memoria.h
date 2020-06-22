#ifndef MEMORIA_BROKER_H_
#define MEMORIA_BROKER_H_

#include "includes.h"

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


//---ALGORITMOS DE MEMORIA----

//Segun el algoritmo que me pasa el archivo de configuracion, designo con esta funcion
//que algoritmo se va a usar y le paso la lista y el tamaño de bytes
void* algoritmo_memoria(t_list* lista_memoria, int tamanio_en_bytes);

void particiones_dinamicas(t_list* lista_memoria, int tamanio_en_bytes);
void buddy_system(t_list* lista_memoria, int tamanio_en_bytes);




#endif /* MEMORIA_BROKER_H_ */
