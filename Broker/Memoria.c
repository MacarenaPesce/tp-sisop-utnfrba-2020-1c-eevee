#include "Memoria.h"

//--------------- ADMINISTRACION DE MEMORIA-------------------


t_bloque_memoria* AsignarMemoriaInicial(int tamanio_en_bytes, t_list* lista_memoria){
    
    /* Asigno la memoria a un puntero auxiliar y la inicializo en cero */
    void* memoria_inicial = malloc(tamanio_en_bytes*sizeof(char));    
    memset(memoria_inicial, 0, tamanio_en_bytes*sizeof(char));
   
    /* Genero el bloque de memoria inicial*/
    t_bloque_memoria *bloque;
    bloque = (t_bloque_memoria*)malloc(sizeof(t_bloque_memoria));

    bloque->tamanio = tamanio_en_bytes;
    bloque->esta_vacio = true;
    bloque->payload = memoria_inicial;
  
    /* Agrego el bloque a la lista */
    list_add(lista_memoria,bloque);

    /* Retorno la variable inicial para guardarla en la global */
    return bloque;
}

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

//el indice lo tengo que buscar por ej con el payload 
void _ParticionarBloqueMemoria(t_list* lista_memoria, int indice_nodo_particionar, int tamanio){

    t_bloque_memoria *bloque_restante;
    t_bloque_memoria *bloque_inicial;

    /* Obtengo el nodo del bloque a particionar por su indice */
    bloque_inicial = list_get(lista_memoria, indice_nodo_particionar);

    /* Si me sobra espacio lo separo en un nuevo nodo */
    if(bloque_inicial->tamanio - tamanio > 0){ 

        bloque_restante->tamanio = bloque_inicial->tamanio - tamanio;
        bloque_restante->esta_vacio = true;
        bloque_restante->payload = bloque_inicial+tamanio;

        list_add_in_index(lista_memoria, indice_nodo_particionar + 1, bloque_restante);    

    }

    /* Seteo el nodo inicial como ocupado */
    bloque_inicial->tamanio = tamanio;
    bloque_inicial->esta_vacio = false;

    return;
}


void* EncontrarBloqueMemoriaLibre(char* bloque_memoria_inicial, int tamanio_en_bytes){


    return ;
}


//----------------------------ALGORITMOS DE MEMORIA-------------------------
void* algoritmo_de_memoria(int tamanio_en_bytes){

    //Creo una nueva particion, que es donde se van a guardar los datos de la particion alojada
    //y la particion que voy a retornar
    t_particion* particionNueva;
    
    //segun el algoritmo del archivo de configuracion, utilizo un algoritmo
    if (algoritmo_memoria == "BD"){
        particionNueva= buddy_system(tamanio_en_bytes);
    }
    else{
        particionNueva= particiones_dinamicas(tamanio_en_bytes);
    }

    return particionNueva;
}

//---------------------------PARTICIONES DINAMICAS CON COMPACTACION----------------------
void* particiones_dinamicas( int tamanio_en_bytes){

    t_particion* particionNueva;

    bool sePuedeAlojar = puedeAlojarse(tamanio_en_bytes);


    return particionNueva;

}


//------------------------------BUDDY SYSTEM----------------------------------
void* buddy_system( int tamanio_en_bytes){

    //int bytes_reservar= numero_potencia_dos(int tamanio_en_bytes)

    t_particion* particionNueva;

    return particionNueva;
    
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