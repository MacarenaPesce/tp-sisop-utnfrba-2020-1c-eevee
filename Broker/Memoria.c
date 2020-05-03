#include "Memoria.h"

void* AsignarMemoriaInicial(int tamanio_en_bytes, t_list* lista_memoria){
    
    /* Asigno la memoria a un puntero auxiliar y la inicializo en cero */
    void* memoria_inicial = malloc(tamanio_en_bytes*sizeof(char));    
    memset(memoria_inicial, 0, tamanio_en_bytes*sizeof(char));
   
    /* Genero el bloque de memoria */
    t_bloque_memoria bloque;
    t_bloque_memoria->tamanio = tamanio_en_bytes;
    t_bloque_memoria->esta_vacio = true;
    t_bloque_memoria->payload = memoria_inicial;
  
    /* Agrego el bloque a la lista */
    list_add(lista_memoria,bloque);

    /* Retorno la variable inicial para guardarla en la global */
    return memoria_inicial;
}

void LiberarMemoriaInicial(void* bloque_memoria_inicial,t_list* lista_memoria){
    
    /* Libero la memoria inicial */
    free(bloque_memoria_inicial);

    /* Destruyo la lista y su contenido */
    list_destroy_and_destroy_elements(lista_memoria);
    return;
}

void* AsignarBloqueMemoria(t_list* lista_memoria, int tamanio_en_bytes){
    
}

void LiberarBloqueMemoria(t_bloque_memoria *bloque){

    /* Inicializo todo el bloque en 0 */
    memset(bloque->payload, 0, bloque->tamanio);   

    /* Marco el bloque como vacio */
    bloque->esta_vacio = true;

    return;

}

void _ParticionarBloqueMemoria(t_list* lista_memoria, int indice_nodo_particionar, int tamanio){

    t_bloque_memoria *bloque_restante;
    t_bloque_memoria *bloque_inicial;

    /* Obtengo el nodo del bloque a particionar por su indice */
    bloque_inicial = list_get(lista_memoria, indice_nodo_particionar);

    /* Si me sobra espacio lo separo en un nuevo nodo */
    if(bloque_inicial->tamanio - size > 0){ 

        bloque_restante->tamanio = bloque_inicial->tamanio - size;
        bloque_restante->esta_vacio = true;
        bloque_restante->payload = bloque_inicial+size;

        list_add_in_index(lista_memoria, indice_nodo_particionar + 1, bloque_restante);    

    }

    /* Seteo el nodo inicial como ocupado */
    bloque_inicial->tamanio = size;
    bloque_inicial->esta_vacio = false;

    return;
}

void* EncontrarBloqueMemoriaLibre(char* bloque_memoria_inicial, int tamanio_en_bytes){

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

