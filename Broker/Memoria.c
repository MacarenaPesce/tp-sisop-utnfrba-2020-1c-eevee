#include "Memoria.h"
extern int tamanio_memoria;
extern int tamanio_minimo_particion;
extern char* algoritmo_memoria;
extern char* algoritmo_reemplazo;
extern char* algoritmo_particion_libre;
extern char* ip_broker;
extern int frecuencia_compactacion;
extern char* log_file;

extern t_log* broker_logger;
extern t_config* config;

extern t_cache_colas* cache_mensajes;

//ASCII boquitas en hexa
//control + shift + u 3e y 3c 

/* 

    Lucas 23/6:
    - Cambie lista_memoria por cache_mensajes->memoria (es la lista de bloques que tiene que quedar)
    - Cambie los parametros tamanio_mensaje y mensaje por estructura_mensaje en las fn:
        - particionar_bloque
        - algoritmo_de_memoria
        - particiones_dinamicas
        - buddy_system
        - algoritmo_de_particion_libre
        - algoritmo_first_fit
        - algoritmo_best_fit
    - Cambie los headers segun los cambios de las fn del punto anterior
    - Cambie la asignacion y copia del mensaje en la fn 'particionar_bloque'
    - Agregue un malloc que faltaba al crear la particion restante en la fn 'particionar_bloque' 
    - Quite el '+1' en el calculo de particion restante en la fn 'particionar_bloque'
    - Cambie varios bloque->payload por bloque->estructura_mensaje
    - Cambie varios bloque->tamanio_mensaje por bloque->estructura_mensaje->tamanio_mensaje
   
    
    PD: No me mates D:
*/


//--------------------------- ADMINISTRACION DE MEMORIA-------------------

/*Dado el valor de memoria inicial, asigno un bloque para la memoria inicial y lo retorno*/
void asignar_memoria_inicial(int tamanio_en_bytes){

    
    if(debug_broker) log_debug(broker_logger, "la direccion de memoria en memoria l51 es: %d ",cache_mensajes);

    //log_info(broker_logger, "Por asignar la memoria inicial");
    cache_mensajes->memoria = list_create();

    /* Asigno la memoria a un puntero auxiliar y la inicializo en cero */
    void* memoria_inicial = malloc(tamanio_en_bytes*sizeof(char));    
    memset(memoria_inicial, 0, tamanio_en_bytes*sizeof(char));
   
    if(debug_broker) log_debug(broker_logger, "Se asigno la memoria inicial",NULL);
    //if(debug_broker) log_debug(broker_logger, "La direccion inicial de memoria es: %p", memoria_inicial);
    log_warning(broker_logger, "La direccion inicial de memoria es: %p", memoria_inicial);

    /* Genero el bloque de memoria inicial*/
    t_bloque_memoria *bloque;
    bloque = (t_bloque_memoria*)malloc(sizeof(t_bloque_memoria));
    
    //bloque->id = 0;
    bloque->tamanio_particion = tamanio_en_bytes;
    bloque->esta_vacio = true;
	bloque->timestamp = 0;
	bloque->last_time = 0;
    bloque->estructura_mensaje = (t_mensaje_cola*) memoria_inicial;

    //if(debug_broker) log_debug(broker_logger, "Particion de memoria inicial creada con: %i \n", bloque->tamanio_particion );
    log_warning(broker_logger, "Particion de memoria inicial creada con: %i \n", bloque->tamanio_particion );

    /* Agrego el bloque a la lista */
    list_add(cache_mensajes->memoria,bloque);

    //log_info(broker_logger, "Ya asigne la memoria inicial y cree el primer bloque");

    /* Retorno la variable inicial para guardarla en la global */
    return ;
}




/*Dado un tamaño de particion, devuelvo el bloque asignado y derivo segun el algoritmo de memoria 
	Ya corri todos los algoritmos aca. 
	Esta deberia ser la que tiene que llamar la cola de mensajes*/
void asignar_particion_memoria(t_mensaje_cola* estructura_mensaje){
    
    /* Corro el algoritmo de memoria */
    algoritmo_de_memoria(estructura_mensaje);

    return ;
}



//----------------------------ALGORITMOS DE MEMORIA-------------------------


/*Segun el algoritmo que me pasa el archivo de configuracion, designo con esta funcion
    que algoritmo se va a usar y le paso la lista y el tamaño de bytes*/
