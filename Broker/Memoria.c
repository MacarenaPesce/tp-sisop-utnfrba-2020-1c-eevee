#include "Memoria.h"


//ASCII boquitas en hexa
//control + shift + u 3e y 3c 


void LiberarMemoriaInicial(void* bloque_memoria_inicial,t_list* lista_memoria){
    
    /* Libero la memoria inicial */
    free(bloque_memoria_inicial);

    /* Destruyo la lista y su contenido */
    //list_destroy_and_destroy_elements(lista_memoria);
    return;
}


//--------------- ADMINISTRACION DE MEMORIA-------------------

//-----------------------------ELECCION DE PARTICION LIBRE--------------------

t_bloque_memoria* algoritmo_de_particion_libre(int tamanio){
    
    t_bloque_memoria* bloque;

    if( algoritmo_particion_libre == "FF"){
        bloque = algoritmo_first_fit(tamanio);
    }
    else{
        bloque = algoritmo_best_fit(tamanio);
    }


    return bloque;

}

//------------------------------FIRST FIT------------------------------------
/* Busco el primer hueco donde puedo alojar una nueva particion y la alojo.
    NOTA: para esto ya esta chequeado que puede alojarse
    NOTA2: recibo el tamaño correcto, ya chequeado si es el minimo a alojar o no.*/
t_bloque_memoria* algoritmo_first_fit(int tamanio_bytes){

    t_bloque_memoria* bloque;
    t_bloque_memoria* aux;
    t_bloque_memoria* bloque_final;
    int indice; 

    //obtengo el primer bloque donde quepa mi particion nueva
	for(int i=0; i< list_size(lista_memoria); i++){

        aux = list_get(lista_memoria, i);

        if((aux->tamanio >= tamanio_bytes) && (aux->esta_vacio == TRUE)){
            bloque = aux;
            i = list_size(lista_memoria);
            break;
        }

    }

    //obtengo el indice del bloque que voy a particionar
    indice = obtener_indice_particion(bloque);

    //particiono el bloque donde voy a alojar mi particion, PERO con el tamaño actualizado
    bloque_final = particionar_bloque(tamanio_bytes,indice);

    return bloque_final;

}

//------------------------------BEST FIT-------------------------------------
/* Busco el huevo mas pequeño donde quepa la nueva particion a alojar, y la alojo.
    NOTA: para esto ya esta chequeado que puede alojarse
    NOTA2: recibo el tamaño correcto, ya chequeado si es el minimo a alojar o no.*/
t_bloque_memoria* algoritmo_best_fit(int tamanio_bytes){

    t_bloque_memoria* bloque;
    t_bloque_memoria* aux;
    t_bloque_memoria* bloque_final;
    int indice;
    int tam_minimo=0; 

    //obtengo el primer bloque donde quepa mi particion nueva
	for(int i=0; i<list_size(lista_memoria); i++){

        //Obtengo el bloque en la posicion de la lista que estamos
        aux = list_get(lista_memoria, i);

        //me fijo si el tamaño que quiero alojar cabe o no en el bloque actual y ademas si el bloque en el que estoy esta vacio o no 
        if((aux->esta_vacio == TRUE) && (aux->tamanio >= tamanio_bytes)){
            
            //me fijo si el tamaño mas chico de particiones sigue siendo 0 
            if(tam_minimo==0){
                tam_minimo = aux->tamanio;
            }

            //me fijo si el tamaño mas chico de particion es menor o igual al tamaño del bloque auxiliar actual, en caso de serlo, al bloque le asigno el aux
            //y asi me quedo con el bloque mas chico de toda la lista
            if(tam_minimo <= aux->tamanio){
                bloque = aux;
            }

        }

    }

    //obtengo el indice del bloque que voy a particionar
    indice = obtener_indice_particion(bloque);

    //particiono el bloque donde voy a alojar mi particion, PERO con el tamaño actualizado
    bloque_final = particionar_bloque(tamanio_bytes,indice);

    return bloque_final;

}

