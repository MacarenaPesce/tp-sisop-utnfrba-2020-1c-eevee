#include "Memoria.h"

void* AsignarMemoriaInicial(int tamaño_en_bytes){
    void* memoria_inicial = malloc(tamaño_en_bytes*sizeof(char));
    memset(memoria_inicial, -1, tamaño_en_bytes*sizeof(char));
    return memoria_inicial;
}

void LiberarMemoriaInicial(void* bloque_memoria_inicial){
    free(bloque_memoria_inicial);
    return;
}

void* AsignarBloqueMemoria(int tamaño_en_bytes,void* bloque_memoria_inicial){
    
}

void LiberarBloqueMemoria(void* bloque_memoria_inicial,void* bloque_a_vaciar,int tamanio_bloqua_a_vaciar){
    memset(&bloque_a_vaciar, -1, tamanio_bloqua_a_vaciar);
    return;
}

void* EncontrarBloqueMemoria(void* bloque_memoria_inicial, int tamanio_en_bytes){

}