void algoritmo_de_memoria(t_mensaje_cola* estructura_mensaje){   

    //segun el algoritmo del archivo de configuracion, utilizo un algoritmo
    if (  strcmp( algoritmo_memoria, "BS") == 0){

        buddy_system(estructura_mensaje);
    }
    else{
        particiones_dinamicas(estructura_mensaje);
    }

    return ;

}




//---------------------------PARTICIONES DINAMICAS CON COMPACTACION----------------------
void particiones_dinamicas( t_mensaje_cola* estructura_mensaje){

    //log_info(broker_logger, "Ejecutando particiones dinamicas");

    if(debug_broker) log_debug(broker_logger,"Particiones dinamicas", NULL);
    if(debug_broker) log_debug(broker_logger,"Alojar: %i", estructura_mensaje->tamanio_mensaje );


    /* Me fijo si el tamaño del mensaje es menor al minimo tamaño de particion */
    int tamanio_parti = tamanio_a_alojar(estructura_mensaje->tamanio_mensaje);
    //if(debug_broker) log_debug(broker_logger,"Por alojar particion de: %i ", tamanio_parti );
    log_warning(broker_logger, "Por alojar particion de: %i", tamanio_parti);

    /* Me fijo si la particion puede alojarse a la primera */
    bool sePuedeAlojar = puede_alojarse(tamanio_parti);

    /* Seteo un bool para controlar cuando ya aloje la particion*/
    bool alojado = false;

    //igualo la frecuencia para compactar segun cuantas veces compacte
    int frec_para_compactar = frecuencia_compactacion; 

    
    while(alojado == false){  //mientras no  este alojado hago la secuencia pueda alojarlo tengo que ir vaciando particiones y fijandome si tengo que compactar

        //me fijo si puedo alojarla a la primera
        if(sePuedeAlojar == true){ 
            //si puede alojarse a la primera llamo al algoritmo de particion libre
            algoritmo_de_particion_libre( tamanio_parti, estructura_mensaje);

            if(debug_broker) log_debug(broker_logger, "Aloje la nueva particion , PD");

            //seteo alojado en true, para salir del while
            alojado = true;

            continue;
        }


        //me fijo si la frecuencia de compactacion esta habilitada para seguir vaciando particiones
        if(frec_para_compactar == -1){ //si la frecuencia esta seteada en -1
            //corro el algoritmo y tengo que compactar si o si 
            algoritmo_de_reemplazo();

            log_info(broker_logger, "Por compactar -1...",NULL);

            compactar();
        }
        else if(frec_para_compactar > 0){ //en caso de estar habilitada la frecuencia
            log_info(broker_logger, "Por compactar mayor 0...",NULL);
            algoritmo_de_reemplazo();
            frec_para_compactar = frec_para_compactar - 1;
        }
        else{  //en caso de no estar habilitada , porque ya se agoto

            log_info(broker_logger, "Por compactar default...",NULL);

            compactar();
            //seteo de nuevo la frecuencia para la prox compactacion
            frec_para_compactar = frecuencia_compactacion;
        }

        //me fijo de nuevo si puede alojarse
        sePuedeAlojar = puede_alojarse(tamanio_parti);
        

    }

    return ;

}




//------------------------------BUDDY SYSTEM----------------------------------
void buddy_system( t_mensaje_cola* estructura_mensaje){

    if(debug_broker) log_debug(broker_logger, "Ejecutando buddy system");

    buddy_funcionamiento( estructura_mensaje);

    return ;
    
}


//---------------------------FIN DE ALGORITMOS DE MEMORIA-----------------------




//-----------------------------ELECCION DE PARTICION LIBRE--------------------


void algoritmo_de_particion_libre(int tamanio_parti, t_mensaje_cola* estructura_mensaje){
    
    //log_info(broker_logger, "Por ejecutar algoritmo de particion libre %s",algoritmo_particion_libre);

    if( strcmp( algoritmo_particion_libre, "FF") == 1){
        algoritmo_first_fit(tamanio_parti, estructura_mensaje);
    }
    else{
        algoritmo_best_fit(tamanio_parti, estructura_mensaje);
    }

    //if(debug_broker) log_debug(broker_logger, "Algoritmo de particion libre ejecutado!");

    return ;

}


//------------------------------FIRST FIT------------------------------------
/* Busco el primer hueco donde puedo alojar una nueva particion y la alojo.
    NOTA: para esto ya esta chequeado que puede alojarse
    NOTA2: recibo el tamaño correcto, ya chequeado si es el minimo a alojar o no.*/
