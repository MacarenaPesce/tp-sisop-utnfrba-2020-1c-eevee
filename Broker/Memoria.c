#include "Memoria.h"


//ASCII boquitas en hexa
//control + shift + u 3e y 3c 


//--------------- ADMINISTRACION DE MEMORIA-------------------

/*Dado el valor de memoria inicial, asigno un bloque para la memoria inicial y lo retorno*/
t_bloque_memoria* asignar_memoria_inicial(int tamanio_en_bytes){

    log_info(broker_logger, "Por asignar la memoria inicial");

    /* Asigno la memoria a un puntero auxiliar y la inicializo en cero */
    void* memoria_inicial = malloc(tamanio_en_bytes*sizeof(char));    
    memset(memoria_inicial, 0, tamanio_en_bytes*sizeof(char));
   
    /* Genero el bloque de memoria inicial*/
    t_bloque_memoria *bloque;
    bloque = (t_bloque_memoria*)malloc(sizeof(t_bloque_memoria));

    bloque->id = 0;
    bloque->tamanio = tamanio_en_bytes;
    bloque->esta_vacio = true;
    bloque->payload = memoria_inicial;
	bloque->timestamp = 0;
	bloque->last_time = 0;

  
    /* Agrego el bloque a la lista */
    list_add(lista_memoria,bloque);

    log_info(broker_logger, "Ya asigne la memoria inicial y cree el primer bloque");

    /* Retorno la variable inicial para guardarla en la global */
    return bloque;
}

/*Dado un tamaño de particion, devuelvo el bloque asignado y derivo segun el algoritmo de memoria 
	Ya corri todos los algoritmos aca. 
	Esta deberia ser la que tiene que llamar la cola de mensajes*/
t_bloque_memoria* asignar_particion_memoria(int tamanio_msje){
    
    log_info(broker_logger, "Por asignar memoria a mi particion");

    //Creo una nueva particion que es la que voy a devolver luego de asignar la particion
    t_bloque_memoria* nuevaParticion;
    //Asigno un bloque segun el algoritmo de memoria que utilicemos
    nuevaParticion = algoritmo_de_memoria(tamanio_msje);

    log_info(broker_logger, "Ya le asigne memoria a mi particion!");

    return nuevaParticion;
}

//-----------------------------ELECCION DE PARTICION LIBRE--------------------

t_bloque_memoria* algoritmo_de_particion_libre(int tamanio_msje, int tamanio_parti){
    
    t_bloque_memoria* bloque;

    log_info(broker_logger, "Por ejecutar algoritmo de particion libre");

    if( algoritmo_particion_libre == "FF"){
        bloque = algoritmo_first_fit(tamanio_msje, tamanio_parti);
    }
    else{
        bloque = algoritmo_best_fit(tamanio_msje, tamanio_parti);
    }

    log_info(broker_logger, "Algoritmo de particion libre ejecutado!");

    return bloque;

}

//------------------------------FIRST FIT------------------------------------
/* Busco el primer hueco donde puedo alojar una nueva particion y la alojo.
    NOTA: para esto ya esta chequeado que puede alojarse
    NOTA2: recibo el tamaño correcto, ya chequeado si es el minimo a alojar o no.*/
t_bloque_memoria* algoritmo_first_fit(int tamanio_msje, int tamanio_parti){

    t_bloque_memoria* bloque;
    t_bloque_memoria* aux;
    t_bloque_memoria* bloque_final;
    int indice; 

    log_info(broker_logger, "Ejecutando first fit");

    //obtengo el primer bloque donde quepa mi particion nueva
	for(int i=0; i< list_size(lista_memoria); i++){

        aux = list_get(lista_memoria, i);

        if((aux->tamanio >= tamanio_parti) && (aux->esta_vacio == true)){
            bloque = aux;
            i = list_size(lista_memoria);
            break;
        }

    }

    //obtengo el indice del bloque que voy a particionar
    indice = obtener_indice_particion(bloque);

    log_info(broker_logger, "Por particionar el bloque, ya encontre mi bloque adecuado");

    //particiono el bloque donde voy a alojar mi particion, PERO con el tamaño actualizado
    bloque_final = particionar_bloque(tamanio_parti,indice,tamanio_msje);

    log_info(broker_logger, "Ya ejecute el algoritmo fist fit");

    return bloque_final;

}

//------------------------------BEST FIT-------------------------------------
/* Busco el huevo mas pequeño donde quepa la nueva particion a alojar, y la alojo.
    NOTA: para esto ya esta chequeado que puede alojarse
    NOTA2: recibo el tamaño correcto, ya chequeado si es el minimo a alojar o no.*/
