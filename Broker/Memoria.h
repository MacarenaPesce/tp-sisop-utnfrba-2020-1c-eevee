#ifndef MEMORIA_BROKER_H_
#define MEMORIA_BROKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <commons/collections/list.h>

typedef struct{
    int tamanio;
    bool esta_vacio;
    float* payload;
}t_bloque_memoria;

void* AsignarMemoriaInicial(int tamanio_en_bytes, t_list* lista_memoria);
void  LiberarMemoriaInicial(void* bloque_memoria_inicial,t_list* lista_memoria);

void* AsignarBloqueMemoria(t_list* lista_memoria, int tamanio_en_bytes);
void  LiberarBloqueMemoria(t_bloque_memoria *bloque);

#endif /* MEMORIA_BROKER_H_ */