//------------------------------FIN DE ELECCION DE PARTICION------------------






//----------------------------ALGORITMOS DE MEMORIA-------------------------

/*Segun el algoritmo que me pasa el archivo de configuracion, designo con esta funcion
    que algoritmo se va a usar y le paso la lista y el tamaño de bytes*/
t_bloque_memoria* algoritmo_de_memoria(int tamanio_en_bytes){

    //Creo una nueva particion, que es donde se van a guardar los datos de la particion alojada
    //y la particion que voy a retornar
    t_bloque_memoria* particionNueva;
    
    //segun el algoritmo del archivo de configuracion, utilizo un algoritmo
    if (algoritmo_memoria == "BD"){
        particionNueva= buddy_system(tamanio_en_bytes);
    }
    else{
        particionNueva= particiones_dinamicas(tamanio_en_bytes);
    }

    return t_bloque_memoria;
}

//---------------------------PARTICIONES DINAMICAS CON COMPACTACION----------------------
t_bloque_memoria* particiones_dinamicas( int tamanio_en_bytes){

    t_bloque_memoria* particionNueva;

    //me fijo si la particion puede alojarse a la primera
    bool sePuedeAlojar = puede_alojarse(tamanio_en_bytes);


    //si puede alojarse a la primera llamo al algoritmo de particion libre
    if(sePuedeAlojar == TRUE){
        particionNueva = algoritmo_de_particion_libre(tamanio_en_bytes);
    }
    /*else{ //si no puede alojarse a la primera ....

    }*/

    return particionNueva;

}


//------------------------------BUDDY SYSTEM----------------------------------
t_bloque_memoria* buddy_system( int tamanio_en_bytes){

    //int bytes_reservar= numero_potencia_dos(int tamanio_en_bytes)

    t_bloque_memoria* particionNueva;

    return t_bloque_memoria;
    
}

//---------------------------FIN DE ALGORITMOS DE MEMORIA-----------------------




//----------------------------ELIMINAR UNA PARTICION-----------------------

/*void* algoritmo_de_reemplazo(){

    return;

}*/

//------------------------------FIFO--------------------------------

/* elije como victima a la pagina que esta cargada en memoria hace mas tiempo*/
/* se puede implementar guardando el instante en el que la pagina fue cargada
    o con una cola fifo y se elige a la pagina que primero se agrego*/
void algoritmo_fifo(){

    t_bloque_memoria* elemento;
    t_bloque_memoria* bloque;
    uint64_t min_time = get_timestamp();
    int indice;

	for(int i=0; i< list_size(lista_memoria); i++){

        elemento = list_get(lista_memoria, i);

        //me fijo si el elemento actual de la lista vacio y si el timestamp es mayor a cero
        if( (elemento->esta_vacio == false) && (elemento->timestamp > 0) ){
            
            //si el timestamp, es menor al minimo time, lo guardo y me guardo el bloque
            if(elemento->timestamp < min_time){
                
                min_time= elemento->timestamp;

                bloque=aux;

            }
        }

    }

    //obtengo el indice del bloque que voy a particionar
    indice = obtener_indice_particion(bloque);
    
    //libero la memoria de un determinado bloque de mi lista , y me lo devuelve
    liberar_bloque_memoria(bloque,indice);

    return ;
}

//------------------------------FIN DE FIFO--------------------------------


//------------------------------LRU--------------------------------

/* elije como victima a la pagina menos reciente utilizada*/
/* se puede implementar guardando el instante de ultima referencia a la pagina 
    o con una pila con numeros de paginas, con cada referencia se coloca la pag
    superior, se elige como victima la pag de la parte inferior*/
void algoritmo_lru(){

    //tengo que agregar en el struct de la particion , un valor para el ultimo momento usado, y ver cuando mierda se actualiza

    return ;
}

//--------------------------FIN DE LRU--------------------------------

//------------------------FIN ELIMINAR UNA PARTICION-----------------------




//----------------------------DUMP DE LA MEMORIA--------------------------



//-----------------------------FIN DEL DUMP--------------------------------