void algoritmo_first_fit(int tamanio_parti,t_mensaje_cola* estructura_mensaje){

    t_bloque_memoria* bloque;
    t_bloque_memoria* aux;
    int indice=0; 

    if(debug_broker) log_debug(broker_logger, "Ejecutando first fit");

    //obtengo el primer bloque donde quepa mi particion nueva
	for(int i=0; i< list_size(cache_mensajes->memoria); i++){

        aux = list_get(cache_mensajes->memoria, i);

        if((aux->tamanio_particion >= tamanio_parti) && (aux->esta_vacio == true)){
            bloque = aux;
            indice = i;
            break;
        }

    }

    if(debug_broker) log_debug(broker_logger, "Por particionar el bloque, ya encontre mi bloque adecuado");

    //particiono el bloque donde voy a alojar mi particion, PERO con el tamaño actualizado
    particionar_bloque(tamanio_parti,indice,estructura_mensaje);

    if(debug_broker) log_debug(broker_logger, "Ya ejecute el algoritmo fist fit");

    return ;

}




//------------------------------BEST FIT-------------------------------------
/* Busco el huevo mas pequeño donde quepa la nueva particion a alojar, y la alojo.
    NOTA: para esto ya esta chequeado que puede alojarse
    NOTA2: recibo el tamaño correcto, ya chequeado si es el minimo a alojar o no.*/
void algoritmo_best_fit(int tamanio_parti, t_mensaje_cola* estructura_mensaje){

    t_bloque_memoria* bloque;
    t_bloque_memoria* aux;
    int indice;
    int tam_minimo=0; 

    if(debug_broker) log_debug(broker_logger, "Ejecutando best fit");


    //obtengo el primer bloque donde quepa mi particion nueva
	for(int i=0; i<list_size(cache_mensajes->memoria); i++){

        //Obtengo el bloque en la posicion de la lista que estamos
        aux = list_get(cache_mensajes->memoria, i);

        //me fijo si el tamaño que quiero alojar cabe o no en el bloque actual y ademas si el bloque en el que estoy esta vacio o no 
        if((aux->esta_vacio == true) && (aux->tamanio_particion >= tamanio_parti)){
            
            //me fijo si el tamaño mas chico de particiones sigue siendo 0 
            if(tam_minimo==0){
                tam_minimo = aux->tamanio_particion;
            }

            //me fijo si el tamaño mas chico de particion es menor o igual al tamaño del bloque auxiliar actual, en caso de serlo, al bloque le asigno el aux
            //y asi me quedo con el bloque mas chico de toda la lista
            if(tam_minimo <= aux->tamanio_particion){
                bloque = aux;
            }

        }

    }

    //obtengo el indice del bloque que voy a particionar
    indice = obtener_indice_particion(bloque);

    if(debug_broker) log_debug(broker_logger, "Por particionar el bloque, ya encontre mi bloque adecuado");

    //particiono el bloque donde voy a alojar mi particion, PERO con el tamaño actualizado
    particionar_bloque(tamanio_parti,indice,estructura_mensaje);

    if(debug_broker) log_debug(broker_logger, "Ya ejecute best fit");

    return ;

}

//------------------------------FIN DE ELECCION DE PARTICION------------------




//----------------------------ELIMINAR UNA PARTICION-----------------------


void algoritmo_de_reemplazo(){

    t_bloque_memoria* bloque;

    //segun el algoritmo del archivo de configuracion, utilizo un algoritmo
    if (strcmp( algoritmo_reemplazo, "LRU") == 1){
        bloque = algoritmo_lru();
    }
    else{
        bloque = algoritmo_fifo();
    }

    //realizo la consolidacion siempre que corro el algoritmo de reemplazo
    consolidar(bloque);

    if(debug_broker) log_debug(broker_logger,"Ya consolide luego de vaciar una particion", NULL);

    return;

}


//------------------------------FIFO--------------------------------

/* elije como victima a la pagina que esta cargada en memoria hace mas tiempo*/
/* se puede implementar guardando el instante en el que la pagina fue cargada
    o con una cola fifo y se elige a la pagina que primero se agrego*/
