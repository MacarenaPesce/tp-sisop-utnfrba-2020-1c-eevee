#include "Memoria.h"

void* AsignarMemoriaInicial(int tamanio_en_bytes){
    void* memoria_inicial = malloc(tamanio_en_bytes*sizeof(char));
    memset(memoria_inicial, -1, tamanio_en_bytes*sizeof(char));
    return memoria_inicial;
}

void LiberarMemoriaInicial(void* bloque_memoria_inicial){
    free(bloque_memoria_inicial);
    return;
}

void* AsignarBloqueMemoria(void* bloque_memoria_inicial, int tamanio_en_bytes){
    return EncontrarBloqueMemoria(bloque_memoria_inicial,tamanio_en_bytes);
}

void LiberarBloqueMemoria(void* bloque_memoria_inicial,void* bloque_a_vaciar,int tamanio_bloqua_a_vaciar){
    memset(&bloque_a_vaciar, -1, tamanio_bloqua_a_vaciar);
    return;
}

void* EncontrarBloqueMemoria(char* bloque_memoria_inicial, int tamanio_en_bytes){

    void* bloque_encontrado = NULL;
    int tamanio_bloque_calculado;
    int i=0;

    while(bloque_encontrado == NULL){

        if(bloque_memoria_inicial+i != -1){
            i++;
            continue;
        }

        tamanio_bloque_calculado = CalcularTamanioBloqueMemoria(bloque_memoria_inicial+i);

        if(tamanio_bloque_calculado >= tamanio_en_bytes){
            return bloque_memoria_inicial+i;
        }

        i=i+tamanio_bloque_calculado+1;
        
    }

    return bloque_encontrado;
}

int CalcularTamanioBloqueMemoria(char* inicio_del_bloque, int tamanio_en_bytes){
    
    int i = 0;
    
    while(inicio_del_bloque+i == -1 && i < tamanio_en_bytes){ i++; }

    return i;

}