t_bloque_memoria* algoritmo_best_fit(int tamanio_msje, int tamanio_parti){

    t_bloque_memoria* bloque;
    t_bloque_memoria* aux;
    t_bloque_memoria* bloque_final;
    int indice;
    int tam_minimo=0; 

    log_info(broker_logger, "Ejecutando best fit");


    //obtengo el primer bloque donde quepa mi particion nueva
	for(int i=0; i<list_size(lista_memoria); i++){

        //Obtengo el bloque en la posicion de la lista que estamos
        aux = list_get(lista_memoria, i);

        //me fijo si el tamaño que quiero alojar cabe o no en el bloque actual y ademas si el bloque en el que estoy esta vacio o no 
        if((aux->esta_vacio == true) && (aux->tamanio >= tamanio_parti)){
            
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

    log_info(broker_logger, "Por particionar el bloque, ya encontre mi bloque adecuado");

    //particiono el bloque donde voy a alojar mi particion, PERO con el tamaño actualizado
    bloque_final = particionar_bloque(tamanio_parti,indice,tamanio_msje);

    log_info(broker_logger, "Ya ejecute best fit");

    return bloque_final;

}

//------------------------------FIN DE ELECCION DE PARTICION------------------




//----------------------------ALGORITMOS DE MEMORIA-------------------------

/*Segun el algoritmo que me pasa el archivo de configuracion, designo con esta funcion
    que algoritmo se va a usar y le paso la lista y el tamaño de bytes*/
t_bloque_memoria* algoritmo_de_memoria(int tamanio_msje){

    //Creo una nueva particion, que es donde se van a guardar los datos de la particion alojada
    //y la particion que voy a retornar
    t_bloque_memoria* particionNueva;
    
    //segun el algoritmo del archivo de configuracion, utilizo un algoritmo
    if (algoritmo_memoria == "BD"){
        particionNueva= buddy_system(tamanio_msje);
    }
    else{
        particionNueva= particiones_dinamicas(tamanio_msje);
    }

    return particionNueva;
}

//---------------------------PARTICIONES DINAMICAS CON COMPACTACION----------------------
t_bloque_memoria* particiones_dinamicas( int tamanio_msje){

    log_info(broker_logger, "Ejecutando particiones dinamicas");

    t_bloque_memoria* particionNueva;

    //me fijo si el tamaño del mensaje es menor al minimo tamaño de particion
    int tamanio_parti = tamanio_a_alojar(tamanio_msje);

    //me fijo si la particion puede alojarse a la primera
    bool sePuedeAlojar = puede_alojarse(tamanio_parti);

    bool alojado = false

    //igualo la frecuencia para compactar segun cuantas veces compacte
    int frec_para_compactar = frecuencia_compactacion; 

    
    while(alojado == false){  //mientras no  este alojado hago la secuencia pueda alojarlo tengo que ir vaciando particiones y fijandome si tengo que compactar

        //me fijo si puedo alojarla a la primera
        if(sePuedeAlojar == true){ 
            //si puede alojarse a la primera llamo al algoritmo de particion libre
            particionNueva = algoritmo_de_particion_libre(tamanio_msje, tamanio_parti);

            log_info(broker_logger, "Aloje la nueva particion , PD");

            //seteo alojado en true, para salir del while
            alojado = true;
        }
        else{ //si no puede alojarse a la primera ....

            //me fijo si la frecuencia de compactacion esta habilitada para seguir vaciando particiones
            if(frec_para_compactar == -1){ //si la frecuencia esta seteada en -1
                //corro el algoritmo y tengo que compactar si o si 
                algoritmo_de_reemplazo();

                log_info(broker_logger, "Por compactar...");

                compactar();
            }
            else if(frec_para_compactar > 0){ //en caso de estar habilitada la frecuencia
                algoritmo_de_reemplazo();
                frec_para_compactar - 1;
            }
            else{  //en caso de no estar habilitada , porque ya se agoto

                log_info(broker_logger, "Por compactar...");

                compactar();
                //seteo de nuevo la frecuencia para la prox compactacion
                frec_para_compactar=frecuencia_compactacion;
            }

            //me fijo de nuevo si puede alojarse
            sePuedeAlojar = puede_alojarse(tamanio_parti);
        }

    }

    return particionNueva;

}


//------------------------------BUDDY SYSTEM----------------------------------
t_bloque_memoria* buddy_system( int tamanio_en_bytes){

    log_info(broker_logger, "Ejecutando buddy system");

    //int bytes_reservar= numero_potencia_dos(int tamanio_en_bytes)

    t_bloque_memoria* particionNueva;

    return t_bloque_memoria;
    
}

//---------------------------FIN DE ALGORITMOS DE MEMORIA-----------------------




//----------------------------ELIMINAR UNA PARTICION-----------------------

void algoritmo_de_reemplazo(){

    //segun el algoritmo del archivo de configuracion, utilizo un algoritmo
    if (algoritmo_reemplazo == "LRU"){
        algoritmo_lru();
    }
    else{
        algoritmo_fifo();
    }

    return;

}

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

                bloque=elemento;

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

    t_bloque_memoria* elemento;
    t_bloque_memoria* bloque;
    uint64_t min_time = get_timestamp();
    int indice;

	for(int i=0; i< list_size(lista_memoria); i++){

        elemento = list_get(lista_memoria, i);

        //me fijo si el elemento actual de la lista vacio y si el timestamp es mayor a cero
        if( (elemento->esta_vacio == false) && (elemento->timestamp > 0) ){
            
            //si el timestamp de la ultima vez usado, es menor al minimo time, lo guardo y me guardo el bloque
            if(elemento->last_time < min_time){
                
                min_time= elemento->last_time;

                bloque=elemento;

            }
        }

    }

    //obtengo el indice del bloque que voy a particionar
    indice = obtener_indice_particion(bloque);
    
    //libero la memoria de un determinado bloque de mi lista , y me lo devuelve
    liberar_bloque_memoria(bloque,indice);
    

    return ;
}

//--------------------------FIN DE LRU--------------------------------

//------------------------FIN ELIMINAR UNA PARTICION-----------------------




//----------------------------DUMP DE LA MEMORIA--------------------------



//-----------------------------FIN DEL DUMP--------------------------------