t_bloque_memoria* algoritmo_fifo(){

    t_bloque_memoria* elemento;
    t_bloque_memoria* bloque;
    uint64_t min_time = get_timestamp();

    if(debug_broker) log_debug(broker_logger, "Ejecutando Algoritmo FIFO");

	for(int i=0; i< list_size(cache_mensajes->memoria); i++){

        elemento = list_get(cache_mensajes->memoria, i);

        //me fijo si el elemento actual de la lista vacio y si el timestamp es mayor a cero
        if( (elemento->esta_vacio == false) && (elemento->timestamp > 0) ){
            
            //si el timestamp, es menor al minimo time, lo guardo y me guardo el bloque
            if(elemento->timestamp < min_time){
                
                min_time= elemento->timestamp;

                bloque=elemento;

            }
        }

    }
    
    log_info(broker_logger, "Eliminado de una particion en la posicion %p", bloque->estructura_mensaje->mensaje);

    //libero la memoria de un determinado bloque de mi lista , y me lo devuelve
    liberar_bloque_memoria(bloque);

    if(debug_broker) log_debug(broker_logger, "Espacio liberado..");

    return bloque;
}




//------------------------------LRU--------------------------------

/* elije como victima a la pagina menos reciente utilizada*/
/* se puede implementar guardando el instante de ultima referencia a la pagina 
    o con una pila con numeros de paginas, con cada referencia se coloca la pag
    superior, se elige como victima la pag de la parte inferior*/
t_bloque_memoria* algoritmo_lru(){

    t_bloque_memoria* elemento;
    t_bloque_memoria* bloque;
    uint64_t min_time = get_timestamp();

    if(debug_broker) log_debug(broker_logger, "Ejecutando Algoritmo LRU");

	for(int i=0; i< list_size(cache_mensajes->memoria); i++){

        elemento = list_get(cache_mensajes->memoria, i);

        //me fijo si el elemento actual de la lista vacio y si el timestamp es mayor a cero
        if( (elemento->esta_vacio == false) && (elemento->timestamp > 0) ){
            
            //si el timestamp de la ultima vez usado, es menor al minimo time, lo guardo y me guardo el bloque
            if(elemento->last_time < min_time){
                
                min_time= elemento->last_time;

                bloque=elemento;

            }
        }

    }

    log_info(broker_logger, "Eliminado de una particion en la posicion %p", bloque->estructura_mensaje->mensaje);

    //libero la memoria de un determinado bloque de mi lista , y me lo devuelve
    liberar_bloque_memoria(bloque);
    
    if(debug_broker) log_debug(broker_logger, "Espacio liberado..");


    return bloque;
}


//------------------------FIN ELIMINAR UNA PARTICION-----------------------



//-----------------------------AUXILIARES DE PD-----------------------------------


/*Dado un indice y un tamaño en byte, alojo la particion en el indice, y creo el nuevo bloque con lo restante en el caso
	que haya algo restante. La idea de esta funcion es que sea llamada por el algoritmo de asignacion.
	Se usaria una vez encontrado el lugar en memoria que ocuparia mi nueva particion */
t_bloque_memoria* particionar_bloque(int tamanio_parti, int indice_nodo_particionar, t_mensaje_cola* estructura_mensaje){


    if(debug_broker) log_debug(broker_logger,"indice a particionar %d",indice_nodo_particionar);
    t_bloque_memoria *bloque_restante;
	t_bloque_memoria *bloque_inicial;

    if(debug_broker) log_debug(broker_logger, "Por particionar...");

    /* Obtengo el nodo del bloque a particionar por su indice */
    bloque_inicial = list_get(cache_mensajes->memoria, indice_nodo_particionar);

    /* Si me sobra espacio lo separo en un nuevo nodo */
    if(bloque_inicial->tamanio_particion - tamanio_parti > 0){ 

        int tamanio_restante = bloque_inicial->tamanio_particion - tamanio_parti;
        void* particion_restante = ((char *)bloque_inicial->estructura_mensaje) + tamanio_parti;

        bloque_restante = crear_bloque_vacio(tamanio_restante, particion_restante);

        list_add_in_index(cache_mensajes->memoria, indice_nodo_particionar + 1, bloque_restante);    

    }

    log_info(broker_logger, "Almacenado mensaje en la posicion %p", bloque_inicial->estructura_mensaje);

    /* Seteo el nodo inicial como ocupado , y actualizo el tamaño */
    bloque_inicial->tamanio_particion = tamanio_parti;
    bloque_inicial->esta_vacio = false;
	bloque_inicial->timestamp = get_timestamp();
	bloque_inicial->last_time = get_timestamp();


    /* Copio el mensaje a MP y apunto a la estructura_mensaje */      
    memcpy((void*)(bloque_inicial->estructura_mensaje),estructura_mensaje->mensaje,estructura_mensaje->tamanio_mensaje);

    void* aux_mensaje = bloque_inicial->estructura_mensaje; 
    bloque_inicial->estructura_mensaje = estructura_mensaje;

    bloque_inicial->estructura_mensaje->mensaje = aux_mensaje;    

    if(debug_broker) log_debug(broker_logger, "Bloque particionado...");

    return bloque_inicial;
}




