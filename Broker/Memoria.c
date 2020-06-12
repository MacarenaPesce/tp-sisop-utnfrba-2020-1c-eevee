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


//----------------------------ALGORITMOS DE MEMORIA-------------------------
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

    bool sePuedeAlojar = puedeAlojarse(tamanio_en_bytes);


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

/*void* algoritmo_first(){

    return;

}*/

/*
    Recorrer la memoria o la lista?
    Buscar la primer particion libre --- buscarParticion()
    fijarme si entra --- la ocupo o sigo buscando
    si no entra, hago una compactacion
    vuelvo a fijarme si entra, si no entra borro algo ---- que se borra?
*/

//----------------------------FIN FIRST FIT----------------------------------

//------------------------------BEST FIT-------------------------------------

/*void* algoritmo_best(){


    return;

}*/
//----------------------------FIN DE BEST FIT---------------------------------

//------------------------------FIN DE ELECCION DE PARTICION------------------





//----------------------------DUMP DE LA MEMORIA--------------------------



//-----------------------------FIN DEL DUMP--------------------------------


/*
uint32_t muse_alloc_mem(int programId, uint32_t tam) {
    // ajusta la memoria solicitada como múltiplo entero del tamaño de la página
    int cantPaginas = tam / MEMORIA.tamPagina + !!(tam % MEMORIA.tamPagina);
    tam = cantPaginas * MEMORIA.tamPagina;
    Segmento *s = MEMORIA.tabla;
    Bloque *b = NULL;
    // intenta asignar en el primer segmento
    if(!s->progId || s->progId == programId) {
        b = alojarBloque(s, tam);
    }
    // mientras no logre asignar y haya segmentos más adelante
    while(s->next && !b) {
        while(s->next->progId && s->next->progId != programId) {
            s = s->next;
        }
        if(s->next) {
            s = s->next;
            b = alojarBloque(s, tam);
        }
    }
    // s apunta ahora al segmento donde fue asignado el bloque
    // o al último segmento de la lista, si no fue asignado
    if(b) { // si el bloque fue asignado
            // establece el programa como propietario del segmento
        s->progId = programId;
    } else {    // si no pudo ser asignado
        // calcula hasta dónde llegará un nuevo segmento a continuación del último
    	uint32_t mem = s->mem + s->tam + METAD + tam;
        // si hay espacio crea el nuevo segmento y le asigna el bloque
        if(mem < MEMORIA.tamMemoria) {
            s->next = nuevoSegmento(programId, s->mem + s->tam, s->next);
            s = s->next;
            b = alojarBloque(s, tam);
            s->progId = programId;
        }
    }
    s->memLiberada = 0;
    free(s);
    // si no logra asignar el bloque es porque se agotó la memoria disponible
    if(b){
    	funMem = iniciar_logger_fun();
    	log_info(funMem, "Alloc realizado con exito. Proceso: %d", programId);
    	free(funMem);
    	return b->data;
    }else{
    	return 0; //sacar al resolver el tema de clock
    	//activar clock moficado wachines
    }
    //return b ? b->data : 0; //si retorna 0 es segmentation fault
}

*/