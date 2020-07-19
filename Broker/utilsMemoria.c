#include "utilsMemoria.h"

/*Me fijo si el tamaño de mi mensaje a guardar, es menor que el minimo tamaño de particion*/
int tamanio_a_alojar(int tamanio){

	int tamanio_final = tamanio_minimo_particion; //tamanio_minimo_particion es variable global

	if(tamanio > tamanio_minimo_particion){ 
		tamanio_final = tamanio;
	}   

	return tamanio_final;
}

/* Obtengo el indice de un determinado, recorriendo toda la lista y comparando los payload */
int obtener_indice_particion(t_bloque_memoria* bloque){

	int indice=0;
    
    bool buscar_bloque(void* _bloque){
        
        if(_bloque!=bloque){
            indice ++;
        }

        return _bloque == bloque;
    }

    list_find(cache_mensajes->memoria, buscar_bloque);

	return indice;
}

/* Seteo un bloque vacio */
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

    void* aux = bloque->estructura_mensaje->mensaje;

    eliminar_mensaje_cola(bloque->estructura_mensaje);

    bloque->estructura_mensaje = aux;   

    /* Marco el bloque como vacio */
    bloque->esta_vacio = true;

	/* Vacio el timestamp del bloque*/
	bloque->timestamp = 0;

	/* Vacio el last_time del bloque*/
	bloque->last_time = 0;


    return ;

}

void liberar_bloque_memoria_sin_mensaje(t_bloque_memoria* bloque){

    bloque->estructura_mensaje = bloque->estructura_mensaje->mensaje;   

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
void* calcular_posicion_relativa(t_bloque_memoria* _bloque){

    /* Obtengo el primer bloque de mi lista para saber donde empieza la memoria*/
    t_bloque_memoria* primer_bloque = list_get(cache_mensajes->memoria,0);

    void* posicion_bloque = _bloque->esta_vacio ? _bloque->estructura_mensaje : _bloque->estructura_mensaje->mensaje;
    void* posicion_primer_bloque = primer_bloque->esta_vacio ? primer_bloque->estructura_mensaje : primer_bloque->estructura_mensaje->mensaje;

    void* resultado =  posicion_bloque - posicion_primer_bloque;    

    return resultado;
}

/* Imprime por pantalla el estado de la memoria*/
void print_memoria(void* _bloque){
    t_bloque_memoria* bloque = (t_bloque_memoria*) _bloque;

    if(bloque->esta_vacio == false) printf("\nId : %d\n",bloque->estructura_mensaje->id_mensaje);

    if(debug_broker) log_warning(broker_logger, "Tamaño: %d \n",bloque->tamanio_particion);

}

/* Ordena las posiciones de memoria */
bool ordenar_bloques_memoria(void* puntero1, void* puntero2){

    t_bloque_memoria* particion1 = (t_bloque_memoria*) puntero1;
    t_bloque_memoria* particion2 = (t_bloque_memoria*) puntero2;

    if(particion1->esta_vacio){
        if(particion2->esta_vacio)  return particion1->estructura_mensaje < particion2->estructura_mensaje;
        else    return particion1->estructura_mensaje < particion2->estructura_mensaje->mensaje;
    }
    else{
        if(particion2->esta_vacio)  return particion1->estructura_mensaje->mensaje < particion2->estructura_mensaje;
        else    return particion1->estructura_mensaje->mensaje < particion2->estructura_mensaje->mensaje;        
    }
}

/*Obtengo el tiempo actual en segundos*/ 
uint64_t get_timestamp(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	uint64_t  x = (uint64_t)( (tv.tv_sec)*1000 + (tv.tv_usec)/1000 );
	return x;
}

void actualizar_lru_de_mensaje(t_mensaje_cola* mensaje){

    t_bloque_memoria* bloque = buscar_bloque_de_mensaje(mensaje);

    bloque->last_time = get_timestamp();

}

t_bloque_memoria* buscar_bloque_de_mensaje(t_mensaje_cola* mensaje){

    bool encontrar_bloque(void* _bloque){
        
        t_bloque_memoria* bloque = (t_bloque_memoria*) _bloque;

        if(bloque->esta_vacio){
            return false;
        }

        return bloque->estructura_mensaje->id_mensaje == mensaje->id_mensaje;
    }

    t_bloque_memoria* bloque_encontrado = list_find(cache_mensajes->memoria,encontrar_bloque);

    return bloque_encontrado;

}