/* Se encarga de realizar la compactacion en particiones dinamicas*/
void compactar(){

    if(debug_broker) log_debug(broker_logger, "Ejecutando compactación");

    void compactar_bloque(void* _bloque){

        t_bloque_memoria* bloque = (t_bloque_memoria*) _bloque;

        //si el bloque no esta vacio no hago nada
        if(!bloque->esta_vacio) return ;

        int indice = obtener_indice_particion(bloque);

        t_bloque_memoria* bloque_siguiente = list_get(cache_mensajes->memoria,indice+1);

        if(bloque_siguiente == NULL) return;

        //si el bloque siguiente esta vacio
        if(bloque_siguiente->esta_vacio){
            consolidar_dos_bloques(bloque, bloque_siguiente);
        }else {

            /* clonar bloque memoria */
            t_bloque_memoria* bloque_auxiliar = (t_bloque_memoria*)malloc(sizeof(t_bloque_memoria));
            memcpy(bloque_auxiliar,bloque_siguiente,sizeof(t_bloque_memoria));
            bloque_auxiliar->estructura_mensaje = bloque_siguiente->estructura_mensaje;

            /* clonar mensaje de memoria principal */
            void* auxiliar_mensaje = malloc(sizeof(bloque_siguiente->tamanio_particion));
            memcpy(auxiliar_mensaje, bloque_siguiente->estructura_mensaje->mensaje,bloque_siguiente->tamanio_particion);
            
            /* reapuntar estructuras de mensajes */
            bloque_siguiente->estructura_mensaje = bloque_siguiente->estructura_mensaje->mensaje;
            bloque_auxiliar->estructura_mensaje->mensaje = auxiliar_mensaje;

            /* liberar el siguiente bloque y consolida */
            liberar_bloque_memoria(bloque_siguiente);
            consolidar_dos_bloques(bloque, bloque_siguiente);

            /* particiona el bloque resultante de consolidar, reinsertando los datos a izq */
            particionar_bloque(bloque_auxiliar->tamanio_particion,indice, bloque_auxiliar->estructura_mensaje);
            
            /* obtengo el nuevo bloque y seteo los viejos timestamp*/
            bloque = list_get(cache_mensajes->memoria,indice);
            bloque->timestamp = bloque_auxiliar->timestamp;
            bloque->last_time = bloque_auxiliar->last_time;

            /* libero los auxiliares */
            free(auxiliar_mensaje);
            free(bloque_auxiliar);
        
        }


    }    

    list_iterate(cache_mensajes->memoria, compactar_bloque);

    if(debug_broker) log_debug(broker_logger, "Compactacion terminada");

	return ;
}




/* Se encarga de realizar la consolidacion en Particiones dinamicas*/
void consolidar(t_bloque_memoria* bloque){
    
    int indice_bloque = obtener_indice_particion(bloque);

    t_bloque_memoria* bloque_anterior = list_get(cache_mensajes->memoria,(indice_bloque-1));

    t_bloque_memoria* bloque_siguiente = list_get(cache_mensajes->memoria,(indice_bloque+1));

    if(bloque_siguiente != NULL && bloque_siguiente->esta_vacio == true){
        consolidar_dos_bloques(bloque,bloque_siguiente);
    }
    if(bloque_anterior != NULL && bloque_anterior->esta_vacio == true){
        consolidar_dos_bloques(bloque_anterior,bloque);
    }

    return ;
}

void consolidar_dos_bloques(t_bloque_memoria* primerBloque, t_bloque_memoria* segundoBloque){

    /* Obtengo el indice del segundo bloque */
    int indice = obtener_indice_particion(segundoBloque);

    /* Modifico el tamaño del primer bloque */
    primerBloque->tamanio_particion += segundoBloque->tamanio_particion;

    /* Borro y destruyo el segundo bloque */
	list_remove_and_destroy_element(cache_mensajes->memoria, indice, (void*)free);

    return ;
}




//----------------------------DUMP DE LA MEMORIA--------------------------



//-----------------------------FIN DEL DUMP--------------------------------