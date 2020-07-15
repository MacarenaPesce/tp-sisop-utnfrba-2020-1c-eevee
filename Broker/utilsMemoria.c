#include "utilsMemoria.h"

//void LiberarMemoriaInicial(void* bloque_memoria_inicial,t_list* lista_memoria){
    
    /* Libero la memoria inicial */
    //free(bloque_memoria_inicial);

    /* Destruyo la lista y su contenido */
    //list_destroy_and_destroy_elements(lista_memoria);
    //return;
//}

/*
    - Hay que modificar la compactacion, si esta seteada en -1 , no se compacta nunca y se sigue buscando, 
    se compacta unicamente cuando no haya particiones ocupadas, pero con la consolidacion no hace falta compactar

    - Hacer dump 
*/



/*Me fijo si el tamaño de mi mensaje a guardar, es menor que el minimo tamaño de particion*/
int tamanio_a_alojar(int tamanio){

	int tamanio_final = tamanio_minimo_particion; //tamanio_minimo_particion es variable global

	if(tamanio > tamanio_minimo_particion){ 
		tamanio_final = tamanio;
	}   

	return tamanio_final;

}



/*Obtengo el indice de un determinado, recorriendo toda la lista y comparando los payload*/
int obtener_indice_particion(t_bloque_memoria* bloque){

	int indice=0;

    bool buscar_bloque(void* _bloque){

        t_bloque_memoria* bloque_memoria = (t_bloque_memoria*) _bloque;        

        if(bloque_memoria->esta_vacio) {
            indice++;
            return false;
        }

        if(bloque_memoria->estructura_mensaje != bloque->estructura_mensaje){
            indice++;
        }

        return bloque_memoria->estructura_mensaje == bloque->estructura_mensaje;

    }

    list_find(cache_mensajes->memoria, buscar_bloque);
    if(debug_broker) log_debug(broker_logger,"El id malo es: %d",indice);

	return indice-1;
}



/* Seteo un bloque vacio*/
t_bloque_memoria* crear_bloque_vacio(int tamanio_particion, void* particion){

    t_bloque_memoria* bloque = (t_bloque_memoria*)malloc(sizeof(t_bloque_memoria));

    bloque->tamanio_particion = tamanio_particion;
    bloque->esta_vacio = true;
    bloque->estructura_mensaje = particion;
    bloque->timestamp = 0;
    bloque->last_time = 0;

    return bloque;

}




/* Libero la memoria de un determinado bloque y lo retorno */
void liberar_bloque_memoria(t_bloque_memoria* bloque){

    /* Marco el bloque como vacio */
    bloque->esta_vacio = true;

	/* Vacio el timestamp del bloque*/
	bloque->timestamp = 0;

	/* Vacio el last_time del bloque*/
	bloque->last_time = 0;

    return ;

}



/*Dado el tamaño de una particion, me fijo si puede alojarse a la primera 
	o si hay que correr el algoritmo de eliminacion*/ 
bool puede_alojarse(int tamanio_bytes){

    bool tiene_espacio_suficiente(void* _bloque){

        t_bloque_memoria* bloque = (t_bloque_memoria*) _bloque;

        return bloque->tamanio_particion >= tamanio_bytes && bloque->esta_vacio;

    }

    t_bloque_memoria *bloque_posible = list_find(cache_mensajes->memoria,tiene_espacio_suficiente);

    return bloque_posible != NULL;

}



/* Calcula la posicion relativa de memoria*/
void* calcular_posicion_relativa(t_bloque_memoria* bloque){

    /* Obtengo el primer bloque de mi lista para saber donde empieza la memoria*/
    t_bloque_memoria* primer_bloque = list_get(cache_mensajes->memoria,0);

    log_warning(broker_logger, "Puntero primer bloque %d", primer_bloque->estructura_mensaje->mensaje);
    log_warning(broker_logger, "Puntero mi bloque %d", bloque->estructura_mensaje->mensaje);

    void* resultado;

    if(primer_bloque->esta_vacio){
        resultado = ((char*)bloque->estructura_mensaje->mensaje) - ((char*)primer_bloque->estructura_mensaje);
    }
    else{
        resultado = ((char*)bloque->estructura_mensaje->mensaje) - ((char*)primer_bloque->estructura_mensaje->mensaje);    
    }

    /* Casteo a char* resto y lo devuelvo */

    log_warning(broker_logger, "Puntero mi bloque %d", resultado);

    return resultado;
}




/*Obtengo el tiempo actual en segundos*/ 
uint64_t get_timestamp(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	uint64_t  x = (uint64_t)( (tv.tv_sec)*1000 + (tv.tv_usec)/1000 );
	return x;
}