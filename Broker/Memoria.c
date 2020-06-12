#include "Memoria.h"

//--------------- ADMINISTRACION DE MEMORIA-------------------




void LiberarMemoriaInicial(void* bloque_memoria_inicial,t_list* lista_memoria){
    
    /* Libero la memoria inicial */
    free(bloque_memoria_inicial);

    /* Destruyo la lista y su contenido */
    //list_destroy_and_destroy_elements(lista_memoria);
    return;
}


void LiberarBloqueMemoria(t_bloque_memoria *bloque){

    /* Inicializo todo el bloque en 0 */
    memset(bloque->payload, 0, bloque->tamanio);   

    /* Marco el bloque como vacio */
    bloque->esta_vacio = true;

    return;

}




void* EncontrarBloqueMemoriaLibre(char* bloque_memoria_inicial, int tamanio_en_bytes){


    return ;
}

t_bloque_memoria* alojar_bloque(int tamanio_bytes){
	/*  me fijo si puede alojarse o ya me habia fijado ??
        tengo que correr el algoritmo de particion ya se first fit o best fit --- algoritmo_de_particion_libre
		encontrar la particion o bloque
		obtener el indice de partcion
		particionar el bloque
		y devuelvo el bloque alojado*/



		t_bloque_memoria* bloque;

		return bloque;
}


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

    bool sePuedeAlojar = puede_alojarse(tamanio_en_bytes);


    return t_bloque_memoria;

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


//------------------------------FIN DE FIFO--------------------------------


//------------------------------LRU--------------------------------

//--------------------------FIN DE LRU--------------------------------

//------------------------FIN ELIMINAR UNA PARTICION-----------------------





//-----------------------------ELECCION DE PARTICION LIBRE--------------------

/*void algoritmo_de_particion_libre(recibo una particion){
    

    //devuelvo una particion ??

    return;

}*/

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

        if(aux->tamanio >= tamanio_bytes){
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

//----------------------------FIN FIRST FIT----------------------------------

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

//----------------------------FIN DE BEST FIT---------------------------------

//------------------------------FIN DE ELECCION DE PARTICION------------------





//----------------------------DUMP DE LA MEMORIA--------------------------



//-----------------------------FIN DEL DUMP--------------------------------